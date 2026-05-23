# 设备认证改进设计文档

**日期**: 2026-05-22
**状态**: 已批准
**方案**: 方案 A - 扩展 device.auth.approve 协议

---

## 1. 概述

### 1.1 问题描述

当前设备连接服务端时，如果设备不存在于数据库中，认证直接失败，管理端无法感知新设备的接入。

### 1.2 目标

1. 设备连接时检测数据库中是否存在该设备
2. 如果不存在，创建待认证记录并通知管理端
3. 管理员可以在管理端认证设备并设置密钥
4. 密钥下发到设备端并持久化保存

---

## 2. 数据流

```
┌──────────┐      ┌──────────┐      ┌──────────┐
│  设备端   │      │  服务端   │      │  管理端   │
└────┬─────┘      └────┬─────┘      └────┬─────┘
     │                 │                 │
     │ 1. auth 请求    │                 │
     │────────────────>│                 │
     │                 │                 │
     │                 │ 2. 检查设备表    │
     │                 │ 不存在          │
     │                 │                 │
     │                 │ 3. 创建 pending_auth
     │                 │    记录         │
     │                 │                 │
     │                 │ 4. 推送事件     │
     │                 │────────────────>│
     │                 │  device.pending_auth.push
     │                 │                 │
     │ 5. auth 失败    │                 │ 6. 弹窗提醒
     │  等待审核提示   │                 │  新设备待认证
     │<────────────────│                 │
     │                 │                 │
     │                 │ 7. 管理员点击认证│
     │                 │    设置密钥     │
     │                 │<────────────────│
     │                 │  device.auth.approve
     │                 │  (含 deviceKey) │
     │                 │                 │
     │                 │ 8. 更新 key_hash│
     │                 │    注册设备     │
     │                 │                 │
     │ 9. 认证成功响应 │                 │
     │  (含新密钥)     │                 │
     │<────────────────│                 │
     │                 │                 │
     │ 10. 更新本地密钥│                 │
     │     保存到配置  │                 │
     │                 │                 │
     │ 11. 设备上线事件│                 │
     │                 │────────────────>│
     │                 │  device.status.push
     │                 │                 │
```

---

## 3. 协议改动

### 3.1 新增消息类型

**服务端 ProtocolTypes.hpp**:
```cpp
inline constexpr std::string_view kTypeDevicePendingAuthPush = "device.pending_auth.push";
```

### 3.2 device.pending_auth.push 消息格式

```json
{
  "type": "device.pending_auth.push",
  "role": "server",
  "from": "server",
  "ts": 1716345600000,
  "code": 0,
  "msg": "ok",
  "data": {
    "deviceId": "DEV-001",
    "ipAddress": "192.168.1.100",
    "status": "pending_auth"
  }
}
```

### 3.3 扩展 device.auth.approve 请求

```json
{
  "type": "device.auth.approve",
  "msgId": "admin-msg-001",
  "data": {
    "deviceId": "DEV-001",
    "deviceKey": "new-secret-key-123"
  }
}
```

- `deviceKey` 字段可选
- 如果提供，服务端将更新 `key_hash`
- 如果不提供，保持默认密钥（changeme）

### 3.4 扩展 auth 响应（设备端）

```json
{
  "type": "auth.response",
  "code": 0,
  "data": {
    "accessToken": "...",
    "refreshToken": "...",
    "deviceKey": "new-secret-key-123",
    "heartbeatSec": 30,
    "serverTime": 1716345600000
  }
}
```

---

## 4. 服务端改动

### 4.1 GatewaySessionHandler.cpp - handleDeviceAuth

**改动位置**: `handleDeviceAuth` 方法

**新增逻辑**:
1. 查询设备是否存在于数据库
2. 如果不存在或状态为 pending_auth:
   - 调用 `upsertDevicePendingAuth` 创建待认证记录
   - 存储到 `pending_device_auth_` 映射
   - 推送 `device.pending_auth.push` 事件到管理端
   - 返回认证失败（code=2002, msg="device pending authorization"）

### 4.2 GatewaySessionHandler.cpp - device.auth.approve

**改动位置**: `device.auth.approve` 处理逻辑

**新增逻辑**:
1. 从请求中提取 `deviceKey` 字段
2. 在数据库更新时，如果提供了 `deviceKey`:
   - 对密钥进行哈希处理
   - 更新 Device 表的 `key_hash` 字段
3. 在认证成功响应中，如果提供了 `deviceKey`:
   - 将明文密钥下发给设备端

### 4.3 DeviceRepository

**无需新增方法**，现有方法已满足需求:
- `upsertDevicePendingAuth` - 创建待认证记录
- `getDeviceStatus` - 查询设备状态
- `upsertDeviceOnline` - 更新设备为在线状态

---

## 5. 管理端改动

### 5.1 DeviceServer.h

**新增信号**:
```cpp
void newDevicePendingAuth(const QString &deviceId, const QString &ipAddress);
```

**新增方法**:
```cpp
void handleDevicePendingAuthPush(const QString &deviceId, const QString &ipAddress);
void approveDeviceWithKey(const QString &deviceId, const QString &deviceKey);
```

### 5.2 DeviceServer.cpp

**新增实现**:
1. `handleDevicePendingAuthPush` - 处理推送事件，发射信号
2. `approveDeviceWithKey` - 带密钥的认证方法

### 5.3 main.cpp

**新增连接**:
```cpp
QObject::connect(eventService, &EventSubscriptionService::serverPushReceived,
                 deviceServer, [deviceServer](const QString &messageType, const QVariantMap &data) {
                     if (messageType == "device.pending_auth.push") {
                         const QString deviceId = data.value("deviceId").toString();
                         const QString ipAddress = data.value("ipAddress").toString();
                         deviceServer->handleDevicePendingAuthPush(deviceId, ipAddress);
                     }
                 });
```

### 5.4 PageDevice.qml

**新增组件**:
1. `authDialog` - 认证对话框，包含密钥设置
2. Toast 提示 - 新设备待认证提醒

**新增连接**:
```qml
Connections {
    target: deviceServer
    function onNewDevicePendingAuth(deviceId, ipAddress) {
        toast.show(qsTr("新设备待认证: %1 (%2)").arg(deviceId).arg(ipAddress))
        authDialog.deviceId = deviceId
        authDialog.ipAddress = ipAddress
        authDialog.open()
    }
}
```

---

## 6. 设备端改动

### 6.1 Networkclient.h

**新增信号**:
```cpp
void devicePendingAuth();  // 设备待审核信号
```

### 6.2 Networkclient.cpp - handleAuthResponse

**新增逻辑**:
1. 检查认证失败消息是否包含 "pending authorization"
2. 如果是，发射 `devicePendingAuth` 信号
3. 认证成功时，检查响应中是否包含 `deviceKey`
4. 如果有，更新本地密钥并持久化

### 6.3 MainWindow.cpp

**新增连接**:
```cpp
connect(m_networkClient, &Networkclient::devicePendingAuth,
        this, [this]() {
            QMessageBox::information(this, tr("设备待审核"),
                tr("设备正在等待管理员审核，请稍后重试。"));
            QTimer::singleShot(30000, this, [this]() {
                m_networkClient->reconnect();
            });
        });
```

### 6.4 ConfigManager

**无需改动**，现有方法已支持:
- `setDeviceKey(const QString &key)` - 设置密钥
- `saveConfig()` - 保存到 config.ini

---

## 7. 错误处理

### 7.1 设备端

| 场景 | 处理方式 |
|------|----------|
| 认证失败（待审核） | 显示提示，30秒后自动重试 |
| 认证失败（其他原因） | 显示错误信息 |
| 密钥更新失败 | 记录日志，不影响认证流程 |

### 7.2 服务端

| 场景 | 处理方式 |
|------|----------|
| 创建 pending_auth 记录失败 | 记录日志，返回认证失败 |
| 更新 key_hash 失败 | 记录日志，认证仍成功 |
| 推送事件失败 | 记录日志，不影响认证流程 |

### 7.3 管理端

| 场景 | 处理方式 |
|------|----------|
| 接收推送失败 | 不影响现有功能 |
| 认证请求失败 | 显示错误信息 |
| 密钥为空 | 使用默认密钥（changeme） |

---

## 8. 测试要点

1. **新设备连接**: 设备不存在时，管理端应收到待认证通知
2. **管理员认证**: 点击认证后，设备应收到认证成功响应
3. **密钥下发**: 设置密钥后，设备端应收到并保存
4. **密钥验证**: 设备使用新密钥重新连接应成功
5. **重复连接**: 已认证设备再次连接应正常工作
6. **断线重连**: 设备断线后重连应使用保存的密钥

---

## 9. 文件清单

### 服务端
- `protocol/ProtocolTypes.hpp` - 新增消息类型常量
- `protocol/GatewaySessionHandler.cpp` - 修改 handleDeviceAuth 和 device.auth.approve

### 管理端
- `src/Device/DeviceServer.h` - 新增信号和方法
- `src/Device/DeviceServer.cpp` - 新增实现
- `main.cpp` - 新增信号连接
- `ui/pages/PageDevice.qml` - 新增认证对话框和提示

### 设备端
- `NetworkClient/networkclient.h` - 新增信号
- `NetworkClient/networkclient.cpp` - 修改 handleAuthResponse
- `mainwindow.cpp` - 新增信号连接
