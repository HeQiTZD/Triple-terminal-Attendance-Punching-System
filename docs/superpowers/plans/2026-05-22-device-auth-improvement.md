# 设备认证改进实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 实现设备连接时的自动检测、管理端通知、密钥下发功能

**Architecture:** 扩展 device.auth.approve 协议，添加 device.pending_auth.push 事件推送，实现密钥下发和持久化

**Tech Stack:** C++17, Qt 6, Boost.Asio, MySQL X DevAPI, nlohmann/json

---

## 文件结构

### 服务端 (D:\attendanceServer)
- `protocol/ProtocolTypes.hpp` - 新增消息类型常量
- `protocol/GatewaySessionHandler.cpp` - 修改 handleDeviceAuth 和 device.auth.approve
- `service/TokenManager.cpp` - 修改 verifyDeviceKey 支持 pending_auth 状态

### 管理端 (E:\project\AttendanceServer)
- `src/Device/DeviceServer.h` - 新增信号和方法
- `src/Device/DeviceServer.cpp` - 新增实现
- `main.cpp` - 新增信号连接
- `ui/pages/PageDevice.qml` - 新增认证对话框和提示

### 设备端 (D:\QTproject\AttendanceSystem)
- `NetworkClient/networkclient.h` - 新增信号
- `NetworkClient/networkclient.cpp` - 修改 handleAuthResponse
- `mainwindow.cpp` - 新增信号连接

---

## Task 1: 服务端 - 新增消息类型常量

**Files:**
- Modify: `D:\attendanceServer\protocol\ProtocolTypes.hpp:28-29`

- [ ] **Step 1: 在 ProtocolTypes.hpp 中添加新消息类型**

在 `kTypeDeviceStatusPush` 后添加：

```cpp
// 设备状态推送事件
inline constexpr std::string_view kTypeDeviceStatusPush = "device.status.push";

// 新设备待认证推送事件
inline constexpr std::string_view kTypeDevicePendingAuthPush = "device.pending_auth.push";
```

- [ ] **Step 2: 验证编译**

Run: `cd D:\attendanceServer && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 3: Commit**

```bash
cd D:\attendanceServer
git add protocol/ProtocolTypes.hpp
git commit -m "feat: add device.pending_auth.push message type constant"
```

---

## Task 2: 服务端 - 修改 handleDeviceAuth 支持新设备检测

**Files:**
- Modify: `D:\attendanceServer\protocol\GatewaySessionHandler.cpp:1365-1460`

- [ ] **Step 1: 读取当前 handleDeviceAuth 方法**

```bash
# 查看当前实现
cat -n D:\attendanceServer\protocol\GatewaySessionHandler.cpp | head -1460 | tail -100
```

- [ ] **Step 2: 修改 handleDeviceAuth 方法**

在 `handleDeviceAuth` 方法开头添加设备检测逻辑：

```cpp
void GatewaySessionHandler::handleDeviceAuth(net::ISession& session, const ParsedEnvelope& env) {
  const std::string device_id = opt_data_string(env.data, "deviceId");
  const std::string device_key = opt_data_string(env.data, "deviceKey");
  const std::string route_to = device_id;

  if (device_id.empty()) {
    session.write_line(build_error(kCodeAuthFailed, "missing deviceId", env.msg_id, route_to));
    return;
  }

  // 新增：获取设备 IP 地址
  const std::string ip_address = remote_ip_string(session);
  
  // 新增：检查设备是否存在于数据库
  bool device_exists = false;
  std::string device_status;
  try {
    auto promise = std::make_shared<std::promise<std::string>>();
    auto future = promise->get_future();
    db_.dispatch(
        [promise, device_id](mysqlx::Session& s) {
          std::string status = db::DeviceRepository::getDeviceStatus(s, device_id);
          promise->set_value(status);
        },
        [promise](std::exception_ptr ep) {
          if (ep) {
            try {
              promise->set_exception(ep);
            } catch (...) {}
          }
        });
    device_status = future.get();
    device_exists = !device_status.empty();
  } catch (const std::exception& ex) {
    std::cerr << "check device exists failed: " << ex.what() << '\n';
    session.write_line(build_error(kCodeDbError, "database error", env.msg_id, route_to));
    return;
  }

  // 新增：如果设备不存在或状态为 pending_auth，创建待认证记录并通知管理端
  if (!device_exists || device_status == "pending_auth") {
    // 创建待认证记录
    db_.dispatch(
        [device_id, ip_address](mysqlx::Session& s) {
          db::DeviceRepository::upsertDevicePendingAuth(s, device_id, ip_address);
        },
        [device_id](std::exception_ptr ep) {
          if (ep) {
            try {
              std::rethrow_exception(ep);
            } catch (const std::exception& ex) {
              std::cerr << "db: upsertDevicePendingAuth failed deviceId="
                        << device_id << " error=" << ex.what() << '\n';
            }
          }
        });

    // 存储到 pending_device_auth_ 以便后续 approve 使用
    {
      std::lock_guard<std::mutex> lk(mu_);
      pending_device_auth_[device_id] = {
        session.shared_from_this(),
        env.msg_id
      };
    }

    // 推送新设备待认证事件到管理端
    {
      nlohmann::json push_msg = {
          {"type", std::string(kTypeDevicePendingAuthPush)},
          {"role", std::string(kRoleServer)},
          {"from", std::string(kRoleServer)},
          {"ts", std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count()},
          {"code", 0},
          {"msg", "ok"},
          {"data", {{"deviceId", device_id}, {"ipAddress", ip_address}, {"status", "pending_auth"}}}};
      subscriptions_.publish("device", push_msg.dump());
    }

    // 返回认证失败，提示等待审核
    session.write_line(build_error(kCodeAuthFailed, "device pending authorization, please wait", env.msg_id, route_to));
    return;
  }

  // 继续现有的认证逻辑...
  auto result = tokenManager_->authenticateDevice(device_id, device_key);
  // ... 后续代码保持不变 ...
}
```

- [ ] **Step 3: 验证编译**

Run: `cd D:\attendanceServer && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 4: Commit**

```bash
cd D:\attendanceServer
git add protocol/GatewaySessionHandler.cpp
git commit -m "feat: add device existence check and pending_auth notification"
```

---

## Task 3: 服务端 - 修改 device.auth.approve 支持密钥下发

**Files:**
- Modify: `D:\attendanceServer\protocol\GatewaySessionHandler.cpp:820-920`

- [ ] **Step 1: 读取当前 device.auth.approve 处理逻辑**

```bash
cat -n D:\attendanceServer\protocol\GatewaySessionHandler.cpp | head -920 | tail -100
```

- [ ] **Step 2: 修改 device.auth.approve 处理逻辑**

在 `device.auth.approve` 处理中添加密钥处理：

```cpp
// 在处理 device.auth.approve 时，提取 deviceKey 字段
const std::string device_key = opt_data_string(env.data, "deviceKey");

// ... 现有的 pending_session 查找逻辑 ...

// 在数据库更新后，如果提供了 deviceKey，更新 key_hash
db_.dispatch(
    [device_id, device_key](mysqlx::Session& s) {
      db::DeviceRepository::upsertDeviceOnline(s, device_id, {});
      
      // 新增：如果提供了 deviceKey，更新 key_hash
      if (!device_key.empty()) {
        std::string key_hash = util::hash_password(device_key);
        s.sql("UPDATE Device SET key_hash = ? WHERE device_id = ?")
            .bind(key_hash, device_id)
            .execute();
      }
    },
    [this, admin_session, pending, device_id, pending_auth_msg_id, msg_id, to, device_key](std::exception_ptr ep) {
      // ... 现有的错误处理 ...
      
      // 构建设备端响应
      nlohmann::json data;
      data["accessToken"] = result.tokens.accessToken;
      data["refreshToken"] = result.tokens.refreshToken;
      data["tokenType"] = "Bearer";
      data["expiresIn"] = result.tokens.expiresIn;
      data["heartbeatSec"] = cfg_.heartbeat_sec;
      data["serverTime"] = util::JwtUtil::currentTimestamp() * 1000;
      data["roles"] = nlohmann::json::array();
      data["permissions"] = nlohmann::json::array({"attendance.report", "sync.request", "device.status.report"});
      
      // 新增：如果提供了 deviceKey，在响应中下发
      if (!device_key.empty()) {
        data["deviceKey"] = device_key;
      }
      
      // ... 发送响应 ...
    });
```

- [ ] **Step 3: 验证编译**

Run: `cd D:\attendanceServer && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 4: Commit**

```bash
cd D:\attendanceServer
git add protocol/GatewaySessionHandler.cpp
git commit -m "feat: add deviceKey support in device.auth.approve"
```

---

## Task 4: 管理端 - 新增 DeviceServer 信号和方法

**Files:**
- Modify: `E:\project\AttendanceServer\src\Device\DeviceServer.h`
- Modify: `E:\project\AttendanceServer\src\Device\DeviceServer.cpp`

- [ ] **Step 1: 修改 DeviceServer.h 添加新信号和方法**

```cpp
class DeviceServer : public QObject
{
    // ... 现有内容 ...

signals:
    void busyChanged();
    void recordsChanged();
    void operationSucceeded(const QString &apiType, const QString &message);
    void operationFailed(const QString &apiType, int code, const QString &message);
    void deviceStatusChanged(const QString &deviceId, const QString &status, const QString &ipAddress);
    void newDevicePendingAuth(const QString &deviceId, const QString &ipAddress);  // 新增

public:
    // ... 现有方法 ...
    
    // 新增：处理新设备待认证推送
    void handleDevicePendingAuthPush(const QString &deviceId, const QString &ipAddress);
    
    // 新增：带密钥的认证方法
    Q_INVOKABLE void approveDeviceWithKey(const QString &deviceId, const QString &deviceKey);
};
```

- [ ] **Step 2: 在 DeviceServer.cpp 中添加新方法实现**

```cpp
void DeviceServer::handleDevicePendingAuthPush(const QString &deviceId, const QString &ipAddress) {
    // 发射信号通知 UI
    emit newDevicePendingAuth(deviceId, ipAddress);
}

void DeviceServer::approveDeviceWithKey(const QString &deviceId, const QString &deviceKey) {
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kDeviceAuthApprove, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    if (deviceId.trimmed().isEmpty()) {
        emit operationFailed(kDeviceAuthApprove, -1, QStringLiteral("请选择待认证设备"));
        return;
    }

    QJsonObject data;
    data[kDeviceId] = deviceId.trimmed();
    
    // 新增：如果提供了密钥，添加到请求中
    if (!deviceKey.isEmpty()) {
        data[QStringLiteral("deviceKey")] = deviceKey;
    }

    QJsonObject msg;
    msg[kType] = kDeviceAuthApprove;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kDeviceAuthApprove, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kDeviceAuthApprove, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kDeviceAuthApprove, code, text);
    });
}
```

- [ ] **Step 3: 验证编译**

Run: `cd E:\project\AttendanceServer && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 4: Commit**

```bash
cd E:\project\AttendanceServer
git add src/Device/DeviceServer.h src/Device/DeviceServer.cpp
git commit -m "feat: add newDevicePendingAuth signal and approveDeviceWithKey method"
```

---

## Task 5: 管理端 - 连接推送信号

**Files:**
- Modify: `E:\project\AttendanceServer\main.cpp`

- [ ] **Step 1: 在 main.cpp 中添加信号连接**

在 `main.cpp` 中现有的信号连接后添加：

```cpp
// 连接设备待认证推送到 DeviceServer
QObject::connect(eventService, &EventSubscriptionService::serverPushReceived,
                 deviceServer, [deviceServer](const QString &messageType, const QVariantMap &data) {
                     if (messageType == QStringLiteral("device.pending_auth.push")) {
                         const QString deviceId = data.value(QStringLiteral("deviceId")).toString();
                         const QString ipAddress = data.value(QStringLiteral("ipAddress")).toString();
                         deviceServer->handleDevicePendingAuthPush(deviceId, ipAddress);
                     }
                 });
```

- [ ] **Step 2: 验证编译**

Run: `cd E:\project\AttendanceServer && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 3: Commit**

```bash
cd E:\project\AttendanceServer
git add main.cpp
git commit -m "feat: connect device.pending_auth.push to DeviceServer"
```

---

## Task 6: 管理端 - 创建认证对话框和提示

**Files:**
- Modify: `E:\project\AttendanceServer\ui\pages\PageDevice.qml`

- [ ] **Step 1: 在 PageDevice.qml 中添加认证对话框**

在 `PageDevice.qml` 的 `ColumnLayout` 后添加：

```qml
Dialog {
    id: authDialog
    title: qsTr("设备认证")
    modal: true
    anchors.centerIn: parent
    width: 400
    
    property string deviceId: ""
    property string ipAddress: ""
    
    ColumnLayout {
        spacing: Theme.spacingMd
        
        Label {
            text: qsTr("设备 ID: ") + authDialog.deviceId
            font.bold: true
        }
        Label {
            text: qsTr("IP 地址: ") + authDialog.ipAddress
        }
        
        Item { height: Theme.spacingSm }
        
        Label {
            text: qsTr("设备密钥:")
        }
        TextField {
            id: deviceKeyField
            placeholderText: qsTr("留空使用默认密钥 (changeme)")
            Layout.fillWidth: true
        }
        Label {
            text: qsTr("提示：设备将使用此密钥进行后续认证")
            color: Theme.textMuted
            font.pixelSize: Theme.fontXs
        }
    }
    
    standardButtons: Dialog.Ok | Dialog.Cancel
    
    onOpened: {
        deviceKeyField.text = ""
        deviceKeyField.forceActiveFocus()
    }
    
    onAccepted: {
        deviceServer.approveDeviceWithKey(deviceId, deviceKeyField.text)
    }
}
```

- [ ] **Step 2: 添加 Toast 提示和信号连接**

在 `PageDevice.qml` 的 `Connections` 中添加：

```qml
Connections {
    target: deviceServer
    function onOperationSucceeded(apiType, message) {
        page.serviceResult(apiType, 0, message)
        if (apiType.indexOf("create") >= 0 || apiType.indexOf("update") >= 0
                || apiType.indexOf("delete") >= 0 || apiType.indexOf("auth.approve") >= 0)
            page._query()
    }
    function onOperationFailed(apiType, code, message) {
        page.serviceResult(apiType, code, message)
    }
    // 新增：处理新设备待认证信号
    function onNewDevicePendingAuth(deviceId, ipAddress) {
        // 显示 Toast 提示
        if (typeof toast !== "undefined") {
            toast.show(qsTr("新设备待认证: %1 (%2)").arg(deviceId).arg(ipAddress))
        }
        // 打开认证对话框
        authDialog.deviceId = deviceId
        authDialog.ipAddress = ipAddress
        authDialog.open()
    }
}
```

- [ ] **Step 3: 验证编译**

Run: `cd E:\project\AttendanceServer && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 4: Commit**

```bash
cd E:\project\AttendanceServer
git add ui/pages/PageDevice.qml
git commit -m "feat: add device auth dialog with key input"
```

---

## Task 7: 设备端 - 新增 devicePendingAuth 信号

**Files:**
- Modify: `D:\QTproject\AttendanceSystem\NetworkClient\networkclient.h`
- Modify: `D:\QTproject\AttendanceSystem\NetworkClient\networkclient.cpp`

- [ ] **Step 1: 在 networkclient.h 中添加新信号**

```cpp
class Networkclient : public QObject
{
    // ... 现有内容 ...

signals:
    // ... 现有信号 ...
    void devicePendingAuth();  // 新增：设备待审核信号
};
```

- [ ] **Step 2: 在 networkclient.cpp 中修改 handleAuthResponse**

在 `handleAuthResponse` 方法中添加待审核检测：

```cpp
void Networkclient::handleAuthResponse(const QJsonObject &msg) {
    const int code = msg.value("code").toInt(-1);
    const QJsonObject data = msg.value("data").toObject();
    
    if (code == 0) {
        // 认证成功
        // ... 现有的 token 存储逻辑 ...
        
        // 新增：检查是否收到新密钥
        const QString newDeviceKey = data.value("deviceKey").toString();
        if (!newDeviceKey.isEmpty()) {
            // 更新本地密钥
            m_deviceKey = newDeviceKey;
            ConfigManager::instance()->setDeviceKey(newDeviceKey);
            ConfigManager::instance()->saveConfig();
            
            qDebug() << "Device key updated from server:" << newDeviceKey;
        }
        
        emit authenticated();
    } else if (code == 2002) {
        // 认证失败
        const QString msgText = data.value("msg").toString();
        if (msgText.contains("pending authorization")) {
            // 新增：设备待审核提示
            qDebug() << "Device pending authorization, waiting for admin approval";
            emit devicePendingAuth();
        } else {
            emit authFailed(msgText);
        }
    } else {
        // 其他错误
        emit authFailed(data.value("msg").toString());
    }
}
```

- [ ] **Step 3: 验证编译**

Run: `cd D:\QTproject\AttendanceSystem && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 4: Commit**

```bash
cd D:\QTproject\AttendanceSystem
git add NetworkClient/networkclient.h NetworkClient/networkclient.cpp
git commit -m "feat: add devicePendingAuth signal and handle pending authorization"
```

---

## Task 8: 设备端 - 处理待审核信号

**Files:**
- Modify: `D:\QTproject\AttendanceSystem\mainwindow.cpp`

- [ ] **Step 1: 在 mainwindow.cpp 中添加信号连接**

在 `initNetworkClient` 方法中添加：

```cpp
void MainWindow::initNetworkClient() {
    // ... 现有连接 ...
    
    // 新增：处理设备待审核信号
    connect(m_networkClient, &Networkclient::devicePendingAuth,
            this, [this]() {
                // 显示待审核提示
                QMessageBox::information(this, tr("设备待审核"),
                    tr("设备正在等待管理员审核，请稍后重试。\n\n"
                       "请联系管理员在管理端进行设备认证。"));
                
                // 设置定时重试（30秒后）
                QTimer::singleShot(30000, this, [this]() {
                    if (m_networkClient && !m_networkClient->isAuthenticated()) {
                        m_networkClient->reconnect();
                    }
                });
            });
}
```

- [ ] **Step 2: 验证编译**

Run: `cd D:\QTproject\AttendanceSystem && cmake --build build`
Expected: 编译成功，无错误

- [ ] **Step 3: Commit**

```bash
cd D:\QTproject\AttendanceSystem
git add mainwindow.cpp
git commit -m "feat: handle devicePendingAuth signal with retry mechanism"
```

---

## Task 9: 测试 - 端到端测试

- [ ] **Step 1: 启动服务端**

```bash
cd D:\attendanceServer
./build/attendance_server
```

- [ ] **Step 2: 启动管理端**

```bash
cd E:\project\AttendanceServer
./build/AttendanceAdmin
```

- [ ] **Step 3: 启动设备端**

```bash
cd D:\QTproject\AttendanceSystem
./build/AttendanceSystem
```

- [ ] **Step 4: 测试新设备连接流程**

1. 使用一个新的 deviceId 连接服务端
2. 验证管理端是否收到待认证通知
3. 验证设备端是否显示"等待审核"提示

- [ ] **Step 5: 测试管理员认证流程**

1. 在管理端点击"认证"按钮
2. 输入新密钥
3. 验证设备端是否收到认证成功响应
4. 验证设备端是否更新了本地密钥

- [ ] **Step 6: 测试密钥验证**

1. 断开设备端连接
2. 使用新密钥重新连接
3. 验证是否认证成功

- [ ] **Step 7: 测试默认密钥**

1. 使用另一个新 deviceId 连接
2. 在管理端认证时不输入密钥
3. 验证是否使用默认密钥（changeme）

---

## Task 10: 文档更新

- [ ] **Step 1: 更新设备端通信协议文档**

在 `设备端通信协议指导文档.md` 中添加：
- device.pending_auth.push 消息说明
- device.auth.approve 扩展字段说明
- auth.response 扩展字段说明

- [ ] **Step 2: 更新管理端开发指导文档**

在 `管理端开发指导文档.md` 中添加：
- 新设备认证流程说明
- DeviceServer 新增方法说明

- [ ] **Step 3: Commit**

```bash
git add 设备端通信协议指导文档.md 管理端开发指导文档.md
git commit -m "docs: update protocol and admin docs for device auth improvement"
```

---

## 自检清单

- [ ] 所有任务已完成
- [ ] 所有代码已编译通过
- [ ] 端到端测试通过
- [ ] 文档已更新
- [ ] 所有 commit 已提交
