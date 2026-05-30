# AttendanceSystem 项目说明文档

## 1. 项目概述

AttendanceSystem 是一款基于 **Qt 6** 的人脸识别考勤终端应用，部署于设备端，支持人脸检测与识别、考勤规则判定、打卡记录上报、离线数据缓存和远程配置同步。系统通过 TCP 长连接与后端服务器通信，实现人员数据同步、考勤记录上传和远程指令下发。

### 1.1 核心能力

- **人脸识别**：集成 ArcFace SDK，实现人脸检测、特征提取、1:N 比对
- **考勤判定**：支持标准工时和弹性工时两种考勤规则，自动判定迟到/早退/正常
- **网络通信**：TCP 长连接 + 心跳保活 + JWT 认证 + 消息队列
- **离线支持**：打卡记录本地持久化（Outbox 模式），联网后自动重试上报
- **数据同步**：全量同步人员和人脸特征数据
- **远程管理**：支持服务器下发远程指令（重启、重同步、配置更新等）

---

## 2. 技术栈

| 技术 | 版本/说明 |
|------|-----------|
| Qt | 6.x（Widgets, Network, Sql, Multimedia, MultimediaWidgets） |
| C++ | C++17 |
| SQLite | 3.x（本地数据存储） |
| ArcFace SDK | 虹软人脸识别引擎 |
| CMake | 3.16+ |
| 工具链 | MinGW 64-bit（Qt Creator） |

---

## 3. 构建与运行

### 3.1 环境要求

- Qt 6.x（包含 Widgets、Network、Sql、Multimedia 模块）
- CMake 3.16+
- MinGW 64-bit 或 MSVC 编译器
- ArcFace SDK（位于 `third_party/arcface/`）

### 3.2 构建步骤

```bash
# 方式一：命令行构建
mkdir build && cd build
cmake ..
cmake --build .

# 方式二：Qt Creator
# 直接打开 CMakeLists.txt，Qt Creator 会自动配置和构建
```

### 3.3 运行时依赖

- `resources/models/` 下的 ArcFace DLL 文件
- `config/config.ini` 和 `config/local.ini` 配置文件（首次运行自动生成）

---

## 4. 系统架构

### 4.1 整体架构图

```
┌─────────────────────────────────────────────────────────────┐
│                        MainWindow (UI)                       │
│  ┌──────────┐  ┌──────────┐  ┌───────────┐  ┌───────────┐  │
│  │ 视频预览  │  │ 信息展示  │  │ 状态指示  │  │ 设置窗口  │  │
│  └────┬─────┘  └────┬─────┘  └───────────┘  └───────────┘  │
│       │              │                                       │
├───────┼──────────────┼───────────────────────────────────────┤
│       ▼              ▼          业务层                        │
│  ┌──────────┐  ┌──────────────┐  ┌───────────────────────┐  │
│  │ Camera   │  │ FaceRecognizer│  │  AttendanceReporter   │  │
│  │ Capture  │  │  (状态机)     │  │  (Outbox 协调器)      │  │
│  └────┬─────┘  └──────┬───────┘  └──────────┬────────────┘  │
│       │               │                      │               │
│       ▼               ▼                      ▼               │
│  ┌──────────┐  ┌──────────────┐  ┌───────────────────────┐  │
│  │ VideoFrame│  │ ArcFaceEngine│  │  AttendanceRuleEngine │  │
│  │ Capture/  │  │ FaceDatabase │  │  (标准/弹性工时)      │  │
│  │ Converter │  │ Manager      │  │                       │  │
│  └──────────┘  └──────────────┘  └───────────────────────┘  │
│                                                              │
├──────────────────────────────────────────────────────────────┤
│                      数据与网络层                             │
│  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐   │
│  │  LocalStorage │  │ NetworkClient│  │  SyncManager    │   │
│  │  (SQLite)     │  │ (TCP+JWT)    │  │  (全量同步)     │   │
│  │  ┌──────────┐ │  │ ┌──────────┐ │  └─────────────────┘   │
│  │  │Repositories│ │  │ │Connection│ │                       │
│  │  │ Outbox    │ │  │ │ Heartbeat│ │  ┌─────────────────┐   │
│  │  │ FaceFeature│ │  │ │ Reader   │ │  │ CommandHandler  │   │
│  │  │ SyncMeta  │ │  │ │ Writer   │ │  │ (远程指令)      │   │
│  │  │ DeviceLocal│ │  │ │ Queue    │ │  └─────────────────┘   │
│  │  └──────────┘ │  │ │ Auth     │ │                       │
│  └──────────────┘  │ │ Outbox   │ │  ┌─────────────────┐   │
│                     │ └──────────┘ │  │ ConfigManager   │   │
│                     └──────────────┘  │ (配置管理)      │   │
│                                        └─────────────────┘   │
├──────────────────────────────────────────────────────────────┤
│                        基础设施层                             │
│  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐   │
│  │  Logger      │  │  ThemeManager│  │  ServerProtocol │   │
│  │  (日志系统)   │  │  (主题/令牌) │  │  (消息协议)     │   │
│  └──────────────┘  └──────────────┘  └─────────────────┘   │
└──────────────────────────────────────────────────────────────┘
```

### 4.2 线程模型

| 线程 | 职责 | 运行组件 |
|------|------|----------|
| **主线程** | UI 渲染、数据库操作、业务协调 | MainWindow、LocalStorage、AttendanceReporter、ConfigManager |
| **人脸线程** (`m_faceThread`) | 人脸检测与识别（CPU 密集） | FaceRecognizer、ArcFaceEngine、FaceDatabaseManager |
| **网络线程** (`m_networkThread`) | TCP 通信、消息收发 | NetworkClient、ConnectionManager、HeartbeatManager、MessageReader/Writer |

跨线程通信全部通过 Qt 信号槽机制（自动连接类型），确保线程安全。

---

## 5. 核心流程

### 5.1 打卡主流程

```
摄像头采集帧
    │
    ▼
VideoFrameCapture (帧捕获 + 格式转换)
    │ 信号: frameCaptured(QImage)
    ▼
FaceRecognizer::processFrame (人脸线程)
    │
    ├── 人脸检测 (ArcFaceEngine::detectFace)
    ├── 特征提取 (ArcFaceEngine::extractFeature)
    ├── 特征比对 (FaceDatabaseManager::findBestMatch)
    │
    ▼
状态机判定
    │
    ├── IDLE → DETECTING (检测到人脸)
    ├── DETECTING → RECOGNIZED (识别成功)
    ├── RECOGNIZED → LOST (人脸离开)
    └── LOST → IDLE (冷却结束)
    │
    ▼ 信号: recognitionSuccess(employeeId, name, status, checkTime, faceImage)
MainWindow::onRecognitionSuccess (主线程)
    │
    ▼
AttendanceRuleEngine::evaluate (考勤规则判定)
    │  返回: 正常/迟到/早退
    ▼
AttendanceReporter::report / reportWithPhoto
    │
    ├── 写入 Outbox (SQLite 持久化)
    └── 通过 NetworkClient 发送到服务器
    │
    ▼
服务器响应 → 更新 Outbox 状态
```

### 5.2 识别状态机

```
         检测到人脸              识别成功
IDLE ──────────────► DETECTING ──────────────► RECOGNIZED
 ▲                                               │
 │                     人脸离开                    │
 └──────────────── LOST ◄────────────────────────┘
                    │
                    │ 冷却 3 秒
                    ▼
                   IDLE
```

状态说明：
- **IDLE**：空闲状态，等待人脸出现
- **DETECTING**：检测到人脸，等待识别结果
- **RECOGNIZED**：识别成功，显示人员信息
- **LOST**：人脸离开视野，进入冷却期

### 5.3 网络通信协议

消息采用 JSON 格式，通过 TCP 长连接传输，协议信封结构：

```json
{
    "type": "attendance.report",
    "role": "device",
    "from": "device_001",
    "to": "server",
    "msgId": "uuid-xxxx",
    "ts": 1717000000000,
    "data": { ... }
}
```

消息类型：

| 类别 | 消息类型 | 方向 | 说明 |
|------|----------|------|------|
| 认证 | `auth` / `auth_response` | 设备→服务器 | 设备认证 |
| 心跳 | `heartbeat` / `heartbeat_response` | 双向 | 保活探测 |
| 考勤 | `attendance.report` | 设备→服务器 | 打卡记录上报 |
| 同步 | `sync.request` → `person.sync` → `face.sync.*` → `sync.ack` | 双向 | 全量数据同步 |
| 指令 | `device.command` / `device.command.ack` | 服务器→设备 | 远程指令 |
| 令牌 | `token.refresh` / `token.refresh.response` | 设备→服务器 | JWT 刷新 |

### 5.4 全量同步流程

```
设备认证成功
    │
    ▼
SyncManager::requestSync()
    │  发送 sync.request
    ▼
服务器返回 person.sync（人员列表）
    │  解析人员数据，写入 SQLite
    ▼
服务器发送 face.sync.begin
    │
    ├── face.sync.item.header + 二进制特征数据（逐条）
    ├── ...
    └── face.sync.end
    │
    ▼
SyncManager 提交事务，发送 sync.ack
    │
    ▼
触发人脸库重载 (requestFaceDbReload)
```

### 5.5 离线打卡与 Outbox 机制

```
AttendanceReporter::report()
    │
    ├── 写入 AttendanceOutbox (SQLite，状态: pending)
    │
    ├── 如果在线 → 立即通过 OutboxManager 发送
    │     ├── 成功 → 标记为已移除
    │     └── 失败 → 保持 pending / 标记 dead（超过最大重试次数）
    │
    └── 如果离线 → 保持 pending
          │
          ▼ 网络恢复后
    AttendanceReporter::retryAll()
          │
          └── 遍历所有 pending 记录，逐条发送

Outbox 记录状态：pending → sending → failed → dead
```

---

## 6. 模块详解

### 6.1 CameraCapture（摄像头采集）

负责摄像头初始化、视频帧捕获和格式转换。

| 类 | 职责 |
|----|------|
| `CameraCapture` | 摄像头生命周期管理（初始化/启动/停止） |
| `VideoFrameCapture` | 视频帧捕获，输出 QImage |
| `VideoFrameConverter` | 帧格式转换（YUV → RGB 等） |

### 6.2 FaceRecognition（人脸识别）

集成 ArcFace SDK，实现完整的人脸识别流水线。

| 类 | 职责 |
|----|------|
| `ArcFaceEngine` | ArcFace SDK 封装（初始化、检测、特征提取、比对） |
| `FaceDatabaseManager` | 人脸特征内存数据库（加载 SQLite 数据、1:N 比对） |
| `FaceFeatureExtractor` | 特征提取工具类（`FaceExtraction` 单方法封装） |
| `FaceRecognizer` | 识别状态机，串联检测→提取→比对全流程 |

### 6.3 Attendance（考勤业务）

考勤规则引擎和上报协调。

| 类 | 职责 |
|----|------|
| `AttendanceRuleEngine` | 考勤规则判定（标准/弹性工时，迟到/早退计算） |
| `AttendanceReporter` | 打卡上报协调器（Outbox 持久化 → 发送 → 响应 → 清理） |
| `AttendanceConfigParser` | 考勤配置解析 |
| `AttendanceConfigSyncHandler` | 远程考勤配置同步处理 |
| `AttendanceConfigValidator` | 考勤配置校验 |

### 6.4 NetworkClient（网络客户端）

TCP 长连接通信，采用组合模式拆分为多个专职组件。

| 类 | 职责 |
|----|------|
| `NetworkClient` | 总控（对外接口、消息路由、组件协调） |
| `ConnectionManager` | TCP 连接管理（连接/断开/重连） |
| `HeartbeatManager` | 心跳保活（定时发送、超时检测） |
| `MessageWriter` | 消息序列化与发送 |
| `MessageReader` | 消息接收与反序列化 |
| `MessageQueue` | 待发消息队列 |
| `Authenticator` | 设备认证流程 |
| `OutboxManager` | 考勤记录出箱管理 |
| `ServerProtocol` | 消息协议（构建器、解析器、常量定义） |

### 6.5 Auth（认证模块）

JWT 令牌生命周期管理。

| 类 | 职责 |
|----|------|
| `TokenManager` | 令牌存储与查询 |
| `TokenRefresher` | 令牌自动刷新 |

### 6.6 LocalStorage（本地存储）

SQLite 数据持久化，采用 Repository 模式。

| 类 | 职责 |
|----|------|
| `LocalStorage` | 数据库总控（单例、连接管理、迁移、Repository 访问器） |
| `BaseRepository` | Repository 基类（公共数据库路径） |
| `FaceFeatureRepository` | 人脸特征数据存储 |
| `AttendanceOutboxRepository` | 打卡记录出箱缓存 |
| `SyncMetaRepository` | 同步元数据存储 |
| `DeviceLocalRepository` | 设备本地信息存储 |

### 6.7 Sync（数据同步）

全量同步流程管理。

| 类 | 职责 |
|----|------|
| `SyncManager` | 同步状态机（Idle → SyncingPersons → SyncingFaces → Committing） |

### 6.8 Command（远程指令）

服务器远程指令处理。

| 类 | 职责 |
|----|------|
| `CommandHandler` | 指令解析与执行（resync、reboot、config 更新等） |

### 6.9 Config（配置管理）

应用程序配置读写。

| 类 | 职责 |
|----|------|
| `ConfigManager` | 配置管理单例（双文件策略：config.ini 可远程覆盖，local.ini 仅本机） |

配置分类：
- **网络连接**：服务器 IP、端口、超时
- **人脸识别**：阈值、最大人脸数、超时、摄像头旋转
- **ArcFace SDK**：AppId、SdkKey
- **考勤规则**：上下班时间、迟到/早退容许、弹性工时、午休等
- **设备信息**：设备 ID、设备密钥、固件版本
- **同步设置**：自动同步、超时
- **重试设置**：最大重试次数、退避基数
- **存储路径**：数据库路径、日志路径

### 6.10 UI（用户界面）

| 类 | 职责 |
|----|------|
| `MainWindow` | 主窗口（视频预览、信息展示、状态指示、窗口控制） |
| `SetWindow` | 设置窗口 |
| `FaceVideoWidget` | 人脸视频渲染组件 |
| `ThemeManager` | 主题管理（设计令牌、QSS 加载） |
| `ActionButton` | 可复用操作按钮 |
| `IconButton` | 图标按钮 |
| `InfoCard` | 信息卡片 |
| `InfoField` | 信息字段（标签 + 值） |
| `SectionPanel` | 分区面板 |
| `StatusIndicator` | 状态指示器（在线/离线等） |
| `ToastNotification` | Toast 通知 |

### 6.11 Utils（工具类）

| 类 | 职责 |
|----|------|
| `Logger` | 日志系统（单例，文件输出，按 Category 分类，自动清理旧日志） |
| `DatabaseManager` | header-only 静态工具类，线程安全的 SQLite 连接管理（`getDatabase`） |

**Logger 日志级别**：`Info`、`Warn`、`Error`（无 Debug 级别）

**Logger 日志分类**（通过宏使用）：
- `LOG_SYS()` — 系统日志
- `LOG_LOGIN()` — 登录日志
- `LOG_DATA()` — 数据日志
- `LOG_CFG()` — 配置日志
- `LOG_NET()` — 网络日志
- `LOG_ATT()` — 考勤日志
- `LOG_SYNC()` — 同步日志
- `LOG_FACE()` — 识别日志
- `LOG_CMD()` — 指令日志

---

## 7. 配置文件

| 文件 | 说明 |
|------|------|
| `config.ini` | 可远程覆盖的配置（人脸识别阈值、考勤规则等），路径由 `ConfigManager::getConfigFilePath()` 决定 |
| `local.ini` | 本机配置（服务器地址、窗口尺寸等），路径由 `ConfigManager::getLocalConfigFilePath()` 决定 |

---

## 8. 调试指南

### 8.1 日志系统

项目使用 `Logger` 分类宏输出日志，在 Qt Creator 中可按分类过滤：

| 宏 | 分类 | 用途 |
|----|------|------|
| `LOG_ATT(level, msg)` | Attendance | 考勤打卡相关 |
| `LOG_FACE(level, msg)` | Recognition | 人脸识别相关 |
| `LOG_NET(level, msg)` | Network | 网络通信相关 |
| `LOG_SYNC(level, msg)` | Sync | 数据同步相关 |
| `LOG_CMD(level, msg)` | Command | 远程指令相关 |
| `LOG_CFG(level, msg)` | Config | 配置变更相关 |
| `LOG_DATA(level, msg)` | Data | 数据库操作相关 |
| `LOG_SYS(level, msg)` | System | 系统启动/关闭 |
| `LOG_LOGIN(level, msg)` | Login | 认证登录相关 |

### 8.2 关键调试点

| 阶段 | 模块 | 说明 |
|------|------|------|
| 人脸检测 | FaceRecognizer | 检测结果、置信度 |
| 特征提取 | ArcFaceEngine | 提取耗时、特征质量 |
| 特征比对 | FaceDatabaseManager::findBestMatch | 比对分数、最佳匹配 |
| 规则判定 | AttendanceRuleEngine | 判定结果（正常/迟到/早退） |
| 打卡上报 | AttendanceReporter | Outbox 写入、发送结果 |
| 服务器响应 | NetworkClient | 响应码、消息内容 |

---

## 9. 第三方依赖

| 库 | 位置 | 用途 |
|----|------|------|
| ArcFace SDK | `third_party/arcface/` | 人脸检测与识别引擎 |
| Qt 6 | 系统安装 | UI 框架、网络、数据库、多媒体 |
| SQLite | Qt 内置 | 本地数据存储 |
