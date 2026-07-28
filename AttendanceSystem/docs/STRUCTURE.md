# AttendanceSystem 项目结构文档

## 1. 顶层目录结构

```
AttendanceSystem/
├── Attendance/                 # 考勤业务模块
├── Auth/                       # 认证模块（JWT 令牌管理）
├── CameraCapture/              # 摄像头采集模块
├── Command/                    # 远程指令处理模块
├── Config/                     # 配置管理模块
├── FaceRecognition/            # 人脸识别模块
├── LocalStorage/               # 本地数据存储模块
├── NetworkClient/              # 网络通信模块
├── Sync/                       # 数据同步模块
├── UI/                         # 用户界面模块
│   ├── Components/             # 可复用 UI 组件
│   └── Theme/                  # 主题与设计令牌
├── Utils/                      # 工具类
├── third_party/                # 第三方库
│   └── arcface/                # ArcFace SDK
├── docs/                       # 项目文档
├── main.cpp                    # 应用入口
├── mainwindow.cpp / .h / .ui   # 主窗口
├── CMakeLists.txt              # 构建配置
└── resources/                  # 资源文件（QSS、图标等）
```

---

## 2. 模块详细结构

### 2.1 Attendance/ — 考勤业务

```
Attendance/
├── AttendanceRuleEngine.h / .cpp         # 考勤规则引擎（单例）
├── AttendanceReporter.h / .cpp           # 考勤上报协调器
├── AttendanceConfigParser.h / .cpp       # 考勤配置解析器
├── AttendanceConfigSyncHandler.h / .cpp  # 远程考勤配置同步处理
└── AttendanceConfigValidator.h / .cpp    # 考勤配置校验器
```

**AttendanceRuleEngine** — 考勤规则判定引擎
- 单例模式
- `evaluate(checkTime)` → 根据当前时间判定考勤状态
- 支持标准工时（`evaluateStandard`）和弹性工时（`evaluateFlexible`）
- 判定项目：迟到、早退、正常、缺勤
- 防重复打卡检查（`isDuplicateCheck`）

**AttendanceReporter** — 考勤上报协调器
- 封装 Outbox 持久化 → 发送 → 响应 → 清理完整流程
- `report()` / `reportWithPhoto()` → 写入 Outbox 并尝试发送
- `retryAll()` → 重试所有待发送记录
- 监听网络状态变化，离线时缓存、上线后自动重试

**AttendanceConfigParser** — 考勤配置解析
- 解析服务器下发的考勤规则配置

**AttendanceConfigSyncHandler** — 远程考勤配置同步
- 处理 `device.command` 中的配置更新指令
- 调用 ConfigManager 应用新配置

**AttendanceConfigValidator** — 考勤配置校验
- 校验配置参数的合法性

---

### 2.2 Auth/ — 认证模块

```
Auth/
├── tokenmanager.h / .cpp     # JWT 令牌管理器
└── tokenrefresher.h / .cpp   # JWT 令牌自动刷新器
```

**TokenManager** — 令牌管理
- 存储和查询当前会话令牌（JWT Access Token）
- 存储和查询 Refresh Token
- 令牌过期检测

**TokenRefresher** — 令牌刷新
- 监听令牌过期事件
- 自动发送 `token.refresh` 请求
- 更新 TokenManager 中的令牌

---

### 2.3 CameraCapture/ — 摄像头采集

```
CameraCapture/
├── cameracapture.h / .cpp          # 摄像头生命周期管理
├── videoframecapture.h / .cpp      # 视频帧捕获器
└── videoframeconverter.h / .cpp    # 帧格式转换器
```

**CameraCapture** — 摄像头管理
- 封装 `QCamera`，提供初始化/启动/停止接口
- 管理可用摄像头设备列表

**VideoFrameCapture** — 帧捕获
- 从摄像头捕获视频帧
- 输出 `QImage` 格式
- 信号 `frameCaptured(QImage)` 连接到 FaceRecognizer（未旋转，用于识别）
- 信号 `frameForDisplay(QImage)` 连接到 FaceVideoWidget（已旋转，用于显示）

**VideoFrameConverter** — 帧转换
- YUV → RGB 等格式转换
- 图像旋转（根据配置的 cameraRotation）

---

### 2.4 Command/ — 远程指令处理

```
Command/
├── CommandHandler.h / .cpp   # 远程指令处理器
```

**CommandHandler** — 指令处理
- 接收 `device.command` 消息
- 解析指令类型并执行
- 支持的指令：
  - `resync` → 触发全量同步
  - `reboot` → 触发设备重启
  - `config.update` → 应用远程配置
- 执行完成后发送 `device.command.ack` 响应

---

### 2.5 Config/ — 配置管理

```
Config/
├── configmanager.h / .cpp    # 配置管理器（单例）
```

**ConfigManager** — 配置管理
- 单例模式
- 双文件策略：
  - `config.ini`：可远程覆盖的配置
  - `local.ini`：仅本机配置
- 配置分类：
  - 网络连接（serverIP, serverPort, connectionTimeout）
  - 人脸识别（faceThreshold, maxFaceCount, recognizeTimeout, cameraRotation）
  - ArcFace SDK（appId, sdkKey）
  - 考勤规则（workStartTime, workEndTime, lateAllowance, flexibleWork 等）
  - 设备信息（deviceId, deviceKey, fwVersion, deviceName）
  - 同步设置（autoSyncOnConnect, syncTimeout）
  - 重试设置（maxRetryCount, retryBackoffBaseMs）
  - 存储路径（databasePath, logPath）
  - 主窗口尺寸（mainWindowWidth, mainWindowHeight）
- `applyRemoteConfig()` → 应用服务器下发的配置

---

### 2.6 FaceRecognition/ — 人脸识别

```
FaceRecognition/
├── arcfaceengine.h / .cpp            # ArcFace SDK 封装
├── facedatabasemanager.h / .cpp      # 人脸特征内存数据库
├── facefeatureextractor.h / .cpp     # 特征提取封装
└── facerecognizer.h / .cpp           # 识别状态机
```

**ArcFaceEngine** — ArcFace 引擎封装（单例）
- SDK 初始化（`initialize`，激活、引擎创建）
- 人脸检测（`detectFace`）→ 返回 `QVector<FaceInfo>`（位置、角度、追踪 ID）
- 特征提取（`extractFeature`）→ 返回 `FaceFeature`（二进制特征向量）
- 特征比对（`compareFeatures`）→ 返回相似度分数（0.0-1.0）

**FaceDatabaseManager** — 人脸数据库（单例）
- `loadFromDatabase()` 从 SQLite 加载所有人员特征到内存
- `findBestMatch()` 1:N 比对，返回 `QPair<QString, float>`（employeeId, 分数）
- `reload()` 支持特征重载（同步完成后通过 `requestFaceDbReload` 信号触发）
- `clear()` 清空内存数据

**FaceFeatureExtractor** — 特征提取工具类
- 单方法封装：`FaceExtraction(QImage)` → `FaceFeature`

**FaceRecognizer** — 识别状态机
- 状态：`IDLE → DETECTING → RECOGNIZED → LOST → IDLE`
- 冷却机制：识别成功后 3 秒内不重复识别（`COOLDOWN_MS = 3000`）
- 丢失检测：3 秒未检测到人脸则回到 IDLE（`LOST_TIMEOUT_MS = 3000`）
- `m_detectingStartTime` 替代 static 局部变量，确保线程安全
- 信号：`recognitionSuccess`、`recognitionFailed`、`requestSaveAttendance`、`faceDetected`、`faceProcessingCompleted`
- 运行在独立的 `m_faceThread` 线程

---

### 2.7 LocalStorage/ — 本地存储

```
LocalStorage/
├── localstorage.h / .cpp                     # 数据库总控（单例）
├── BaseRepository.h                           # Repository 基类
├── FaceFeatureRepository.h / .cpp            # 人脸特征存储
├── AttendanceOutboxRepository.h / .cpp       # 打卡记录出箱
├── SyncMetaRepository.h / .cpp               # 同步元数据
└── DeviceLocalRepository.h / .cpp            # 设备本地信息
```

**LocalStorage** — 数据库总控
- 单例模式
- 管理 SQLite 连接
- 执行数据库迁移（`runMigrations`）
- 提供 Repository 访问器：
  - `faceFeatures()` → FaceFeatureRepository
  - `outbox()` → AttendanceOutboxRepository
  - `syncMeta()` → SyncMetaRepository
  - `deviceLocal()` → DeviceLocalRepository
- 兼容旧版 API（`syncPersons`、`addAttendanceRecord` 等委托给 Repository）

**BaseRepository** — 基类（header-only）
- 提供公共数据库路径（`m_dbPath`）
- 内联实现三个数据库操作方法：`executeNonQuery`（INSERT/UPDATE/DELETE）、`executeScalar`（SELECT 单值）、`executeReader`（SELECT 多行，回调模式）
- 通过 `DatabaseManager::getDatabase` 获取线程安全的数据库连接

**FaceFeatureRepository** — 人脸特征存储
- 存储人员 employeeId、特征二进制数据、特征大小
- `insertOrReplace` / `insertOrReplaceBatch` → 单条/批量写入
- `loadByGeneration(generation)` → 按同步代次加载
- `deleteStale(currentGeneration)` → 清理旧代次数据
- `clearAll()` → 清空全部

**AttendanceOutboxRepository** — 打卡出箱
- 存储待上报的打卡记录
- 状态：`pending` → `sending` → `failed` → `dead`
- 支持按状态查询（`fetchPending`）、状态标记（`markState`）、重试计数（`incrementRetry`）
- 支持按 `clientMsgId` 或 `employeeId` 查询

**SyncMetaRepository** — 同步元数据
- 存储同步版本号（currentGeneration / stagingGeneration）、最后同步时间等
- 支持 staging 机制：`beginStaging` → `commitGeneration`

**DeviceLocalRepository** — 设备信息
- 存储设备本地持久化信息（deviceId, deviceName, ipAddress, fwVersion）

---

### 2.8 NetworkClient/ — 网络通信

```
NetworkClient/
├── networkclient.h / .cpp        # 网络客户端总控（单例）
├── connectionmanager.h / .cpp    # TCP 连接管理
├── heartbeatmanager.h / .cpp     # 心跳保活
├── messagewriter.h / .cpp        # 消息写入器
├── messagereader.h / .cpp        # 消息读取器
├── messagequeue.h / .cpp         # 消息队列
├── authenticator.h / .cpp        # 设备认证
├── outboxmanager.h / .cpp        # 考勤出箱管理
└── serverprotocol.h / .cpp       # 消息协议定义
```

**NetworkClient** — 网络客户端总控
- 单例模式
- 对外接口：连接、认证、考勤上报、数据同步、消息发送
- 内部组合：ConnectionManager、HeartbeatManager、MessageWriter、MessageReader、MessageQueue、Authenticator、OutboxManager
- 消息路由：根据 `ServerProtocol::MessageType` 分发到对应处理函数
- 运行在独立的 `m_networkThread` 线程

**ConnectionManager** — 连接管理
- TCP 连接建立与断开
- 状态机：`Disconnected → Connecting → Connected → Authenticated`
- 自动重连机制（指数退避，最大 30 秒，最多 5 次）
- `scheduleReconnect()` 断开当前连接但保留自动重连能力（用于密钥更新后重连）

**HeartbeatManager** — 心跳保活
- 定时发送心跳包（周期由 auth_response 决定，默认 30 秒）
- 超时检测（阈值 = 心跳间隔 × 3），触发断线重连
- `onAnyMessage()` 收到任意消息时重置超时计时器

**MessageWriter** — 消息写入
- JSON 序列化
- 二进制帧发送

**MessageReader** — 消息读取
- JSON 反序列化
- 二进制帧接收

**MessageQueue** — 消息队列
- 待发消息排队
- 按优先级处理

**Authenticator** — 设备认证
- 发送 `auth` 请求（`sendAuthRequest`）
- 处理 `auth_response`（`handleAuthResponse`）
- 管理会话令牌
- 设备密钥轮换通知（`deviceKeyUpdated` 信号）

**OutboxManager** — 出箱管理
- `enqueue` / `enqueueWithPhoto` → 入队考勤记录
- `processOutbox` → 调度发送（通过 MessageWriter）
- `handleUploadResponse` / `handleServerError` → 响应处理
- 退避重试机制（`onRetryTick` 定时器驱动，`m_retryRound` 轮次递增）
- `rollbackSendingToPending` → 断连时回滚 sending 状态

**ServerProtocol** — 消息协议
- 消息信封（Envelope）构建与解析
- 消息类型常量定义
- 错误码定义
- 数据结构（PersonData、AttendanceRecord、PersonSyncItem）
- 消息构建器（buildAuth、buildHeartbeat、buildAttendanceReport 等）

---

### 2.9 Sync/ — 数据同步

```
Sync/
├── SyncManager.h / .cpp   # 全量同步管理器
```

**SyncManager** — 同步管理
- 状态机：`Idle → SyncingPersons → SyncingFaces → Committing`
- 同步流程：`sync.request` → `person.sync` → `face.sync.begin` → `face.sync.item.header` × N → `face.sync.end` → `sync.ack`
- 信号：`syncStarted`、`syncCompleted`、`syncFailed`、`requestFaceDbReload`

---

### 2.10 UI/ — 用户界面

```
UI/
├── setwindow.h / .cpp / .ui      # 设置窗口
├── facevideowidget.h / .cpp      # 人脸视频渲染组件
├── Components/                    # 可复用组件
│   ├── ActionButton.h / .cpp     # 操作按钮
│   ├── IconButton.h / .cpp       # 图标按钮
│   ├── InfoCard.h / .cpp         # 信息卡片
│   ├── InfoField.h / .cpp        # 信息字段
│   ├── SectionPanel.h / .cpp     # 分区面板
│   ├── StatusIndicator.h / .cpp  # 状态指示器
│   └── ToastNotification.h / .cpp # Toast 通知
└── Theme/                         # 主题系统
    ├── DesignTokens.h             # 设计令牌（颜色、间距、字号等）
    └── ThemeManager.h / .cpp     # 主题管理器
```

**Components** 组件说明：

| 组件 | 说明 |
|------|------|
| ActionButton | 带样式的操作按钮，支持主色/次色变体 |
| IconButton | 图标按钮，用于窗口控制等 |
| InfoCard | 卡片式信息展示容器 |
| InfoField | 标签 + 值的信息字段 |
| SectionPanel | 带标题的分区面板 |
| StatusIndicator | 状态指示器（在线/离线/同步中等） |
| ToastNotification | 临时通知消息 |

**Theme** 主题系统：
- `DesignTokens.h`：定义设计令牌常量（Primitive 色板、Semantic 令牌、字体族、字号、字重、间距、圆角）
- `ThemeManager`（单例）：令牌映射表，提供 `color()`、`font()`、`space()`、`radius()` 访问方法，加载补充 QSS

---

### 2.11 Utils/ — 工具类

```
Utils/
├── Logger.h / .cpp           # 日志系统
└── DatabaseManager.h         # 数据库连接管理
```

**Logger** — 日志系统
- 单例模式
- 日志级别：`Info`、`Warn`、`Error`（无 Debug 级别）
- 日志分类（Category）：System、Login、Data、Config、Network、Attendance、Sync、Recognition、Command
- 通过分类宏使用：`LOG_ATT()`、`LOG_FACE()`、`LOG_NET()` 等
- 文件输出，自动清理旧日志（默认保留 30 天）

**DatabaseManager** — 数据库管理
- header-only 静态工具类（仅 `DatabaseManager.h`，无 `.cpp`）
- `getDatabase(dbPath)` → 线程安全的 SQLite 连接（按线程名创建独立连接）
- 自动设置 WAL 模式、外键约束、UTF-8 编码

---

### 2.12 third_party/ — 第三方库

```
third_party/
└── arcface/
    ├── include/              # SDK 头文件
    │   ├── amcomdef.h        # 基础类型定义
    │   ├── arcsoft_face_sdk.h # SDK API 声明
    │   ├── asvloffscreen.h   # 离屏图像结构
    │   └── merror.h          # 错误码定义
    └── lib/                  # SDK 库文件
        └── libarcsoft_face_engine.lib
```

---

## 3. 类关系图

### 3.1 核心类依赖关系

```
MainWindow (主线程)
    ├── owns → CameraCapture
    ├── owns → VideoFrameCapture
    ├── owns → FaceRecognizer (→ m_faceThread)
    ├── owns → NetworkClient (→ m_networkThread)
    ├── owns → SyncManager
    ├── owns → CommandHandler
    ├── owns → AttendanceReporter
    ├── owns → LocalStorage (单例访问)
    ├── owns → ConfigManager (单例访问)
    └── owns → SetWindow

FaceRecognizer (人脸线程)
    ├── uses → ArcFaceEngine
    └── uses → FaceDatabaseManager

NetworkClient (网络线程)
    ├── owns → ConnectionManager
    ├── owns → HeartbeatManager
    ├── owns → MessageWriter
    ├── owns → MessageReader
    ├── owns → MessageQueue
    ├── owns → Authenticator
    ├── owns → OutboxManager
    ├── owns → TokenManager
    └── owns → TokenRefresher

AttendanceReporter (主线程)
    ├── uses → LocalStorage::outbox()
    └── uses → NetworkClient

SyncManager (主线程)
    ├── uses → LocalStorage
    └── uses → NetworkClient::sendJson()

CommandHandler (主线程)
    └── uses → NetworkClient::sendJson()

LocalStorage (主线程)
    ├── owns → FaceFeatureRepository
    ├── owns → AttendanceOutboxRepository
    ├── owns → SyncMetaRepository
    └── owns → DeviceLocalRepository
```

### 3.2 信号槽连接（关键路径）

```
VideoFrameCapture::frameCaptured ──→ FaceRecognizer::processFrame
FaceRecognizer::recognitionSuccess ──→ MainWindow::onRecognitionSuccess
FaceRecognizer::requestSaveAttendance ──→ MainWindow::onSaveAttendanceRequest (QueuedConnection)
FaceRecognizer::faceDetected ──→ MainWindow (lambda → FaceVideoWidget::setFaceRects)
FaceRecognizer::faceProcessingCompleted ──→ VideoFrameCapture::onFaceProcessingDone
VideoFrameCapture::frameForDisplay ──→ MainWindow (lambda → FaceVideoWidget::setCurrentFrame)
NetworkClient::networkStateChanged ──→ MainWindow::onNetworkStateChanged (QueuedConnection)
NetworkClient::networkStateChanged ──→ AttendanceReporter::onConnectionStateChanged (QueuedConnection)
NetworkClient::attendanceReportResult ──→ AttendanceReporter::onReportResult (QueuedConnection)
NetworkClient::deviceCommandReceived ──→ CommandHandler::handleCommand (QueuedConnection)
NetworkClient::personSyncReceived ──→ SyncManager::handlePersonSync (QueuedConnection)
NetworkClient::faceSyncBeginReceived ──→ SyncManager::handleFaceSyncBegin (QueuedConnection)
NetworkClient::faceSyncEndReceived ──→ SyncManager::handleFaceSyncEnd (QueuedConnection)
NetworkClient::faceSyncItemReceived ──→ SyncManager::handleFaceItem (QueuedConnection)
NetworkClient::authSuccess ──→ SyncManager::requestSync (QueuedConnection)
NetworkClient::devicePendingAuth ──→ MainWindow (lambda, 30 秒后重试)
SyncManager::requestFaceDbReload ──→ FaceDatabaseManager::reload (QueuedConnection)
SyncManager::sendMessage ──→ NetworkClient::sendJson (QueuedConnection)
CommandHandler::sendMessage ──→ NetworkClient::sendJson (QueuedConnection)
CommandHandler::resyncRequested ──→ SyncManager::requestSync
CommandHandler::configApplied ──→ MainWindow (lambda, DeviceKey 变更检测 → 重连)
```

---

## 4. 文件清单（94 个源文件）

### 4.1 核心源文件

| 模块 | 文件 | 符号数 |
|------|------|--------|
| 入口 | `main.cpp` | 14 |
| 主窗口 | `mainwindow.cpp` / `.h` | 40 / 24 |

### 4.2 Attendance 模块（10 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `AttendanceRuleEngine.h` | 7 | 规则引擎声明 |
| `AttendanceRuleEngine.cpp` | 16 | 规则引擎实现 |
| `AttendanceReporter.h` | 6 | 上报协调器声明 |
| `AttendanceReporter.cpp` | 12 | 上报协调器实现 |
| `AttendanceConfigParser.h` | 5 | 配置解析声明 |
| `AttendanceConfigParser.cpp` | 12 | 配置解析实现 |
| `AttendanceConfigSyncHandler.h` | 6 | 配置同步声明 |
| `AttendanceConfigSyncHandler.cpp` | 24 | 配置同步实现 |
| `AttendanceConfigValidator.h` | 7 | 配置校验声明 |
| `AttendanceConfigValidator.cpp` | 10 | 配置校验实现 |

### 4.3 Auth 模块（4 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `tokenmanager.h` | 7 | 令牌管理声明 |
| `tokenmanager.cpp` | 11 | 令牌管理实现 |
| `tokenrefresher.h` | 7 | 令牌刷新声明 |
| `tokenrefresher.cpp` | 11 | 令牌刷新实现 |

### 4.4 CameraCapture 模块（6 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `cameracapture.h` | 7 | 摄像头管理声明 |
| `cameracapture.cpp` | 9 | 摄像头管理实现 |
| `videoframecapture.h` | 12 | 帧捕获声明 |
| `videoframecapture.cpp` | 12 | 帧捕获实现 |
| `videoframeconverter.h` | 4 | 帧转换声明 |
| `videoframeconverter.cpp` | 7 | 帧转换实现 |

### 4.5 Command 模块（2 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `CommandHandler.h` | 5 | 指令处理声明 |
| `CommandHandler.cpp` | 12 | 指令处理实现 |

### 4.6 Config 模块（2 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `configmanager.h` | 85 | 配置管理声明（含大量 getter/setter） |
| `configmanager.cpp` | 29 | 配置管理实现 |

### 4.7 FaceRecognition 模块（8 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `arcfaceengine.h` | 16 | ArcFace 引擎声明 |
| `arcfaceengine.cpp` | 12 | ArcFace 引擎实现 |
| `facedatabasemanager.h` | 6 | 人脸数据库声明 |
| `facedatabasemanager.cpp` | 12 | 人脸数据库实现 |
| `facefeatureextractor.h` | 5 | 特征提取声明 |
| `facefeatureextractor.cpp` | 4 | 特征提取实现 |
| `facerecognizer.h` | 22 | 识别状态机声明 |
| `facerecognizer.cpp` | 18 | 识别状态机实现 |

### 4.8 LocalStorage 模块（11 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `localstorage.h` | 9 | 数据库总控声明 |
| `localstorage.cpp` | 26 | 数据库总控实现 |
| `BaseRepository.h` | 13 | Repository 基类 |
| `FaceFeatureRepository.h` | 7 | 人脸特征存储声明 |
| `FaceFeatureRepository.cpp` | 9 | 人脸特征存储实现 |
| `AttendanceOutboxRepository.h` | 8 | 打卡出箱声明 |
| `AttendanceOutboxRepository.cpp` | 13 | 打卡出箱实现 |
| `SyncMetaRepository.h` | 5 | 同步元数据声明 |
| `SyncMetaRepository.cpp` | 9 | 同步元数据实现 |
| `DeviceLocalRepository.h` | 5 | 设备信息声明 |
| `DeviceLocalRepository.cpp` | 9 | 设备信息实现 |

### 4.9 NetworkClient 模块（18 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `networkclient.h` | 24 | 网络客户端总控声明 |
| `networkclient.cpp` | 42 | 网络客户端总控实现 |
| `connectionmanager.h` | 5 | 连接管理声明 |
| `connectionmanager.cpp` | 15 | 连接管理实现 |
| `heartbeatmanager.h` | 5 | 心跳管理声明 |
| `heartbeatmanager.cpp` | 14 | 心跳管理实现 |
| `messagewriter.h` | 5 | 消息写入声明 |
| `messagewriter.cpp` | 11 | 消息写入实现 |
| `messagereader.h` | 8 | 消息读取声明 |
| `messagereader.cpp` | 10 | 消息读取实现 |
| `messagequeue.h` | 6 | 消息队列声明 |
| `messagequeue.cpp` | 9 | 消息队列实现 |
| `authenticator.h` | 12 | 认证器声明 |
| `authenticator.cpp` | 13 | 认证器实现 |
| `outboxmanager.h` | 9 | 出箱管理声明 |
| `outboxmanager.cpp` | 17 | 出箱管理实现 |
| `serverprotocol.h` | 23 | 消息协议定义 |
| `serverprotocol.cpp` | 21 | 消息协议实现 |

### 4.10 Sync 模块（2 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `SyncManager.h` | 10 | 同步管理声明 |
| `SyncManager.cpp` | 12 | 同步管理实现 |

### 4.11 UI 模块（16 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `setwindow.h` | 6 | 设置窗口声明 |
| `setwindow.cpp` | 63 | 设置窗口实现 |
| `facevideowidget.h` | 9 | 视频组件声明 |
| `facevideowidget.cpp` | 7 | 视频组件实现 |
| `Components/ActionButton.h` | 7 | 操作按钮声明 |
| `Components/ActionButton.cpp` | 9 | 操作按钮实现 |
| `Components/IconButton.h` | 3 | 图标按钮声明 |
| `Components/IconButton.cpp` | 7 | 图标按钮实现 |
| `Components/InfoCard.h` | 4 | 信息卡片声明 |
| `Components/InfoCard.cpp` | 9 | 信息卡片实现 |
| `Components/InfoField.h` | 4 | 信息字段声明 |
| `Components/InfoField.cpp` | 11 | 信息字段实现 |
| `Components/SectionPanel.h` | 5 | 分区面板声明 |
| `Components/SectionPanel.cpp` | 9 | 分区面板实现 |
| `Components/StatusIndicator.h` | 4 | 状态指示声明 |
| `Components/StatusIndicator.cpp` | 12 | 状态指示实现 |
| `Components/ToastNotification.h` | 5 | Toast 通知声明 |
| `Components/ToastNotification.cpp` | 13 | Toast 通知实现 |
| `Theme/DesignTokens.h` | 4 | 设计令牌 |
| `Theme/ThemeManager.h` | 7 | 主题管理声明 |
| `Theme/ThemeManager.cpp` | 17 | 主题管理实现 |

### 4.12 Utils 模块（3 文件）

| 文件 | 符号数 | 说明 |
|------|--------|------|
| `Logger.h` | 19 | 日志系统声明 |
| `Logger.cpp` | 16 | 日志系统实现 |
| `DatabaseManager.h` | 8 | 数据库管理声明 |

### 4.13 third_party（4 文件）

| 文件 | 说明 |
|------|------|
| `amcomdef.h` | ArcFace 基础类型 |
| `arcsoft_face_sdk.h` | ArcFace API 声明 |
| `asvloffscreen.h` | 离屏图像结构 |
| `merror.h` | ArcFace 错误码 |

---

## 5. 资源文件

```
resources/
├── resources.qrc         # Qt 资源文件
├── qss/
│   └── mainwindow.qss    # 主窗口样式表
└── models/               # ArcFace 运行时 DLL（不入版本控制）
```
