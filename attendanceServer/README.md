# 考勤服务器系统

基于 C++17 的高性能考勤管理服务器，支持设备端考勤打卡、管理端数据查询、人脸注册识别等功能。

## 项目概述

本项目是一个企业级考勤管理系统的服务端实现，采用 TCP 长连接通信协议，支持考勤设备和管理客户端的并发连接。系统集成了虹软（ArcSoft）人脸识别引擎，提供人脸注册和识别功能。

### 核心特性

- **双端架构**：支持设备端（考勤机）和管理端（Web/客户端）同时连接
- **实时通信**：基于 TCP 的 JSON 行协议 + 二进制帧传输
- **人脸识别**：集成虹软 ArcFace SDK，支持人脸特征提取和比对
- **JWT 认证**：设备和管理员双因素认证，支持令牌刷新和撤销
- **RBAC 权限**：基于角色的访问控制，细粒度权限管理
- **连接池**：MySQL 连接池管理，自动健康检查和断线重连
- **TLS 加密**：可选的 TLS/SSL 加密传输
- **事件订阅**：管理端可订阅考勤事件推送
- **设备配置下发**：支持远程设备配置部署

### 技术栈

- **语言**：C++17
- **构建系统**：CMake 3.10+
- **网络库**：Boost.Asio（异步 I/O）
- **数据库**：MySQL 8.0+（X Protocol，端口 33060）
- **JSON 库**：nlohmann/json（单头文件版本）
- **人脸识别**：虹软 ArcFace SDK
- **加密库**：OpenSSL（TLS + HMAC-SHA256）
- **JWT 库**：jwt-cpp
- **密码哈希**：SHA-256 + 随机盐

## 功能特性

### 设备端功能

1. **设备认证**：通过设备 ID 和密钥进行身份验证
2. **心跳保活**：定期心跳检测，超时自动断开
3. **考勤上报**：上报考勤记录（含员工 ID、打卡时间、状态）
4. **照片上传**：上传考勤照片（二进制帧）
5. **配置同步**：接收管理端下发的设备配置
6. **数据同步**：支持人脸数据批量同步

### 管理端功能

1. **管理员认证**：用户名/密码登录，获取 JWT 令牌
2. **人员管理**：增删改查员工信息
3. **考勤管理**：查询、修改、删除考勤记录，支持归档
4. **设备管理**：设备注册、状态监控、配置下发
5. **用户管理**：系统用户账号管理
6. **角色权限**：RBAC 角色和权限配置
7. **人脸管理**：人脸数据查询、删除、注册
8. **事件订阅**：订阅实时考勤事件推送
9. **操作日志**：审计日志记录和查询

### 认证与安全

1. **JWT 令牌**：
   - 访问令牌（Access Token）：短期有效，默认 1 小时
   - 刷新令牌（Refresh Token）：长期有效，默认 7 天
   - 令牌撤销：支持强制下线
2. **密码安全**：
   - SHA-256 + 随机盐哈希存储
   - 密码强度验证（至少 8 位，包含字母和数字）
3. **RBAC 权限**：
   - 角色管理：创建、修改、删除角色
   - 权限分配：为角色分配细粒度权限
   - 用户角色：为用户分配角色
4. **TLS 加密**：可选的 TLS 1.2+ 加密传输

## 技术架构

### 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                      考勤服务器系统                          │
├─────────────────────────────────────────────────────────────┤
│  网络层 (net)                                               │
│  ├── TcpServer: TCP 服务器，监听连接                         │
│  ├── Session: TCP 会话管理                                  │
│  └── TlsSession: TLS 加密会话                               │
├─────────────────────────────────────────────────────────────┤
│  协议层 (protocol)                                          │
│  ├── GatewaySessionHandler: 网关会话处理器                   │
│  ├── LineCodec: JSON 行编解码                               │
│  ├── Envelope: 消息信封解析                                 │
│  ├── MessageHandlerRegistry: 消息处理器注册表                │
│  └── ResponseBuilders: 响应构建器                           │
├─────────────────────────────────────────────────────────────┤
│  服务层 (service)                                           │
│  ├── DbExecutor: 数据库执行器（线程池 + 连接池）             │
│  ├── DeviceRegistry: 设备注册表                             │
│  ├── AdminRegistry: 管理端注册表                            │
│  ├── TokenManager: JWT 令牌管理                             │
│  ├── MessageRouter: 消息路由器                              │
│  ├── EventSubscriptionRegistry: 事件订阅注册表              │
│  └── admin/: 管理端业务服务                                 │
│      ├── PersonService: 人员管理                            │
│      ├── AttendanceCrudService: 考勤管理                    │
│      ├── DeviceCrudService: 设备管理                        │
│      ├── UserService: 用户管理                              │
│      ├── RoleService: 角色权限管理                          │
│      ├── FaceCrudService: 人脸数据管理                      │
│      └── DeviceConfigService: 设备配置管理                  │
├─────────────────────────────────────────────────────────────┤
│  数据库层 (db)                                              │
│  ├── DbClient: 数据库客户端                                 │
│  ├── ConnectionPool: 连接池                                 │
│  ├── Schema: 数据库 Schema 管理                             │
│  └── *Repository: 数据仓库（CRUD 操作）                     │
│      ├── PersonRepository: 人员数据                         │
│      ├── AttendanceRecordRepository: 考勤记录               │
│      ├── DeviceRepository: 设备数据                         │
│      ├── UserAccountRepository: 用户账号                    │
│      ├── RoleRepository: 角色权限                           │
│      ├── FaceDataRepository: 人脸数据                       │
│      ├── DeviceConfigRepository: 设备配置                   │
│      └── OperationLogRepository: 操作日志                   │
├─────────────────────────────────────────────────────────────┤
│  工具层 (util)                                              │
│  ├── ArcFaceEngine: 虹软人脸识别引擎                        │
│  ├── JwtUtil: JWT 工具类                                    │
│  ├── PasswordHash: 密码哈希                                 │
│  ├── Base64: Base64 编解码                                  │
│  ├── SystemError: 系统错误格式化                            │
│  └── DebugLog: 调试日志                                     │
├─────────────────────────────────────────────────────────────┤
│  配置层 (config)                                            │
│  ├── ServerConfig: 服务器配置                               │
│  ├── ConfigFile: 配置文件加载                               │
│  └── ConsoleConfigUi: 控制台配置界面                        │
└─────────────────────────────────────────────────────────────┘
```

### 通信协议

系统采用基于 TCP 的自定义协议，支持两种帧类型：

1. **文本帧（JSON 行）**：
   - 以换行符（`\n`）分隔的 JSON 对象
   - 最大单行长度：1 MB
   - 用于业务消息传输

2. **二进制帧**：
   - 4 字节大端长度前缀 + 负载数据
   - 最大负载长度：16 MB
   - 用于照片等二进制数据传输

#### 消息信封格式

```json
{
  "type": "message.type",
  "role": "device|admin",
  "from": "sender_id",
  "to": "receiver_id",
  "msgId": "unique_message_id",
  "inReplyTo": "reply_to_message_id",
  "ts": 1234567890,
  "code": 0,
  "msg": "message",
  "data": {}
}
```

### 数据库设计

系统使用 MySQL 8.0+，通过 X Protocol（端口 33060）连接。

#### 核心表结构

1. **Person（人员表）**
   - id, name, employee_id, department, position
   - created_at, updated_at

2. **AttendanceRecord（考勤记录表）**
   - id, employee_id, check_time, device_id, status
   - photo (LONGBLOB), received_time

3. **AttendanceRecordArchive（考勤归档表）**
   - 归档历史记录，包含人员快照信息

4. **Device（设备表）**
   - id, device_id, device_name, ip_address
   - last_online, status

5. **UserAccount（用户账号表）**
   - id, employee_id, name, password
   - created_at, last_login_time

6. **Role（角色表）**
   - id, role_key, role_name, description
   - is_system, created_at, updated_at

7. **Permission（权限表）**
   - id, perm_key, perm_name, resource, description

8. **UserRole（用户角色关联表）**
   - user_id, role_key

9. **RolePermission（角色权限关联表）**
   - role_key, perm_key

10. **FaceData（人脸数据表）**
    - id, employee_id, feature_vector (LONGBLOB)
    - feature_size, created_at, updated_at

11. **DeviceConfigDeploy（设备配置部署表）**
    - id, config_version, config_content, config_hash
    - description, created_by, created_at

12. **DeviceConfigTarget（设备配置目标表）**
    - id, deploy_id, device_id, status
    - forward_msg_id, error_message, sent_at, acked_at

13. **OperationLog（操作日志表）**
    - id, user_id, employee_id, action, target_type
    - target_id, detail, ip_address, result, created_at

## 目录结构

```
attendanceServer/
├── main.cpp                          # 程序入口
├── CMakeLists.txt                    # CMake 构建配置
├── README.md                         # 项目说明文档
│
├── config/                           # 配置模块
│   ├── ServerConfig.hpp/cpp          # 服务器配置结构
│   ├── ConfigFile.hpp/cpp            # 配置文件加载
│   └── ConsoleConfigUi.hpp/cpp       # 控制台配置 UI
│
├── net/                              # 网络层
│   ├── TcpServer.hpp/cpp             # TCP 服务器
│   ├── Session.hpp/cpp               # TCP 会话
│   ├── TlsSession.hpp/cpp            # TLS 会话
│   ├── SessionBase.hpp               # 会话基类
│   ├── TlsConfig.hpp                 # TLS 配置
│   └── Framing.hpp/cpp               # 帧编解码
│
├── protocol/                         # 协议层
│   ├── GatewaySessionHandler.hpp/cpp # 网关会话处理器
│   ├── GatewayConfig.hpp             # 网关配置
│   ├── ConnContext.hpp               # 连接上下文
│   ├── LineCodec.hpp/cpp             # JSON 行编解码
│   ├── Envelope.hpp/cpp              # 消息信封
│   ├── ResponseBuilders.hpp/cpp      # 响应构建器
│   ├── DbErrorMap.hpp/cpp            # 数据库错误映射
│   ├── MessageHandlerRegistry.hpp    # 消息处理器注册表
│   ├── AppError.hpp                  # 应用错误定义
│   ├── ProtocolUtils.hpp             # 协议工具函数
│   └── handlers/                     # 消息处理器
│       ├── AttendancePhotoHandler.hpp
│       ├── ConfigDeployHandler.hpp
│       ├── CrudHandlerFactory.hpp
│       ├── DeviceCommandHandler.hpp
│       ├── FaceHandler.hpp
│       └── SubscribeHandler.hpp
│
├── service/                          # 服务层
│   ├── IDbExecutor.hpp               # 数据库执行器接口
│   ├── IDeviceRegistry.hpp           # 设备注册表接口
│   ├── DbExecutor.hpp/cpp            # 数据库执行器实现
│   ├── DeviceRegistry.hpp/cpp        # 设备注册表
│   ├── AdminRegistry.hpp/cpp         # 管理端注册表
│   ├── TokenManager.hpp/cpp          # JWT 令牌管理
│   ├── MessageRouter.hpp/cpp         # 消息路由器
│   ├── EventSubscriptionRegistry.hpp/cpp # 事件订阅注册表
│   ├── AttendanceService.hpp/cpp     # 考勤服务
│   ├── DeviceService.hpp/cpp         # 设备服务
│   ├── DeviceConfigService.hpp/cpp   # 设备配置服务
│   ├── FaceService.hpp/cpp           # 人脸服务
│   ├── SyncService.hpp/cpp           # 同步服务
│   └── admin/                        # 管理端服务
│       ├── AdminCrudService.hpp      # CRUD 服务聚合头文件
│       ├── PersonService.hpp/cpp     # 人员管理
│       ├── AttendanceCrudService.hpp/cpp # 考勤管理
│       ├── DeviceCrudService.hpp/cpp # 设备管理
│       ├── UserService.hpp/cpp       # 用户管理
│       ├── RoleService.hpp/cpp       # 角色权限管理
│       └── FaceCrudService.hpp/cpp   # 人脸数据管理
│
├── db/                               # 数据库层
│   ├── DbConfig.hpp                  # 数据库配置
│   ├── DbClient.hpp/cpp              # 数据库客户端
│   ├── ConnectionPool.hpp            # 连接池
│   ├── Schema.hpp/cpp                # Schema 管理
│   ├── PersonRepository.hpp/cpp      # 人员数据仓库
│   ├── AttendanceRecordRepository.hpp/cpp # 考勤记录仓库
│   ├── DeviceRepository.hpp/cpp      # 设备数据仓库
│   ├── UserAccountRepository.hpp/cpp # 用户账号仓库
│   ├── RoleRepository.hpp/cpp        # 角色权限仓库
│   ├── FaceDataRepository.hpp/cpp    # 人脸数据仓库
│   ├── DeviceConfigRepository.hpp/cpp # 设备配置仓库
│   └── OperationLogRepository.hpp/cpp # 操作日志仓库
│
├── util/                             # 工具层
│   ├── ArcFaceEngine.hpp/cpp         # 虹软人脸识别引擎
│   ├── JwtUtil.hpp/cpp               # JWT 工具类
│   ├── PasswordHash.hpp/cpp          # 密码哈希
│   ├── Base64.hpp                    # Base64 编解码
│   ├── SystemError.hpp/cpp           # 系统错误格式化
│   └── DebugLog.hpp                  # 调试日志
│
├── arcface/                          # 虹软 SDK
│   └── include/                      # 头文件
│       ├── arcsoft_face_sdk.h
│       ├── amcomdef.h
│       ├── asvloffscreen.h
│       └── merror.h
│
├── json-develop/                     # nlohmann/json 库
│   └── include/
│       └── nlohmann/
│           └── json.hpp
│
├── third_party/                      # 第三方库
│   └── jwt-cpp/                      # jwt-cpp 库
│
└── build/                            # 构建目录
```

## 模块说明

### 网络层 (net)

#### TcpServer
- 异步 TCP 服务器，基于 Boost.Asio
- 支持多线程 worker 处理并发连接
- 可配置绑定地址、端口、worker 线程数
- 支持 TLS 加密（可选）

#### Session / TlsSession
- TCP/TLS 会话管理
- 异步读写操作
- 帧解析（文本行 + 二进制帧）
- 心跳检测和超时处理

#### Framing
- 帧编解码工具
- 大端 32 位长度前缀（二进制帧）
- 文本行解析（LF 分隔）
- 缓冲区管理和溢出保护

### 协议层 (protocol)

#### GatewaySessionHandler
- 网关会话处理器，处理所有连接事件
- 消息路由和分发
- 认证流程处理（设备/管理员）
- 心跳管理
- 连接上下文管理

#### MessageHandlerRegistry
- 消息处理器注册表（模板类）
- 支持按消息类型注册处理器
- 动态消息路由

#### Envelope
- 消息信封解析
- 兼容旧版顶层字段合并
- JSON 格式验证

#### ResponseBuilders
- 标准响应构建器
- 认证响应、心跳响应、错误响应
- 业务响应（考勤上报确认等）

### 服务层 (service)

#### DbExecutor
- 数据库执行器，基于 Boost.Asio 线程池
- 异步任务分发和回调
- 连接池管理
- 支持健康检查和断线重连

#### DeviceRegistry
- 设备注册表，管理在线设备
- 设备会话映射（deviceId -> Session）
- 重复设备处理策略（踢下线/拒绝新连接）
- 线程安全

#### AdminRegistry
- 管理端注册表，管理在线管理员
- 支持多端同时登录
- 会话生命周期管理

#### TokenManager
- JWT 令牌管理器
- 设备认证和管理员认证
- 令牌生成、验证、刷新、撤销
- 令牌黑名单管理

#### MessageRouter
- 消息路由器，处理设备命令转发
- 管理端 -> 设备端命令转发
- ACK 超时处理
- 消息 ID 映射

#### EventSubscriptionRegistry
- 事件订阅注册表
- 主题订阅和发布
- 考勤事件实时推送

### 管理端服务 (service/admin)

#### PersonService
- 人员信息 CRUD 操作
- 支持分页查询
- 操作审计日志

#### AttendanceCrudService
- 考勤记录 CRUD 操作
- 考勤归档管理
- 支持多条件查询

#### DeviceCrudService
- 设备信息 CRUD 操作
- 设备状态管理
- 设备配置下发

#### UserService
- 用户账号 CRUD 操作
- 密码管理
- 用户角色分配

#### RoleService
- 角色 CRUD 操作
- 权限管理
- 角色权限分配

#### FaceCrudService
- 人脸数据管理
- 人脸注册（特征提取）
- 人脸查询和删除

### 数据库层 (db)

#### DbClient
- MySQL X Protocol 客户端
- 连接管理
- 连接 URL 构建

#### ConnectionPool
- 固定大小连接池
- 阻塞获取和归还
- 健康检查和自动重建
- 线程安全

#### Schema
- 数据库 Schema 管理
- 表创建（幂等）
- RBAC 种子数据初始化

#### *Repository
- 数据仓库模式
- 静态方法封装 CRUD 操作
- 类型安全的输入/输出结构体
- 错误处理和异常映射

### 工具层 (util)

#### ArcFaceEngine
- 虹软人脸识别引擎封装
- 单例模式
- 人脸特征提取
- 错误处理

#### JwtUtil
- JWT 令牌工具类
- HMAC-SHA256 签名
- Base64 URL 编解码
- 令牌生成和验证

#### PasswordHash
- SHA-256 + 随机盐密码哈希
- 密码验证
- 密码强度验证

#### Base64
- Base64 编解码
- 用于二进制数据传输

#### DebugLog
- 调试日志系统
- 多级别日志（Debug/Info/Warn/Error）
- 分类日志（NET/AUTH/PROTO/DB/SESSION/ATTENDANCE/EVENT）
- 线程安全
- 编译开关控制

## 配置说明

### 配置文件

配置文件格式为 JSON，示例文件：`attendance.json.example`

```json
{
  "db": {
    "host": "localhost",
    "port": 33060,
    "user": "root",
    "password": "root",
    "schema": "attendanceserver"
  },
  "enable_tcp": true,
  "tcp_bind": "0.0.0.0",
  "tcp_port": 8080,
  "tcp_worker_threads": 0,
  "db_pool_threads": 0,
  "gateway": {
    "heartbeat_sec": 30,
    "heartbeat_grace_multiplier": 3,
    "duplicate_policy": "KickOld",
    "device_keys": {
      "device001": "secret_key_1",
      "device002": "secret_key_2"
    },
    "default_device_key": "changeme",
    "jwt_secret": "your-jwt-secret-at-least-32-bytes",
    "access_token_ttl": 3600,
    "refresh_token_ttl": 604800,
    "clock_skew": 30
  },
  "arcface": {
    "enabled": false,
    "app_id": "your-arcface-app-id",
    "sdk_key": "your-arcface-sdk-key",
    "min_face_size": 80
  },
  "tls": {
    "enabled": false,
    "cert_file": "certs/server.crt",
    "key_file": "certs/server.key",
    "ca_file": "certs/ca.crt",
    "verify_client": false,
    "min_version": "TLSv1.2"
  }
}
```

### 配置优先级

1. 命令行参数（最高优先级）
2. 环境变量（`ATTENDANCE_*` 前缀）
3. 配置文件
4. 默认值（最低优先级）

### 环境变量

- `ATTENDANCE_DB_HOST`：数据库主机
- `ATTENDANCE_DB_PORT`：数据库端口
- `ATTENDANCE_DB_USER`：数据库用户
- `ATTENDANCE_DB_PASSWORD`：数据库密码
- `ATTENDANCE_DB_SCHEMA`：数据库 Schema
- `ATTENDANCE_TCP_BIND`：TCP 绑定地址
- `ATTENDANCE_TCP_PORT`：TCP 端口
- `ATTENDANCE_JWT_SECRET`：JWT 密钥
- `ATTENDANCE_ARCFACE_APP_ID`：虹软 App ID
- `ATTENDANCE_ARCFACE_SDK_KEY`：虹软 SDK Key

### 命令行参数

```bash
./attendanceServer --help                    # 显示帮助
./attendanceServer --config path/to/config   # 指定配置文件
./attendanceServer --no-arcface              # 禁用人脸识别
./attendanceServer --db-host localhost       # 数据库主机
./attendanceServer --db-port 33060           # 数据库端口
./attendanceServer --tcp-port 8080           # TCP 端口
```

## 构建与部署

### 环境要求

- **操作系统**：Windows 10/11, Linux (Ubuntu 20.04+)
- **编译器**：MSVC 2019+, GCC 9+, Clang 10+
- **CMake**：3.10+
- **Boost**：1.78+（静态链接）
- **MySQL**：8.0+（X Protocol 支持）
- **OpenSSL**：1.1.1+
- **虹软 SDK**：ArcFace SDK（可选）

### 构建步骤

#### Windows

```bash
# 1. 克隆项目
git clone <repository-url>
cd attendanceServer

# 2. 创建构建目录
mkdir build
cd build

# 3. 配置 CMake
cmake .. -G "Visual Studio 16 2019" -A x64

# 4. 编译
cmake --build . --config Release

# 5. 运行
./Release/attendanceServer.exe
```

#### Linux

```bash
# 1. 克隆项目
git clone <repository-url>
cd attendanceServer

# 2. 安装依赖
sudo apt-get install libboost-all-dev libssl-dev libmysqlcppconn-dev

# 3. 创建构建目录
mkdir build
cd build

# 4. 配置 CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 5. 编译
make -j$(nproc)

# 6. 运行
./attendanceServer
```

### 数据库准备

1. 安装 MySQL 8.0+，确保 X Protocol 已启用（默认端口 33060）

2. 创建数据库和用户：

```sql
CREATE DATABASE attendanceserver CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'attendance'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON attendanceserver.* TO 'attendance'@'localhost';
FLUSH PRIVILEGES;
```

3. 启动服务器，系统会自动创建表结构和初始化 RBAC 数据

### TLS 配置（可选）

1. 生成证书：

```bash
# 生成 CA 证书
openssl req -x509 -newkey rsa:4096 -keyout ca.key -out ca.crt -days 365 -nodes

# 生成服务器证书
openssl req -newkey rsa:4096 -keyout server.key -out server.csr -nodes
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 365
```

2. 配置 TLS：

```json
{
  "tls": {
    "enabled": true,
    "cert_file": "certs/server.crt",
    "key_file": "certs/server.key",
    "ca_file": "certs/ca.crt",
    "verify_client": false,
    "min_version": "TLSv1.2"
  }
}
```

## API 参考

### 设备端消息类型

#### 认证相关
- `device.auth`：设备认证请求
- `device.auth.response`：认证响应

#### 考勤相关
- `attendance.report`：考勤上报
- `attendance.photo`：考勤照片（二进制帧）

#### 心跳相关
- `heartbeat`：心跳请求
- `heartbeat.response`：心跳响应

#### 配置相关
- `config.sync`：配置同步请求
- `config.deploy`：配置下发

#### 同步相关
- `sync.request`：数据同步请求
- `sync.ack`：同步确认

### 管理端消息类型

#### 认证相关
- `admin.auth`：管理员登录
- `admin.auth.response`：登录响应
- `token.refresh`：令牌刷新

#### 人员管理
- `person.create`：创建人员
- `person.query`：查询人员
- `person.update`：更新人员
- `person.delete`：删除人员

#### 考勤管理
- `attendance.create`：创建考勤记录
- `attendance.query`：查询考勤记录
- `attendance.update`：更新考勤记录
- `attendance.delete`：删除考勤记录
- `attendance.archive.query`：查询归档记录
- `attendance.archive.delete`：删除归档记录

#### 设备管理
- `device.create`：创建设备
- `device.query`：查询设备
- `device.update`：更新设备
- `device.delete`：删除设备
- `device.command`：设备命令下发

#### 用户管理
- `user.create`：创建用户
- `user.query`：查询用户
- `user.update`：更新用户
- `user.delete`：删除用户

#### 角色权限
- `role.create`：创建角色
- `role.query`：查询角色
- `role.update`：更新角色
- `role.delete`：删除角色
- `user.role.assign`：分配用户角色
- `user.role.revoke`：撤销用户角色
- `user.role.query`：查询用户角色
- `permission.query`：查询权限
- `permission.self`：查询自身权限

#### 人脸管理
- `face.query`：查询人脸数据
- `face.delete`：删除人脸数据
- `face.register`：注册人脸

#### 事件订阅
- `subscribe`：订阅事件
- `unsubscribe`：取消订阅

#### 设备配置
- `config.deploy`：部署设备配置

## 错误码参考

### 协议错误码

| 错误码 | 说明 |
|--------|------|
| 1001 | 解析错误 |
| 1002 | 负载过大 |
| 2001 | 未认证 |
| 2002 | 认证失败 |
| 2003 | 重复会话 |
| 2004 | 令牌无效 |
| 2005 | 令牌过期 |
| 2006 | 刷新令牌无效 |
| 2007 | 权限不足 |
| 3001 | 禁止访问 |
| 4000 | 业务验证失败 |
| 4001 | 员工不存在 |
| 5001 | 设备离线 |
| 5002 | 转发超时 |
| 6001 | 重复键 |
| 6002 | 数据库错误 |

## 性能优化

### 连接池配置

- `db_pool_threads`：数据库线程池大小，默认为 CPU 核心数
- `tcp_worker_threads`：TCP worker 线程数，默认为 CPU 核心数

### 内存管理

- 使用 `shared_ptr` 和 `weak_ptr` 管理会话生命周期
- 连接池自动回收无效连接
- 消息处理器注册表避免内存泄漏

### 并发处理

- Boost.Asio 异步 I/O 模型
- 线程池处理数据库操作
- 会话级 strand 保证顺序执行

## 监控与日志

### 日志级别

- **Debug**：调试信息（开发环境）
- **Info**：一般信息
- **Warn**：警告信息
- **Error**：错误信息

### 日志分类

- **NET**：网络层日志
- **AUTH**：认证日志
- **PROTO**：协议层日志
- **DB**：数据库日志
- **SESSION**：会话日志
- **ATTENDANCE**：考勤业务日志
- **EVENT**：事件日志

### 编译开关

```cpp
// 禁用调试日志（零开销）
#define ATTENDANCE_DEBUG_LOG 0
```

## 常见问题

### Q: 如何重置管理员密码？

A: 直接修改数据库中的 `UserAccount` 表，使用 `hash_password` 工具生成新密码哈希。

### Q: 如何添加新设备？

A: 在配置文件的 `gateway.device_keys` 中添加设备 ID 和密钥，或通过管理端 API 创建设备。

### Q: 如何启用人脸识别？

A: 在配置文件中设置 `arcface.enabled: true`，并配置正确的 `app_id` 和 `sdk_key`。

### Q: 如何配置 TLS？

A: 参考 TLS 配置章节，生成证书并在配置文件中启用 TLS。

### Q: 数据库连接失败怎么办？

A: 检查 MySQL 服务是否启动，X Protocol 是否启用，连接参数是否正确。

## 许可证

本项目为私有项目，版权所有。

## 联系方式

如有问题或建议，请联系项目维护者。
