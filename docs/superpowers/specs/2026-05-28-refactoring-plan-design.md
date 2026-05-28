# AttendanceSystem 重构计划设计文档

## 文档信息

- **日期：** 2026-05-28
- **版本：** 1.0
- **状态：** 待审批
- **作者：** Claude Code

---

## 1. 概述

### 1.1 背景

AttendanceSystem 是一个基于 Qt6 的人脸识别考勤设备端应用。经过代码分析，发现以下可维护性问题：

- **NetworkClient 模块职责过重**（2427行）：连接管理、认证、消息收发、业务接口、Outbox 处理、JWT 令牌管理全在一个类
- **FaceRecognition 模块命名问题**：拼音方法名、拼写错误
- **类名命名不一致**：`Networkclient` vs `Heartbeatmanager` vs `Connectionmanager`
- **LocalStorage 模块重复代码**：Repository 类中大量重复的数据库操作模式
- **ConfigManager 职责过重**：配置读写、路径计算、目录创建

### 1.2 重构目标

- **主要目标：** 提高代码可维护性
- **范围：** 全部模块
- **策略：** 分阶段重构
- **兼容性要求：** 完全兼容，不破坏现有功能

### 1.3 重构方案选择

经过评估，选择**方案2：中等方案（适度重构）**：

- 修复命名问题和拼写错误
- 适度拆分职责，平衡风险和收益
- 保持单例模式但改进实现
- 不引入依赖注入（保持完全兼容）

---

## 2. 阶段1：命名修正

### 2.1 方法名修正

| 文件 | 当前名称 | 修正后 | 说明 |
|------|----------|--------|------|
| `facerecognizer.h/cpp` | `WanZhengYeWuLiuCheng` | `processFrame` | 拼音→英文 |
| `facerecognizer.h/cpp` | `perfromRecognition` | `performRecognition` | 拼写错误 |
| `localstorage.h/cpp` | `connectDatabse` | `connectDatabase` | 拼写错误 |

### 2.2 类名统一（PascalCase）

| 当前名称 | 修正后 | 影响文件 |
|----------|--------|----------|
| `Networkclient` | `NetworkClient` | networkclient.h/cpp, 所有引用处 |
| `Heartbeatmanager` | `HeartbeatManager` | heartbeatmanager.h/cpp, 所有引用处 |
| `Connectionmanager` | `ConnectionManager` | connectionmanager.h/cpp, 所有引用处 |
| `Messagewriter` | `MessageWriter` | messagewriter.h/cpp, 所有引用处 |
| `Messagereader` | `MessageReader` | messagereader.h/cpp, 所有引用处 |
| `Messagequeue` | `MessageQueue` | messagequeue.h/cpp, 所有引用处 |

### 2.3 信号名修正

| 文件 | 当前名称 | 修正后 |
|------|----------|--------|
| `heartbeatmanager.h` | `heartbeattimeout` | `heartbeatTimeout` |
| `heartbeatmanager.h` | `sendHeartbeat` | `sendHeartbeatData`（更清晰） |

### 2.4 影响范围

- 需要修改所有引用这些类名/方法名的文件
- 使用全局替换确保一致性
- 需要更新 CMakeLists.txt 中的文件名（如果有）

### 2.5 风险评估

- **风险等级：** 低
- **验证方式：** 编译通过 + 基本功能测试

---

## 3. 阶段2：NetworkClient 模块重构

### 3.1 职责拆分

将 `NetworkClient` 类拆分为以下职责：

```
当前 NetworkClient (2427行)
    ↓
┌─────────────────────────────────────────────────────────┐
│  NetworkClient (核心)                                    │
│  - 连接管理（委托给 ConnectionManager）                   │
│  - 消息收发（委托给 MessageReader/Writer）                │
│  - 业务接口（仅保留方法签名，实现委托给专门类）            │
└─────────────────────────────────────────────────────────┘
    ↓
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│ Authenticator   │  │ OutboxManager   │  │ AttendanceClient│
│ - 认证流程      │  │ - 出箱队列管理  │  │ - 考勤上报接口  │
│ - 令牌刷新      │  │ - 重试逻辑      │  │ - 批量上报      │
│ - 会话管理      │  │ - 状态跟踪      │  │ - 响应处理      │
└─────────────────┘  └─────────────────┘  └─────────────────┘
```

### 3.2 Authenticator 类设计

```cpp
// NetworkClient/authenticator.h
class Authenticator : public QObject {
    Q_OBJECT
public:
    explicit Authenticator(QObject *parent = nullptr);
    
    void setDeviceCredentials(const QString &deviceId, const QString &deviceKey);
    void authenticate(QTcpSocket *socket);
    void refreshToken();
    bool isAuthenticated() const;
    QString sessionToken() const;
    
signals:
    void authSuccess(const QString &sessionToken);
    void authFailed(int code, const QString &message);
    void tokenRefreshed(const QString &newToken);
    
public slots:
    void handleMessage(const QJsonObject &message);
    
private:
    void handleAuthResponse(const QJsonObject &message);
    void handleTokenRefreshResponse(const QJsonObject &message);
    
    QString m_deviceId;
    QString m_deviceKey;
    QString m_sessionToken;
    bool m_isAuthenticated = false;
    TokenManager *m_tokenManager;
    TokenRefresher *m_tokenRefresher;
};
```

### 3.3 OutboxManager 类设计

```cpp
// NetworkClient/outboxmanager.h
class OutboxManager : public QObject {
    Q_OBJECT
public:
    explicit OutboxManager(QObject *parent = nullptr);
    
    QString enqueue(const QString &employeeId, const QString &status,
                    const QDateTime &checkTime);
    QString enqueueWithPhoto(const QString &employeeId, const QString &status,
                             const QByteArray &photoJpeg, const QDateTime &checkTime);
    void retryAll();
    void processOutbox();
    int pendingCount() const;
    
signals:
    void recordEnqueued(const QString &msgId);
    void recordSent(const QString &msgId);
    void recordConfirmed(const QString &msgId);
    void recordFailed(const QString &msgId, const QString &error);
    void pendingCountChanged(int count);
    
public slots:
    void handleResponse(const QJsonObject &message);
    
private:
    QTimer *m_retryTimer;
    int m_retryRound = 0;
    int m_maxRetryCount = 5;
    int m_retryBackoffBaseMs = 2000;
};
```

### 3.4 NetworkClient 简化后的接口

```cpp
// NetworkClient/networkclient.h
class NetworkClient : public QObject {
    Q_OBJECT
public:
    static NetworkClient* instance();
    
    // 连接管理（保持不变）
    bool connectToServer(const QString &ip, quint16 port);
    void disconnect();
    bool isConnected() const;
    
    // 认证（委托给 Authenticator）
    bool isAuthenticated() const;
    Authenticator* authenticator() const;
    
    // 考勤上报（委托给 OutboxManager）
    QString uploadAttendance(const QString &employeeId, const QString &status,
                             const QDateTime &checkTime);
    OutboxManager* outboxManager() const;
    
    // 同步接口（保持不变）
    bool syncPersonData();
    bool sendJson(const QJsonObject &message);
    
    // 设备身份（保持不变）
    void setDeviceId(const QString &deviceId);
    QString deviceId() const;
    void setDeviceKey(const QString &deviceKey);
    QString deviceKey() const;
    
signals:
    // 连接状态
    void connected();
    void disconnected();
    void networkStateChanged(bool isOnline);
    
    // 认证（转发 Authenticator 信号）
    void authSuccess();
    void authFailed(int code, const QString &message);
    void devicePendingAuth();
    
    // 业务数据
    void personDataReceived(const QVector<ServerProtocol::PersonData> &persons);
    void uploadFinished(bool success, const QString &message);
    void faceSyncItemReceived(const QJsonObject &header, const QByteArray &payload);
    void attendanceReportResult(const QString &employeeId, bool success, const QString &message);
    void deviceCommandReceived(const QJsonObject &message);
    
    // 同步流信号（路由到 SyncManager）
    void personSyncReceived(const QJsonObject &message);
    void faceSyncBeginReceived(const QJsonObject &message);
    void faceSyncEndReceived(const QJsonObject &message);
    
    // JWT 令牌刷新响应
    void tokenRefreshResponse(const QJsonObject &message);
    
public slots:
    void scheduleReconnect();
    
private slots:
    void onConnectionConnected();
    void onConnectionDisconnected();
    void onConnectionStateChanged(bool isOnline);
    void onMessageReceived(const QJsonObject &message);
    void onBinaryFrameReceived(const QJsonObject &header, const QByteArray &payload);
    void onHeartbeatTimeout();
    void onSendError();
    void onSendHeartbeat(const QByteArray &data);
    
private:
    explicit NetworkClient(QObject *parent = nullptr);
    void setupConnections();
    void processQueue();
    void loadDeviceConfig();
    
    ConnectionManager *m_connection;
    HeartbeatManager *m_heartbeat;
    MessageWriter *m_writer;
    MessageReader *m_reader;
    MessageQueue *m_queue;
    Authenticator *m_authenticator;
    OutboxManager *m_outboxManager;
    
    QString m_deviceId = QStringLiteral("device_001");
    QString m_deviceKey;
    QString m_deviceName;
    QString m_fwVersion = QStringLiteral("1.0.0");
    
    bool m_isOnline = false;
};
```

### 3.5 消息路由简化

```cpp
void NetworkClient::onMessageReceived(const QJsonObject &message) {
    const QString type = message["type"].toString();
    
    // 路由到专门的处理器
    if (type.startsWith("device.auth")) {
        m_authenticator->handleMessage(message);
    } else if (type.startsWith("attendance.")) {
        m_outboxManager->handleResponse(message);
    } else if (type.startsWith("person.") || type.startsWith("face.")) {
        emit syncMessageReceived(message);  // 路由到 SyncManager
    } else if (type == "device.command") {
        emit deviceCommandReceived(message);  // 路由到 CommandHandler
    } else if (type == "device.pending_auth") {
        emit devicePendingAuth();
    }
}
```

### 3.6 实施步骤

1. 创建 `Authenticator` 类
2. 创建 `OutboxManager` 类
3. 将认证逻辑从 `NetworkClient` 移动到 `Authenticator`
4. 将 Outbox 逻辑从 `NetworkClient` 移动到 `OutboxManager`
5. 简化 `NetworkClient` 的消息路由
6. 更新所有调用点

### 3.7 风险评估

- **风险等级：** 中
- **验证方式：** 编译通过 + 认证流程测试 + 打卡流程测试

---

## 4. 阶段3：FaceRecognition 模块改进

### 4.1 方法重命名

| 当前 | 修正后 | 说明 |
|------|--------|------|
| `WanZhengYeWuLiuCheng` | `processFrame` | 清晰表达功能 |
| `perfromRecognition` | `performRecognition` | 修正拼写 |

### 4.2 实现空方法

当前 `handleDetectingState()` 是空实现，需要补充逻辑：

```cpp
void FaceRecognizer::handleDetectingState()
{
    // 检测超时：如果在检测状态停留太久，回到 IDLE
    setState(RecognitionState::IDLE);
}
```

### 4.3 解耦 NetworkClient 依赖

当前 `facerecognizer.h` 直接 include 了 `networkclient.h`，但实际上 `FaceRecognizer` 并不直接使用 `NetworkClient`。

**依赖分析：**
- `FaceRecognizer` 依赖：`arcfaceengine`, `FaceDatabaseManager`, `ConfigManager`, `AttendanceRuleEngine`
- 不直接依赖：`NetworkClient`, `LocalStorage`

**改进：** 移除不必要的 include，降低编译依赖。

```cpp
// 移除这些 include
// #include "../LocalStorage/localstorage.h"
// #include "../NetworkClient/networkclient.h"
```

### 4.4 实施步骤

1. 重命名 `WanZhengYeWuLiuCheng` → `processFrame`
2. 重命名 `perfromRecognition` → `performRecognition`
3. 实现 `handleDetectingState()` 方法
4. 移除不必要的 include
5. 更新所有调用点

### 4.5 风险评估

- **风险等级：** 低
- **验证方式：** 编译通过 + 人脸识别功能测试

---

## 5. 阶段4：LocalStorage 模块改进

### 5.1 问题分析

当前 Repository 类存在大量重复代码：

```cpp
// 每个方法都有相同的模式
QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
QSqlQuery query(db);
query.prepare("...");
query.bindValue("...", value);
if (!query.exec()) {
    return false;
}
return true;
```

### 5.2 解决方案：抽取基类

```cpp
// LocalStorage/BaseRepository.h
class BaseRepository {
public:
    explicit BaseRepository(const QString &dbPath);
    virtual ~BaseRepository() = default;
    
protected:
    // 通用查询执行
    bool executeNonQuery(const QString &sql, 
                        const QVariantMap &params = {});
    
    QSqlQuery executeQuery(const QString &sql, 
                          const QVariantMap &params = {});
    
    // 事务管理
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    QString m_dbPath;
};
```

### 5.3 代码简化示例

**重构前（FaceFeatureRepository.cpp）：**
```cpp
bool FaceFeatureRepository::insertOrReplace(...) {
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO face_feature ...");
    query.bindValue(":eid", employeeId);
    query.bindValue(":blob", featureBlob);
    query.bindValue(":size", featureSize);
    query.bindValue(":gen", syncGeneration);
    if (!query.exec()) {
        return false;
    }
    return true;
}
```

**重构后：**
```cpp
bool FaceFeatureRepository::insertOrReplace(...) {
    return executeNonQuery(
        "INSERT OR REPLACE INTO face_feature "
        "(employee_id, feature_blob, feature_size, updated_at, sync_generation) "
        "VALUES (:eid, :blob, :size, datetime('now'), :gen)",
        {{":eid", employeeId}, {":blob", featureBlob}, 
         {":size", featureSize}, {":gen", syncGeneration}}
    );
}
```

### 5.4 影响范围

需要重构的 Repository 类：
- `FaceFeatureRepository` (6个方法)
- `AttendanceOutboxRepository` (10个方法)
- `SyncMetaRepository` (4个方法)
- `DeviceLocalRepository` (3个方法)

### 5.5 实施步骤

1. 创建 `BaseRepository` 基类
2. 重构 `FaceFeatureRepository` 继承基类
3. 重构 `AttendanceOutboxRepository` 继承基类
4. 重构 `SyncMetaRepository` 继承基类
5. 重构 `DeviceLocalRepository` 继承基类

### 5.6 风险评估

- **风险等级：** 中
- **验证方式：** 编译通过 + 数据库操作测试

---

## 6. 阶段5：Attendance 模块清理

### 6.1 未使用信号清理

当前 `AttendanceReporter` 定义了两个未使用的信号：

```cpp
// AttendanceReporter.h 中定义但未使用
signals:
    void sendMessage(const QJsonObject &message);
    void sendRawBytes(const QByteArray &data);
```

**处理：** 移除这两个信号，因为 `AttendanceReporter` 直接调用 `NetworkClient::instance()` 而非通过信号。

### 6.2 实施步骤

1. 移除 `sendMessage` 信号声明
2. 移除 `sendRawBytes` 信号声明
3. 确认没有其他代码连接这些信号

### 6.3 风险评估

- **风险等级：** 低
- **验证方式：** 编译通过

---

## 7. 阶段6：Config 模块改进

### 7.1 拆分 `restoreDefaults()` 方法

```cpp
// 当前：一个巨大的方法
void ConfigManager::restoreDefaults() {
    // 50+ 行代码...
}

// 重构后：拆分为多个小方法
void ConfigManager::restoreDefaults() {
    restoreNetworkDefaults();
    restoreFaceRecognitionDefaults();
    restoreAttendanceDefaults();
    restoreDeviceDefaults();
    restoreSyncDefaults();
    restoreRetryDefaults();
}

void ConfigManager::restoreNetworkDefaults() {
    m_serverIP = DEFAULT_SERVER_IP;
    m_serverPort = DEFAULT_SERVER_PORT;
    m_connectionTimeout = DEFAULT_CONNECTION_TIMEOUT;
}

void ConfigManager::restoreFaceRecognitionDefaults() {
    m_faceThreshold = DEFAULT_FACE_THRESHOLD;
    m_maxFaceCount = DEFAULT_MAX_FACE_COUNT;
    m_recognizeTimeout = DEFAULT_RECOGNIZE_TIMEOUT;
    m_cameraRotation = DEFAULT_CAMERA_ROTATION;
}

void ConfigManager::restoreAttendanceDefaults() {
    m_workStartTime = QTime(9, 0);
    m_workEndTime = QTime(18, 0);
    m_checkInStartOffset = DEFAULT_CHECK_IN_START_OFFSET;
    m_lateAllowance = DEFAULT_LATE_ALLOWANCE;
    m_earlyLeaveAllowance = DEFAULT_EARLY_LEAVE_ALLOWANCE;
    m_checkOutEndOffset = DEFAULT_CHECK_OUT_END_OFFSET;
    m_flexibleWorkEnabled = DEFAULT_FLEXIBLE_WORK_ENABLED;
    m_flexibleRange = DEFAULT_FLEXIBLE_RANGE;
    m_coreWorkStart = QTime(10, 0);
    m_coreWorkEnd = QTime(16, 0);
    m_lunchBreakEnabled = DEFAULT_LUNCH_BREAK_ENABLED;
    m_lunchBreakStart = QTime(12, 0);
    m_lunchBreakEnd = QTime(13, 0);
    m_minCheckInterval = DEFAULT_MIN_CHECK_INTERVAL;
    m_allowCrossDay = DEFAULT_ALLOW_CROSS_DAY;
    m_maxWorkHours = DEFAULT_MAX_WORK_HOURS;
}

void ConfigManager::restoreDeviceDefaults() {
    if (m_deviceId.isEmpty()) m_deviceId = QStringLiteral("device_001");
    m_deviceName = QString();
    m_fwVersion = QStringLiteral("1.0.0");
    m_configVersion = QString();
    m_configHash = QString();
}

void ConfigManager::restoreSyncDefaults() {
    m_autoSyncOnConnect = DEFAULT_AUTO_SYNC_ON_CONNECT;
    m_syncTimeout = DEFAULT_SYNC_TIMEOUT;
}

void ConfigManager::restoreRetryDefaults() {
    m_maxRetryCount = DEFAULT_MAX_RETRY_COUNT;
    m_retryBackoffBaseMs = DEFAULT_RETRY_BACKOFF_BASE_MS;
}
```

### 7.2 使用智能指针

```cpp
// 当前
class ConfigManager : public QObject {
private:
    QSettings* m_settings;
    QSettings* m_localSettings;
};

// 重构后
class ConfigManager : public QObject {
private:
    std::unique_ptr<QSettings> m_settings;
    std::unique_ptr<QSettings> m_localSettings;
};
```

需要更新构造函数和析构函数：

```cpp
ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    restoreDefaults();
    
    QString configPath = getConfigFilePath();
    QString localConfigPath = getLocalConfigFilePath();
    QDir dir;
    QString configDir = QFileInfo(configPath).path();
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }
    
    m_settings = std::make_unique<QSettings>(configPath, QSettings::IniFormat, this);
    m_localSettings = std::make_unique<QSettings>(localConfigPath, QSettings::IniFormat, this);
    
    loadConfig();
}

// 移除析构函数中的手动 delete
// ConfigManager::~ConfigManager() = default;
```

### 7.3 实施步骤

1. 拆分 `restoreDefaults()` 为多个小方法
2. 将 `QSettings*` 改为 `std::unique_ptr<QSettings>`
3. 更新构造函数使用 `std::make_unique`
4. 移除析构函数中的手动 delete

### 7.4 风险评估

- **风险等级：** 低
- **验证方式：** 编译通过 + 配置读写测试

---

## 8. 实施顺序

### 8.1 推荐顺序

1. **阶段1：命名修正**（低风险，快速完成）
2. **阶段3：FaceRecognition 模块改进**（低风险，独立模块）
3. **阶段5：Attendance 模块清理**（低风险，简单改动）
4. **阶段6：Config 模块改进**（低风险，内部重构）
5. **阶段4：LocalStorage 模块改进**（中风险，需要仔细测试）
6. **阶段2：NetworkClient 模块重构**（中风险，最大改动）

### 8.2 里程碑

| 里程碑 | 阶段 | 预计时间 | 验证标准 |
|--------|------|----------|----------|
| M1 | 阶段1 + 阶段3 + 阶段5 | 1-2天 | 编译通过，基本功能正常 |
| M2 | 阶段6 | 1天 | 编译通过，配置读写正常 |
| M3 | 阶段4 | 2-3天 | 编译通过，数据库操作正常 |
| M4 | 阶段2 | 3-5天 | 编译通过，认证和打卡流程正常 |

---

## 9. 测试策略

### 9.1 单元测试

由于项目当前没有单元测试框架，建议：

1. 添加 Google Test 或 Catch2 框架
2. 为重构的类编写单元测试
3. 重点测试：
   - `Authenticator` 的认证流程
   - `OutboxManager` 的重试逻辑
   - `BaseRepository` 的数据库操作
   - `FaceRecognizer` 的状态机

### 9.2 集成测试

1. **认证流程测试：** 设备认证 → 心跳 → 令牌刷新
2. **打卡流程测试：** 人脸检测 → 特征提取 → 规则判定 → 上报
3. **数据同步测试：** 人员同步 → 人脸特征同步
4. **离线测试：** 断网 → 打卡 → 恢复网络 → 重试上报

### 9.3 回归测试

每个阶段完成后，执行完整的回归测试：

1. 启动应用，检查界面显示
2. 测试人脸识别功能
3. 测试打卡流程
4. 测试网络连接和断线重连
5. 测试配置读写

---

## 10. 风险缓解

### 10.1 风险识别

| 风险 | 影响 | 概率 | 缓解措施 |
|------|------|------|----------|
| 编译错误 | 高 | 中 | 分阶段重构，每阶段后编译验证 |
| 功能回归 | 高 | 低 | 完整的回归测试 |
| 性能下降 | 中 | 低 | 性能测试，重点关注数据库操作 |
| 合并冲突 | 中 | 中 | 频繁提交，小步快跑 |

### 10.2 回滚策略

每个阶段独立，如果出现问题：

1. 使用 `git revert` 回滚到上一个稳定版本
2. 分析问题原因
3. 修复后重新实施

---

## 11. 文档更新

### 11.1 需要更新的文档

- `CLAUDE.md` - 更新架构说明
- `项目说明文档.md` - 更新模块说明
- 代码注释 - 更新类和方法的注释

### 11.2 代码规范

重构后的代码应遵循：

- 类名：PascalCase（如 `NetworkClient`）
- 方法名：camelCase（如 `processFrame`）
- 信号名：camelCase（如 `heartbeatTimeout`）
- 成员变量：m_ 前缀（如 `m_deviceId`）
- 常量：k 前缀或全大写（如 `kGraceMultiplier`）

---

## 12. 总结

本重构计划采用分阶段策略，从低风险的命名修正开始，逐步推进到更复杂的架构重构。每个阶段都有明确的目标、实施步骤和验证标准，确保重构过程可控、可验证。

**关键原则：**

1. **完全兼容：** 不破坏现有功能
2. **分阶段实施：** 小步快跑，降低风险
3. **持续验证：** 每阶段后编译和测试
4. **文档同步：** 及时更新文档

**预期收益：**

- 代码可维护性显著提升
- 模块职责更清晰
- 命名规范统一
- 重复代码减少
- 为后续功能扩展打下良好基础
