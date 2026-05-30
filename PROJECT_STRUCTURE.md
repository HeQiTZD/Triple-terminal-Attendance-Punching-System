# AttendanceAdmin 项目结构文档

## 目录结构总览

```
AttendanceServer/
├── CMakeLists.txt                    # CMake 构建配置
├── main.cpp                          # 应用程序入口
├── README.md                         # 项目说明文档
├── PROJECT_STRUCTURE.md              # 项目结构文档（本文件）
│
├── src/                              # 源代码目录
│   ├── Models/                       # 数据模型层
│   ├── Protocol/                     # 通信协议定义
│   ├── Network/                      # 网络通信层
│   ├── Auth/                         # 认证与会话管理
│   ├── Person/                       # 人员管理服务
│   ├── Attendance/                   # 考勤管理服务
│   ├── Device/                       # 设备管理服务
│   ├── Face/                         # 人脸识别服务
│   ├── User/                         # 用户管理服务
│   ├── Rbac/                         # 权限管理服务
│   ├── Config/                       # 配置部署服务
│   ├── Event/                        # 事件订阅服务
│   └── Log/                          # 日志服务
│
└── ui/                               # QML 用户界面
    ├── Main.qml                      # 主窗口
    ├── LoginPage.qml                 # 登录页面
    ├── theme/                        # 主题配置
    ├── models/                       # QML 数据模型
    ├── components/                   # 可复用组件
    └── pages/                        # 功能页面
```

## 详细结构说明

### 1. 根目录文件

#### CMakeLists.txt
CMake 构建配置文件，定义了：
- 项目名称和版本（AttendanceAdmin v0.1）
- Qt 6 依赖组件（Quick, Core, Gui, Network, Widgets, Charts）
- QML 模块注册
- 源文件列表
- 安装配置

#### main.cpp
应用程序入口文件，负责：
- 初始化 QApplication
- 创建并注册所有服务类
- 配置 QML 引擎
- 设置 Qt 消息处理器
- 启动事件循环

---

### 2. src/ 源代码目录

#### 2.1 Models/ - 数据模型层

数据模型定义，用于在 C++ 和 QML 之间传递结构化数据。

| 文件 | 说明 |
|------|------|
| `Person.h/cpp` | 人员数据模型 |
| `attendancerecord.h/cpp` | 考勤记录模型 |
| `device.h/cpp` | 设备数据模型 |
| `faceData.h/cpp` | 人脸数据模型 |

**Person 模型属性：**
- `id` - 记录ID
- `name` - 姓名
- `employeeId` - 工号
- `department` - 部门
- `position` - 职位
- `createdAt` - 创建时间
- `updatedAt` - 更新时间

---

#### 2.2 Protocol/ - 通信协议定义

| 文件 | 说明 |
|------|------|
| `protocol.h` | 协议常量和错误码定义 |

**主要定义内容：**

**消息信封字段：**
- `type` - 消息类型
- `role` - 角色（admin/server）
- `from` - 发送方标识
- `to` - 接收方标识
- `msgId` - 消息ID
- `inReplyTo` - 回复消息ID
- `ts` - 时间戳
- `code` - 响应码
- `msg` - 响应消息
- `data` - 数据载荷

**认证相关：**
- `auth` / `auth_response` - 认证请求/响应
- `token.refresh` / `token.refresh.response` - Token 刷新
- `sessionToken` / `accessToken` / `refreshToken` - 令牌类型

**业务消息类型：**
- `person.*` - 人员管理
- `device.*` - 设备管理
- `attendance.*` - 考勤管理
- `face.*` - 人脸识别
- `user.*` - 用户管理
- `role.*` / `permission.*` - 权限管理
- `config.deploy` - 配置部署
- `subscribe` / `unsubscribe` - 事件订阅

**错误码（ErrorCode 命名空间）：**
```cpp
kSuccess               = 0     // 成功
kParseError            = 1001  // 解析错误
kPayloadTooLarge       = 1002  // 数据过大
kNotAuthenticated      = 2001  // 未认证
kAuthFailed            = 2002  // 认证失败
kDuplicateSession      = 2003  // 重复会话
kTokenInvalid          = 2004  // Token 无效
kTokenExpired          = 2005  // Token 过期
kRefreshTokenInvalid   = 2006  // Refresh Token 无效
kInsufficientPermissions = 2007 // 权限不足
kForbidden             = 3001  // 禁止访问
kBusinessValidation    = 4000  // 业务验证失败
kEmployeeNotFound      = 4001  // 员工不存在
kDeviceOffline         = 5001  // 设备离线
kForwardTimeout        = 5002  // 转发超时
kDuplicateKey          = 6001  // 重复键
kDbError               = 6002  // 数据库错误
```

---

#### 2.3 Network/ - 网络通信层

| 文件 | 说明 |
|------|------|
| `TcpConnectionManager.h/cpp` | TCP 连接管理器 |

**TcpConnectionManager 类职责：**
- TCP 连接的建立和断开
- JSON 消息的发送和接收
- 心跳保活机制
- 自动重连逻辑
- 请求超时处理
- Token 自动刷新
- 二进制帧传输

**连接状态枚举（ConnectionState）：**
- `Disconnected` - 已断开
- `Connecting` - 连接中
- `Connected` - 已连接
- `Authenticated` - 已认证

**连接配置（ConnectionConfig）：**
```cpp
struct ConnectionConfig {
    QString host = "127.0.0.1";
    quint16 port = 9527;
    QString clientId = "admin_001";
    QString username;
    QString password;
    int heartbeatSec = 30;
    int reconnectMaxAttempts = 10;
    int reconnectBaseDelayMs = 1000;
    int requestTimeoutMs = 10000;
};
```

**主要信号：**
- `stateChanged` - 连接状态变化
- `authenticated` - 认证成功
- `authFailed` - 认证失败
- `messageReceived` - 消息接收
- `tokenRefreshed` - Token 刷新成功
- `errorOccurred` - 错误发生

---

#### 2.4 Auth/ - 认证与会话管理

| 文件 | 说明 |
|------|------|
| `SessionManager.h/cpp` | 会话管理器 |

**SessionManager 类职责：**
- 用户登录/登出
- 会话状态维护
- 权限查询和验证
- Token 管理

**QML 属性：**
- `isLoggedIn` - 是否已登录
- `sessionToken` - 会话令牌
- `accessToken` - 访问令牌
- `refreshToken` - 刷新令牌
- `roles` - 角色列表
- `permissions` - 权限列表
- `currentUsername` - 当前用户名
- `connectionState` - 连接状态

**QML 方法：**
- `login(host, port, username, password, clientId)` - 登录
- `logout()` - 登出
- `hasPermission(permKey)` - 检查权限
- `hasRole(roleKey)` - 检查角色
- `refreshPermissions()` - 刷新权限

---

#### 2.5 Person/ - 人员管理服务

| 文件 | 说明 |
|------|------|
| `PersonServer.h/cpp` | 人员管理服务 |

**功能：**
- 员工信息的 CRUD 操作
- 多条件查询筛选

**QML 方法：**
- `createPerson(name, employeeId, department, position)` - 创建人员
- `queryPersons(name, employeeId, department, position, createdAt, updatedAt)` - 查询人员
- `updatePerson(employeeId, name, department, position)` - 更新人员
- `deletePerson(employeeId)` - 删除人员

---

#### 2.6 Attendance/ - 考勤管理服务

| 文件 | 说明 |
|------|------|
| `AttendanceService.h/cpp` | 考勤管理服务 |

**功能：**
- 考勤记录查询与管理
- 考勤归档管理
- 数据导出

**QML 方法：**
- `query(idFilter, employeeId, checkTime, deviceId, status, receivedTime)` - 查询考勤
- `createRecord(employeeId, checkTime, status, deviceId)` - 创建记录
- `updateRecord(locate, updates)` - 更新记录
- `deleteRecord(criteria)` - 删除记录
- `queryArchive(...)` - 查询归档
- `deleteArchive(employeeId)` - 删除归档
- `exportToFile(fileUrl, content)` - 导出文件

---

#### 2.7 Device/ - 设备管理服务

| 文件 | 说明 |
|------|------|
| `DeviceServer.h/cpp` | 设备管理服务 |

**功能：**
- 考勤设备的 CRUD 操作
- 设备状态监控
- 设备命令下发
- 设备状态推送处理

**QML 方法：**
- `createDevice(deviceId, deviceName, deviceKey, ipAddress, status)` - 创建设备
- `queryDevices(deviceId, deviceName, ipAddress)` - 查询设备
- `updateDevice(deviceId, deviceName, ipAddress, status)` - 更新设备
- `deleteDevice(deviceId)` - 删除设备
- `sendCommand(deviceId, command, paramsJson)` - 发送命令

**信号：**
- `deviceStatusChanged(deviceId, status, ipAddress)` - 设备状态变化

---

#### 2.8 Face/ - 人脸识别服务

| 文件 | 说明 |
|------|------|
| `FaceServer.h/cpp` | 人脸识别服务 |

**功能：**
- 人脸数据注册与更新
- 人脸信息查询
- 人脸特征删除
- 支持从本地图片文件注册

**QML 方法：**
- `queryFace(employeeId)` - 查询人脸数据
- `deleteFace(employeeId)` - 删除人脸数据
- `registerFace(employeeId, photoBase64, overwrite)` - 注册人脸
- `registerFaceFromFile(employeeId, filePathOrUrl, overwrite)` - 从文件注册
- `clearRecords()` - 清空记录

**QML 属性：**
- `records` - 人脸记录列表
- `lastFound` - 最后查询是否找到
- `lastRecord` - 最后查询的记录

---

#### 2.9 User/ - 用户管理服务

| 文件 | 说明 |
|------|------|
| `UserServer.h/cpp` | 用户管理服务 |

**功能：**
- 系统用户账号管理
- 用户 CRUD 操作

**QML 方法：**
- `createUser(employeeId, password, name)` - 创建用户
- `queryUsers(employeeId, createdAt)` - 查询用户
- `updateUser(employeeId, name, password)` - 更新用户
- `deleteUser(employeeId)` - 删除用户

---

#### 2.10 Rbac/ - 权限管理服务

| 文件 | 说明 |
|------|------|
| `RbacServer.h/cpp` | 基于角色的访问控制服务 |

**功能：**
- 角色管理
- 权限管理
- 用户角色分配

**QML 方法：**
- `queryRoles()` - 查询所有角色
- `queryPermissions()` - 查询所有权限
- `querySelfPermissions()` - 查询当前用户权限
- `queryUserRoles(userId)` - 查询用户角色
- `createRole(roleKey, roleName, description)` - 创建角色
- `updateRole(roleKey, fields)` - 更新角色
- `deleteRole(roleKey)` - 删除角色
- `assignUserRole(userId, roleKey)` - 分配用户角色
- `revokeUserRole(userId, roleKey)` - 撤销用户角色

**QML 属性：**
- `roleRecords` - 角色记录列表
- `permissionRecords` - 权限记录列表
- `userRoleKeys` - 用户角色键列表

---

#### 2.11 Config/ - 配置部署服务

| 文件 | 说明 |
|------|------|
| `ConfigDeployServer.h/cpp` | 配置部署服务 |

**功能：**
- 设备配置批量部署
- 配置版本管理
- 部署状态追踪

**QML 方法：**
- `deployConfig(targetDevices, configContent, configVersion, description)` - 部署配置

**QML 属性：**
- `deployTargets` - 部署目标列表
- `lastDeployId` - 最后部署ID
- `configVersion` - 配置版本
- `configHash` - 配置哈希

---

#### 2.12 Event/ - 事件订阅服务

| 文件 | 说明 |
|------|------|
| `EventSubscriptionService.h/cpp` | 事件订阅服务 |

**功能：**
- 设备状态实时推送
- 考勤记录实时推送
- 事件主题订阅管理

**信号：**
- `serverPushReceived(messageType, data)` - 服务端推送接收

---

#### 2.13 Log/ - 日志服务

| 文件 | 说明 |
|------|------|
| `LogWriter.h/cpp` | 日志写入器 |
| `FileService.h/cpp` | 文件服务 |

**功能：**
- 系统日志记录
- 日志文件管理
- 错误日志分级

---

### 3. ui/ - QML 用户界面

#### 3.1 Main.qml
主窗口文件，定义应用程序主界面布局。

#### 3.2 LoginPage.qml
登录页面，提供用户认证界面。

#### 3.3 theme/
主题配置目录，包含全局样式定义。

#### 3.4 models/
QML 数据模型，提供前端数据管理。

| 文件 | 说明 |
|------|------|
| `Logger.qml` | 日志模型 |
| `History.qml` | 历史记录模型 |
| `Presets.qml` | 预设配置模型 |
| `ErrorCatalog.qml` | 错误目录模型 |
| `PermissionCatalog.qml` | 权限目录模型 |
| `PushFeed.qml` | 推送订阅模型 |
| `LogReader.qml` | 日志读取模型 |

#### 3.5 components/
可复用的 UI 组件。

| 文件 | 说明 |
|------|------|
| `Card.qml` | 卡片组件 |
| `BusyOverlay.qml` | 忙碌遮罩 |
| `SideBar.qml` | 侧边栏 |
| `StatusBar.qml` | 状态栏 |
| `BadgeStatus.qml` | 状态徽章 |
| `LabeledField.qml` | 标签字段 |
| `ToolBarRow.qml` | 工具栏行 |
| `JsonEditor.qml` | JSON 编辑器 |
| `JsonViewer.qml` | JSON 查看器 |
| `DataTable.qml` | 数据表格 |
| `LogPanel.qml` | 日志面板 |
| `ConfirmDialog.qml` | 确认对话框 |
| `LoginServerSettingsDialog.qml` | 登录设置对话框 |
| `WindowCaptionButtons.qml` | 窗口标题栏按钮 |
| `PermissionButton.qml` | 权限按钮 |
| `PermissionDeniedDialog.qml` | 权限不足对话框 |
| `PermissionChipRow.qml` | 权限标签行 |
| `PageAccessPlaceholder.qml` | 页面访问占位符 |
| `AttendanceStatusPieChart.qml` | 考勤状态饼图 |
| `RecentPunchList.qml` | 最近打卡列表 |
| `BarChart.qml` | 柱状图 |
| `ChartLegend.qml` | 图例 |
| `ChartContainer.qml` | 图表容器 |
| `StatCard.qml` | 统计卡片 |
| `PageHost.qml` | 页面宿主 |
| `FaceSyncButton.qml` | 人脸同步按钮 |

#### 3.6 pages/
功能页面。

| 文件 | 说明 |
|------|------|
| `PageDashboard.qml` | 仪表盘页面 |
| `PagePerson.qml` | 人员管理页面 |
| `PageDevice.qml` | 设备管理页面 |
| `PageConfigDeploy.qml` | 配置部署页面 |
| `PageAttendance.qml` | 考勤管理页面 |
| `PageFace.qml` | 人脸识别页面 |
| `PageUser.qml` | 用户管理页面 |
| `PageRbac.qml` | 权限管理页面 |
| `PageLogViewer.qml` | 日志查看页面 |

---

## 数据流架构

### 请求流程

```
QML UI
  │
  ▼
Service (PersonServer, DeviceServer, ...)
  │
  ▼
TcpConnectionManager.sendMessage()
  │
  ▼
QTcpSocket.write()
  │
  ▼
Server
```

### 响应流程

```
Server
  │
  ▼
QTcpSocket.readyRead
  │
  ▼
TcpConnectionManager.processJsonLine()
  │
  ▼
ResponseCallback (Lambda)
  │
  ▼
Service 更新 Q_PROPERTY
  │
  ▼
QML UI 自动刷新
```

### 推送流程

```
Server (Push)
  │
  ▼
TcpConnectionManager.processReceivedMessage()
  │
  ▼
EventSubscriptionService.serverPushReceived
  │
  ▼
DeviceServer.handleDeviceStatusPush()
  │
  ▼
QML UI 更新
```

---

## 编译依赖

### Qt 模块

| 模块 | 用途 |
|------|------|
| Qt6::Quick | QML 引擎和 UI 渲染 |
| Qt6::Core | 核心非 GUI 功能 |
| Qt6::Gui | 图形用户界面基础 |
| Qt6::Network | TCP 网络通信 |
| Qt6::Widgets | 窗口部件（QApplication） |
| Qt6::Charts | 图表展示组件 |

### 系统要求

- **操作系统**: Windows 10/11
- **编译器**: MSVC 2019+ 或 MinGW 8+
- **CMake**: 3.16+
- **Qt**: 6.8+

---

## 扩展指南

### 添加新的服务模块

1. **创建目录和文件**
   ```
   src/NewService/
   ├── NewService.h
   └── NewService.cpp
   ```

2. **实现服务类**
   ```cpp
   class NewService : public QObject {
       Q_OBJECT
       Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
   public:
       explicit NewService(QObject *parent = nullptr);
       void setTcpManager(TcpConnectionManager *tcp);
       Q_INVOKABLE void doSomething();
   signals:
       void busyChanged();
       void operationSucceeded(const QString &apiType, const QString &message);
       void operationFailed(const QString &apiType, int code, const QString &message);
   private:
       TcpConnectionManager *m_tcp = nullptr;
       bool m_busy = false;
   };
   ```

3. **在 main.cpp 中注册**
   ```cpp
   auto *newService = new NewService(&app);
   newService->setTcpManager(tcpManager);
   engine.setInitialProperties({
       { "newService", QVariant::fromValue(newService) }
   });
   ```

4. **更新 CMakeLists.txt**
   ```cmake
   SOURCES
       src/NewService/NewService.h
       src/NewService/NewService.cpp
   ```

5. **在 Protocol 中添加消息类型**
   ```cpp
   inline const QString kNewServiceAction = QStringLiteral("new_service.action");
   ```

### 添加新的 QML 页面

1. **创建页面文件**
   ```
   ui/pages/PageNewFeature.qml
   ```

2. **在 CMakeLists.txt 中注册**
   ```cmake
   QML_FILES
       ui/pages/PageNewFeature.qml
   ```

3. **在 Main.qml 中添加导航**
   ```qml
   // 在 SideBar 中添加菜单项
   // 在 PageHost 中添加页面路由
   ```

---

## 版本历史

### v0.1 (当前版本)
- 初始版本发布
- 实现核心功能模块
- 完成基础 UI 框架

---

## 许可证

[待定]

## 维护者

[待定]
