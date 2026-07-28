# Triple-terminal Attendance Punching System

**三端考勤打卡系统** — 一个企业级考勤管理解决方案，由三个子系统组成：

| 子系统 | 目录 | 角色 | 技术栈 |
|--------|------|------|--------|
| **AttendanceAdmin** | `AttendanceAdmin/` | 管理后台（桌面端） | Qt 6.8+ / QML / C++17 |
| **AttendanceSystem** | `AttendanceSystem/` | 考勤终端（设备端） | Qt 6 / Widgets / ArcFace |
| **attendanceServer** | `attendanceServer/` | 考勤服务器（后端） | C++17 / Boost.Asio / MySQL |

---

## 系统架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        attendanceServer                                │
│                    (C++ 考勤服务器 + MySQL)                              │
│                                                                         │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────────────────────┐   │
│  │ 网络层       │  │ 协议层       │  │ 业务层                      │   │
│  │ Boost.Asio  │→│ JSON 行协议  │→│ · 认证/授权 (JWT + RBAC)    │   │
│  │ TCP/TLS     │  │ 二进制帧     │  │ · 考勤上报 & 归档           │   │
│  │ 连接管理    │  │ 消息路由     │  │ · 人脸注册 & 识别           │   │
│  └─────────────┘  └──────────────┘  │ · 设备管理 & 配置下发       │   │
│                                      │ · 数据同步 & 事件推送       │   │
│                                      │ · CRUD + 操作审计           │   │
│                                      └──────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────┘
          ▲                          ▲
          │ TCP/JSON                  │ TCP/JSON
          │ (device.*)                │ (person.*, attendance.*, ...)
          ▼                          ▼
┌─────────────────────┐    ┌─────────────────────────────────────┐
│ AttendanceSystem    │    │ AttendanceAdmin                     │
│ (考勤终端)          │    │ (管理后台)                           │
│                     │    │                                      │
│ · 摄像头人脸采集    │    │ · 仪表盘 (统计图表)                  │
│ · ArcFace 检测/提取 │    │ · 人员/设备/用户管理                 │
│ · 考勤规则引擎      │    │ · 考勤记录查询 & CSV 导出            │
│ · 离线缓存 & 自动上报│   │ · 人脸数据管理                       │
│ · 远程配置 & 同步   │    │ · RBAC 权限管理                      │
│ · JWT 认证          │    │ · 日志查看器                         │
│ · 深色科技风 UI     │    │ · 事件订阅 & 实时推送                │
└─────────────────────┘    └─────────────────────────────────────┘
```

---

## 子项目详细介绍

### 1. AttendanceAdmin — 管理后台

基于 **Qt 6.8+ / QML / C++17** 的桌面管理客户端，采用 **QML 前端 + C++ 服务层** 架构。无本地数据库，所有数据通过 TCP/JSON 协议与服务端通信。

**核心模块：**

| 模块 | 功能 |
|------|------|
| **仪表盘** | 员工数、在线设备、今日打卡统计、考勤状态饼图、最近打卡列表 |
| **人员管理** | 员工信息 CRUD（姓名/工号/部门/职位）、多条件查询 |
| **设备管理** | 设备 CRUD、在线状态监控、远程命令下发 |
| **考勤记录** | 记录查询、归档管理、CSV 导出 |
| **人脸管理** | 人脸注册（拍照/文件）、特征查询与删除 |
| **系统用户** | 账号 CRUD |
| **RBAC 权限** | 角色 CRUD、用户角色分配、页面级权限控制 |
| **配置部署** | 批量配置下发到指定设备，版本管理+部署状态追踪 |
| **日志查看器** | 按日期浏览服务端日志 |
| **事件订阅** | 订阅设备状态/考勤记录实时推送 |

**UI 组件库：** 28 个可复用 QML 组件（DataTable、Card、图表、对话框、权限按钮等），企业蓝色系主题，支持图表展示（饼图、柱状图、折线图）。

---

### 2. AttendanceSystem — 考勤终端

基于 **Qt 6 / C++17 / Widgets** 的人脸识别考勤设备端应用，集成 **虹软 ArcFace SDK** 进行人脸检测与识别。

**核心业务流程：**

```
摄像头采集 → 人脸检测 → 特征提取 → 特征比对 → 考勤判定 → 上报服务端
  (30fps)    (ArcFace)  (ArcFace)  (内存库)  (规则引擎)  (TCP/JSON)
```

**核心模块：**

| 模块 | 功能 |
|------|------|
| **人脸识别** | ArcFace 人脸检测/特征提取/比对，识别状态机（IDLE→DETECTING→RECOGNIZED→LOST），3 秒冷却防重复 |
| **视频采集** | QCamera 封装，后台线程取帧，QVideoFrame → QImage 格式转换 |
| **考勤规则引擎** | 判定迟到/早退/正常/缺勤，支持标准工时和弹性工时 |
| **网络通信** | TCP 长连接 + JSON 协议，心跳保活，自动重连（指数退避） |
| **离线容错** | 打卡记录先写入 SQLite `attendance_outbox`，网络恢复后自动重发，支持幂等去重 |
| **数据同步** | 全量同步流程：人员数据→人脸特征→提交确认，版本号控制 |
| **远程指令** | 接收服务端下发的 resync/reboot/configApply 指令 |
| **配置管理** | 本地 config.ini + 远程配置同步 |
| **UI 组件库** | 7 个自定义 Widget 组件（ActionButton、StatusIndicator、ToastNotification 等），深色科技风 QSS 主题 |

**线程模型：** 主线程（UI + 数据库写入）+ 人脸识别线程 + 网络线程。

---

### 3. attendanceServer — 考勤服务器

基于 **C++17 / Boost.Asio / MySQL** 的高性能考勤服务端，支持考勤设备端和管理后台的并发连接。

**架构分层：**

```
网络层 (net/)       协议层 (protocol/)      业务层 (service/)      数据层 (db/)
TcpServer ──→ GatewaySessionHandler ──→ *Service ──→ *Repository ──→ MySQL
Session         MessageHandlerRegistry    DbExecutor               ConnectionPool
Framing         ConnContext               DeviceRegistry
TLS             handlers/                 TokenManager
                                           EventSubscription
```

**核心特性：**

| 特性 | 说明 |
|------|------|
| **双端架构** | 同时服务设备端和管理端，通过 ConnContext 角色区分 |
| **认证体系** | 设备认证（device_id + device_key）、管理端认证（username + password），JWT 双令牌（access + refresh） |
| **RBAC 权限** | 基于角色的细粒度权限控制，覆盖所有管理端操作 |
| **人脸识别** | ArcFace 引擎集成，支持服务端特征提取 |
| **MySQL 连接池** | 固定大小连接池，自动健康检查和断线重连 |
| **事件推送** | 管理端可订阅主题，设备上报后实时推送 |
| **消息路由** | 管理端命令转发到在线设备 + 10 秒超时 |
| **操作审计** | 所有管理端操作记录到 operation_log 表 |
| **TLS 加密** | 可选的 SSL/TLS 加密传输 |
| **密码安全** | SHA-256 + 16 字节随机盐，密码强度验证 |
| **设备配置下发** | 配置版本管理，设备上线自动推送 |

**协议消息类型（部分）：**

| 消息类型 | 方向 | 说明 |
|----------|------|------|
| `device.auth` | 设备→服务端 | 设备认证 |
| `attendance.report` | 设备→服务端 | 考勤记录上报 |
| `sync.request` | 设备→服务端 | 数据同步请求 |
| `heartbeat` | 双向 | 心跳保活 |
| `person.*` | 双向 | 人员 CRUD |
| `device.command` | 管理端→设备 | 设备指令下发 |
| `subscribe` | 管理端→服务端 | 事件订阅 |

**数据表（MySQL）：**

`person` / `user_account` / `device` / `attendance_record` / `face_data` / `device_config` / `role` / `permission` / `user_role` / `role_permission` / `operation_log`

---

## 技术栈总览

| 层面 | 技术 |
|------|------|
| **前端（管理后台）** | Qt 6.8+ (QML + Quick Controls 2), Fusion 风格, Direct3D 11 |
| **前端（考勤终端）** | Qt 6 (Widgets), 深色 QSS 主题 |
| **后端** | C++17, Boost.Asio (网络), MySQL 8.0+ X DevAPI |
| **人脸识别** | 虹软 ArcFace SDK (人脸检测/特征提取/比对) |
| **认证授权** | JWT (HMAC-SHA256), RBAC 权限模型 |
| **网络协议** | TCP 长连接, JSON 行协议, 4 字节大端长度前缀二进制帧 |
| **数据存储** | MySQL (服务端), SQLite (终端本地缓存) |
| **第三方库** | nlohmann/json, jwt-cpp, OpenSSL, stb_image |
| **构建工具** | CMake 3.16+, Ninja, Visual Studio 2022, MinGW |
| **目标平台** | Windows 10/11 |

---

## 项目结构

```
E:\peoject-v1/
│
├── AttendanceAdmin/                  # 管理后台 (Qt QML)
│   ├── src/
│   │   ├── Network/                  # TCP 网络层
│   │   ├── Auth/                     # 会话 & JWT 管理
│   │   ├── Person/                   # 人员服务
│   │   ├── Device/                   # 设备服务
│   │   ├── Attendance/               # 考勤服务
│   │   ├── Face/                     # 人脸服务
│   │   ├── User/                     # 用户服务
│   │   ├── Config/                   # 配置部署
│   │   ├── Event/                    # 事件订阅
│   │   ├── Log/                      # 日志服务
│   │   ├── Models/                   # 数据模型
│   │   └── Protocol/                 # 协议定义
│   ├── ui/                           # QML 界面
│   │   ├── pages/                    # 9 个功能页面
│   │   ├── components/               # 28 个可复用组件
│   │   ├── models/                   # 7 个单例 QML 模型
│   │   └── theme/                    # 主题系统
│   └── CMakeLists.txt
│
├── AttendanceSystem/                 # 考勤终端 (Qt Widgets)
│   ├── Attendance/                   # 考勤规则引擎 & 上报
│   ├── Auth/                         # JWT 令牌管理
│   ├── CameraCapture/                # 视频采集 & 帧处理
│   ├── FaceRecognition/              # ArcFace 人脸识别
│   ├── NetworkClient/                # TCP 网络客户端
│   ├── Sync/                         # 数据同步
│   ├── Command/                      # 远程指令处理
│   ├── Config/                       # 配置管理
│   ├── LocalStorage/                 # SQLite 本地存储
│   ├── UI/                           # 界面 & 组件
│   ├── Utils/                        # 工具类
│   └── CMakeLists.txt
│
├── attendanceServer/                 # 考勤服务器 (C++ Backend)
│   ├── net/                          # Boost.Asio 网络层
│   ├── protocol/                     # JSON 协议 & 消息处理器
│   │   └── handlers/                 # 11 个消息类型处理器
│   ├── service/                      # 业务服务层
│   │   └── admin/                    # 管理端 CRUD 服务
│   ├── db/                           # MySQL 数据访问层 (Repository 模式)
│   ├── config/                       # 服务配置
│   ├── util/                         # 工具类 (JWT/密码/ArcFace)
│   ├── arcface/                      # ArcFace SDK 二进制
│   ├── third_party/                  # jwt-cpp 等第三方库
│   └── CMakeLists.txt
│
└── README.md                         # 本文件
```

---

## 快速开始

### 前置要求

- **编译器**: Visual Studio 2022 (管理后台) / MinGW (考勤终端)
- **Qt**: Qt 6.8+ (管理后台) / Qt 6 (考勤终端)
- **CMake**: 3.16+
- **MySQL**: 8.0+ (服务端)
- **Boost**: 1.80+ (服务端)
- **OpenSSL**: 1.1+ (服务端)

### 构建步骤

#### 1. 考勤服务器 (attendanceServer)

```bash
cd attendanceServer
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
# 首次运行会引导配置数据库连接
./build/attendanceServer --config attendance.json
```

#### 2. 管理后台 (AttendanceAdmin)

```bash
cd AttendanceAdmin
cmake --preset x64-debug  # 或手动配置 Qt 路径
cmake --build out/build/debug
./out/build/debug/appAttendanceAdmin.exe
```

#### 3. 考勤终端 (AttendanceSystem)

```bash
cd AttendanceSystem
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"
cmake --build build
./build/AttendanceSystem.exe
```

---

## 通信协议概述

系统使用 **TCP 长连接**，默认端口 `9527`。

### JSON 行协议

请求/响应格式（`\n` 分隔的 JSON 行）：

```json
{
  "type": "person.create",
  "msgId": "req-001",
  "from": "admin_001",
  "to": "server",
  "ts": 1700000000,
  "data": { ... },
  "code": 0
}
```

### 二进制帧

大文件传输（如人脸照片）使用 **4 字节大端长度前缀 + 二进制载荷**，先发送 JSON 元数据头，再发送二进制数据体。

---

## 开发状态

| 子系统 | 状态 | 说明 |
|--------|:----:|------|
| **attendanceServer** | ✅ 完成 | 核心功能完整，支持所有消息类型 |
| **AttendanceSystem** | ✅ 完成 | 人脸打卡流程完整，离线容错，深色 UI |
| **AttendanceAdmin** | ✅ 完成 | 9 个管理页面，完整的权限控制和事件推送 |

---

## 许可证

本项目仅供学习和参考。
