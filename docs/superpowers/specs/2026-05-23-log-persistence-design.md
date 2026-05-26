# 日志记录与持久化功能设计

## 概述

为考勤管理端添加日志记录与持久化功能，记录重要操作并提供日志查看页面。

## 需求

### 功能需求

1. **日志记录类型**
   - 用户登录/登出
   - 数据增删改
   - 系统配置变更
   - 错误和异常

2. **日志存储**
   - 所有类型日志合并存储，按时间顺序显示
   - 纯文本格式
   - 按日期轮转（每天一个文件）
   - 保留 30 天
   - 存储位置：程序目录/logs

3. **日志查看页面**
   - 布局：经典表格布局
   - 筛选功能：日期范围、级别、类型、关键词搜索
   - 导出功能：支持导出为文本文件
   - 分页：无限滚动
   - 权限：仅超级管理员可查看

### 非功能需求

- 性能：日志写入不应影响主业务流程
- 可靠性：日志写入失败不应导致程序崩溃
- 可维护性：代码结构清晰，职责分离

## 架构设计

```
┌─────────────────────────────────────────────────────────┐
│                      QML UI 层                          │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │
│  │ LogViewer   │  │ LogFilters  │  │ LogExporter │     │
│  │ (展示列表)   │  │ (筛选控件)   │  │ (导出功能)   │     │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘     │
│         └────────────────┼────────────────┘             │
│                          │                              │
│                    ┌─────▼─────┐                        │
│                    │ LogReader │                        │
│                    │ (读取文件) │                        │
│                    └─────┬─────┘                        │
└──────────────────────────┼──────────────────────────────┘
                           │
                    ┌──────▼──────┐
                    │ FileService │ (C++ 服务)
                    │ 文件读写服务 │
                    └──────┬──────┘
                           │
┌──────────────────────────┼──────────────────────────────┐
│                    ┌─────▼─────┐                        │
│                    │LogWriter  │                        │
│                    │ (写入文件) │                        │
│                    └─────┬─────┘                        │
│                          │                              │
│  ┌─────────────┐  ┌──────┴──────┐  ┌─────────────┐     │
│  │ qtMessage   │  │ Operation   │  │ Error       │     │
│  │ Handler     │  │ Logger      │  │ Logger      │     │
│  │ (Qt消息)    │  │ (业务操作)   │  │ (错误异常)   │     │
│  └─────────────┘  └─────────────┘  └─────────────┘     │
│                      C++ 层                             │
└─────────────────────────────────────────────────────────┘
```

## 数据模型

### 日志文件结构

```
logs/
├── app-2026-05-23.log
├── app-2026-05-24.log
└── ...
```

### 日志条目格式

```
[2026-05-23 14:30:45] [INFO] [LOGIN] 用户登录 | 用户=admin | IP=192.168.1.100 | 结果=成功
[2026-05-23 14:35:22] [INFO] [DATA] 删除人员 | 用户=admin | 目标=工号1001 | 结果=成功
[2026-05-23 14:35:22] [ERROR] [DATA] 删除人员失败 | 用户=admin | 目标=工号1001 | 详情=网络超时
[2026-05-23 14:40:18] [WARN] [CONFIG] 修改考勤规则 | 用户=admin | 详情=迟到阈值30→15分钟 | 结果=成功
[2026-05-23 15:00:00] [ERROR] [SYSTEM] 数据库连接异常 | 详情=连接池耗尽
```

### 字段说明

- 时间戳：`[YYYY-MM-DD HH:mm:ss]`
- 级别：`[INFO/WARN/ERROR]`
- 类型：`[LOGIN/DATA/CONFIG/SYSTEM]`
- 消息：操作描述
- 附加字段：`key=value` 格式，用 `|` 分隔

## 详细设计

### 1. LogWriter (C++)

负责日志写入，处理日志轮转和清理。

```cpp
class LogWriter : public QObject {
    Q_OBJECT
public:
    explicit LogWriter(QObject *parent = nullptr);
    
    // 写入业务操作日志
    Q_INVOKABLE void logOperation(const QString &level, const QString &type, 
                                   const QString &message, const QVariantMap &details = {});
    
    // 写入错误日志
    Q_INVOKABLE void logError(const QString &type, const QString &message, 
                               const QVariantMap &details = {});
    
private:
    void writeLog(const QString &level, const QString &type, 
                  const QString &message, const QVariantMap &details);
    QString getLogFilePath();
    void rotateLogs();
    void cleanOldLogs();
    
    QDir m_logDir;
    QMutex m_mutex;
};
```

### 2. FileService (C++)

提供文件读取接口给 QML。

```cpp
class FileService : public QObject {
    Q_OBJECT
public:
    explicit FileService(QObject *parent = nullptr);
    
    // 读取日志文件内容
    Q_INVOKABLE QString readLogFile(const QString &date);
    
    // 获取可用日志文件列表
    Q_INVOKABLE QStringList getLogFileDates();
    
    // 导出筛选后的日志
    Q_INVOKABLE bool exportLogs(const QString &content, const QString &filePath);
    
private:
    QDir m_logDir;
};
```

### 3. LogReader (QML)

封装文件读取逻辑，支持筛选。

```qml
QtObject {
    id: root
    
    property string currentDate: Qt.formatDate(new Date(), "yyyy-MM-dd")
    property string filterLevel: ""
    property string filterType: ""
    property string filterKeyword: ""
    property date filterDateStart
    property date filterDateEnd
    
    property var logEntries: []
    property int currentPage: 0
    property int pageSize: 50
    
    function loadLogs() { ... }
    function loadMore() { ... }
    function applyFilters() { ... }
    function exportLogs(filePath) { ... }
}
```

### 4. LogViewer (QML)

展示日志列表，支持无限滚动。

```qml
Rectangle {
    // 筛选栏
    // 日志表格
    // 无限滚动
}
```

### 5. 导航栏配置

在 `PermissionCatalog.qml` 中添加：

```qml
readonly property var allNavItems: [
    // ... 现有导航项 ...
    
    { group: qsTr("其它") },
    { key: "logViewer", label: qsTr("日志查看"), icon: "📋" },
]

function canAccessNav(key, sessionManager) {
    // ... 现有权限检查 ...
    case "logViewer":
        return hasRoleKey(sessionManager, "super_admin")
}
```

### 6. 日志写入触发点

在以下位置调用 LogWriter：

1. **SessionManager** - 用户登录/登出
2. **PersonServer** - 人员增删改
3. **DeviceServer** - 设备增删改
4. **ConfigDeployServer** - 配置变更
5. **AttendanceService** - 考勤操作
6. **FaceServer** - 人脸操作
7. **UserServer** - 用户操作
8. **RbacServer** - 权限操作
9. **qtMessageHandler** - WARN/ERROR/FATAL 消息

## UI 设计

### 页面布局

```
┌─────────────────────────────────────────────────────┐
│                   日志查看                           │
│                   共 1,234 条记录                    │
├─────────────────────────────────────────────────────┤
│  日期范围: [____] 至 [____]  级别: [全部▼]          │
│  类型: [全部▼]  关键词: [____]  [搜索] [导出]       │
├─────────────────────────────────────────────────────┤
│  时间            级别   类型    详情                 │
│  2026-05-23      INFO   LOGIN  用户登录 | admin...  │
│  14:30:45                                         │
│  ─────────────────────────────────────────────────  │
│  2026-05-23      INFO   DATA   删除人员 | admin...  │
│  14:35:22                                         │
│  ─────────────────────────────────────────────────  │
│  2026-05-23      ERROR  DATA   删除人员失败 | ...   │
│  14:35:22         (红色高亮)                        │
│  ─────────────────────────────────────────────────  │
│  ... (无限滚动加载更多)                             │
└─────────────────────────────────────────────────────┘
```

### 颜色方案

- INFO：蓝色 (#1890ff)
- WARN：黄色 (#faad14)
- ERROR：红色 (#ff4d4f)，行背景为浅红色 (#fff1f0)
- 类型标签：灰色 (#f0f0f0)

## 文件清单

### 新增文件

1. `src/Log/LogWriter.h` - 日志写入类头文件
2. `src/Log/LogWriter.cpp` - 日志写入类实现
3. `src/Log/FileService.h` - 文件服务类头文件
4. `src/Log/FileService.cpp` - 文件服务类实现
5. `ui/pages/PageLogViewer.qml` - 日志查看页面
6. `ui/models/LogReader.qml` - 日志读取模型

### 修改文件

1. `main.cpp` - 注册 LogWriter 和 FileService
2. `ui/models/PermissionCatalog.qml` - 添加日志查看导航项和权限
3. `ui/components/PageHost.qml` - 添加日志查看页面路由
4. `CMakeLists.txt` - 添加新源文件

## 测试计划

1. **单元测试**
   - LogWriter 日志写入
   - LogWriter 日志轮转
   - LogWriter 过期日志清理
   - FileService 文件读取
   - FileService 日期列表获取

2. **集成测试**
   - 登录/登出日志记录
   - 数据操作日志记录
   - 配置变更日志记录
   - 错误异常日志记录

3. **UI 测试**
   - 日志列表展示
   - 筛选功能
   - 无限滚动
   - 导出功能
   - 权限控制

## 风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 日志写入影响性能 | 高 | 使用异步写入，队列缓冲 |
| 日志文件过大 | 中 | 按日期轮转，保留30天 |
| 并发写入冲突 | 中 | 使用互斥锁保护 |
| 磁盘空间不足 | 低 | 定期清理过期日志 |
