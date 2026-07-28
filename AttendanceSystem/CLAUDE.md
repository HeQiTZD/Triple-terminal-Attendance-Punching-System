# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

AttendanceSystem 是一个基于 Qt6 的人脸识别考勤设备端应用，支持人脸检测与识别、考勤规则判定、打卡记录上报、离线数据缓存和远程配置同步。

## 技术栈

- **Qt 6.x** (Widgets, Network, Sql, Multimedia, MultimediaWidgets)
- **C++17**
- **SQLite 3.x** (本地数据存储)
- **ArcFace SDK** (人脸识别引擎)
- **CMake 3.16+** (构建系统)
- **Qt Creator** (主要IDE，使用 MinGW 64-bit 工具链)

## 构建命令

```bash
# 在项目根目录下创建构建目录
mkdir build && cd build

# 配置项目（Qt Creator 会自动执行）
cmake ..

# 构建项目
cmake --build .

# 或者使用 Qt Creator 直接打开 CMakeLists.txt 进行构建
```

构建产物位于 `build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug/` 目录下。

## 代码架构

### 核心模块

```
AttendanceSystem/
├── Attendance/          # 考勤业务：规则引擎、配置解析、打卡上报
├── Auth/                # 认证模块：令牌管理与刷新
├── CameraCapture/       # 摄像头采集：视频帧捕获与格式转换
├── Command/             # 远程指令处理
├── Config/              # 配置管理（单例 ConfigManager）
├── FaceRecognition/     # 人脸识别：ArcFace SDK 封装、状态机识别流程
├── LocalStorage/        # 本地存储：SQLite Repository 模式
├── NetworkClient/       # 网络客户端：连接管理、心跳、消息队列
├── Sync/                # 数据同步管理
├── UI/                  # 用户界面：组件库、主题系统
│   ├── Components/      # 可复用 UI 组件（ActionButton, InfoCard 等）
│   └── Theme/           # 主题管理与设计令牌
└── Utils/               # 工具类：Logger, DatabaseManager
```

### 关键架构模式

**线程模型**：
- 主线程：UI 和数据库操作
- `m_faceThread`：人脸识别线程
- `m_networkThread`：网络通信线程（NetworkClient 移至此线程）

**识别状态机**（FaceRecognizer）：
```
IDLE → DETECTING → RECOGNIZED → LOST → IDLE
```

**Repository 模式**（LocalStorage）：
- `FaceFeatureRepository`：人脸特征存储
- `AttendanceOutboxRepository`：打卡出箱缓存（离线支持）
- `SyncMetaRepository`：同步元数据
- `DeviceLocalRepository`：设备信息

**网络消息协议**（ServerProtocol）：
- JSON 格式消息，类型包括 `device.auth.*`、`heartbeat`、`attendance.report`、`person.sync`、`face.sync.*`、`device.command`

### 打卡核心流程

```
摄像头采集 → 人脸检测 → 特征提取 → 特征比对 → 考勤规则判定 → 打卡上报 → 服务器响应
```

## 配置管理

配置文件位于应用运行目录的 `config/` 下：
- `config.ini`：可远程覆盖的配置（人脸识别阈值、考勤规则等）
- `local.ini`：本机配置（服务器地址、窗口尺寸等）

## 调试输出规范

项目使用 `[打卡流程]` 前缀进行打卡流程调试输出：

```cpp
qDebug() << "[打卡流程]" << "描述" << "key=" << value;
```

在 Qt Creator 中使用正则表达式 `\[打卡流程\]` 过滤日志。

关键调试点：FaceRecognizer（检测/提取/比对）、AttendanceRuleEngine（规则判定）、AttendanceReporter（上报）、NetworkClient（服务器响应）。

## 开发注意事项

- 项目使用 Qt 的 `AUTOUIC`、`AUTOMOC`、`AUTORCC`，修改 `.h`/`.ui`/`.qrc` 文件后 CMake 会自动处理
- ArcFace SDK 位于 `third_party/arcface/`，运行时需要 `resources/models/` 下的 DLL 文件
- 数据库迁移脚本位于 `LocalStorage/migrations/`
- QSS 样式文件位于 `resources/qss/`，通过 Qt 资源系统加载
