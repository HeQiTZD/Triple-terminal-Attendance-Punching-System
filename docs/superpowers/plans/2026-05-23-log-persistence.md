# 日志记录与持久化功能实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 为考勤管理端添加日志记录与持久化功能，记录重要操作并提供日志查看页面。

**Architecture:** 混合方案 - C++ 层负责日志写入（LogWriter、FileService），QML 层负责读取和展示（LogReader、PageLogViewer）。所有日志合并存储为纯文本文件，按日期轮转，保留30天。

**Tech Stack:** C++17, Qt 6.8, QML, QFile, QDir, QMutex

---

## 文件结构

### 新增文件

| 文件路径 | 职责 |
|----------|------|
| `src/Log/LogWriter.h` | 日志写入类头文件 |
| `src/Log/LogWriter.cpp` | 日志写入类实现 |
| `src/Log/FileService.h` | 文件服务类头文件 |
| `src/Log/FileService.cpp` | 文件服务类实现 |
| `ui/models/LogReader.qml` | 日志读取模型（单例） |
| `ui/pages/PageLogViewer.qml` | 日志查看页面 |

### 修改文件

| 文件路径 | 修改内容 |
|----------|----------|
| `main.cpp` | 注册 LogWriter 和 FileService |
| `ui/models/PermissionCatalog.qml` | 添加日志查看导航项和权限 |
| `ui/components/PageHost.qml` | 添加日志查看页面路由 |
| `CMakeLists.txt` | 添加新源文件 |

---

### Task 1: 创建 LogWriter 头文件

**Files:**
- Create: `src/Log/LogWriter.h`

- [ ] **Step 1: 创建 LogWriter.h**

```cpp
#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDir>
#include <QMutex>

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

- [ ] **Step 2: Commit**

```bash
git add src/Log/LogWriter.h
git commit -m "feat(log): add LogWriter header"
```

---

### Task 2: 创建 LogWriter 实现

**Files:**
- Create: `src/Log/LogWriter.cpp`

- [ ] **Step 1: 创建 LogWriter.cpp**

```cpp
#include "LogWriter.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QDirIterator>

LogWriter::LogWriter(QObject *parent)
    : QObject(parent)
{
    m_logDir = QDir(QCoreApplication::applicationDirPath() + "/logs");
    if (!m_logDir.exists()) {
        m_logDir.mkpath(".");
    }
}

void LogWriter::logOperation(const QString &level, const QString &type,
                              const QString &message, const QVariantMap &details)
{
    writeLog(level, type, message, details);
}

void LogWriter::logError(const QString &type, const QString &message,
                          const QVariantMap &details)
{
    writeLog("ERROR", type, message, details);
}

void LogWriter::writeLog(const QString &level, const QString &type,
                          const QString &message, const QVariantMap &details)
{
    QMutexLocker locker(&m_mutex);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString line = QString("[%1] [%2] [%3] %4")
                       .arg(timestamp, level, type, message);

    // 添加详细信息
    if (!details.isEmpty()) {
        QStringList detailParts;
        for (auto it = details.begin(); it != details.end(); ++it) {
            detailParts << QString("%1=%2").arg(it.key(), it.value().toString());
        }
        line += " | " + detailParts.join(" | ");
    }

    QString filePath = getLogFilePath();
    QFile file(filePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << line << "\n";
        file.close();
    }

    // 每次写入时检查是否需要清理旧日志
    static int writeCount = 0;
    if (++writeCount % 100 == 0) {
        cleanOldLogs();
    }
}

QString LogWriter::getLogFilePath()
{
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    return m_logDir.filePath(QString("app-%1.log").arg(dateStr));
}

void LogWriter::rotateLogs()
{
    // 按日期轮转由 getLogFilePath() 自动实现
    // 每天创建新文件
}

void LogWriter::cleanOldLogs()
{
    QDateTime cutoff = QDateTime::currentDateTime().addDays(-30);
    QDirIterator it(m_logDir.path(), {"app-*.log"}, QDir::Files);
    while (it.hasNext()) {
        it.next();
        QFileInfo fi = it.fileInfo();
        if (fi.lastModified() < cutoff) {
            QFile::remove(fi.filePath());
        }
    }
}
```

- [ ] **Step 2: Commit**

```bash
git add src/Log/LogWriter.cpp
git commit -m "feat(log): implement LogWriter"
```

---

### Task 3: 创建 FileService 头文件

**Files:**
- Create: `src/Log/FileService.h`

- [ ] **Step 1: 创建 FileService.h**

```cpp
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>

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

- [ ] **Step 2: Commit**

```bash
git add src/Log/FileService.h
git commit -m "feat(log): add FileService header"
```

---

### Task 4: 创建 FileService 实现

**Files:**
- Create: `src/Log/FileService.cpp`

- [ ] **Step 1: 创建 FileService.cpp**

```cpp
#include "FileService.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDirIterator>
#include <QUrl>

FileService::FileService(QObject *parent)
    : QObject(parent)
{
    m_logDir = QDir(QCoreApplication::applicationDirPath() + "/logs");
}

QString FileService::readLogFile(const QString &date)
{
    QString filePath = m_logDir.filePath(QString("app-%1.log").arg(date));
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    return in.readAll();
}

QStringList FileService::getLogFileDates()
{
    QStringList dates;
    QDirIterator it(m_logDir.path(), {"app-*.log"}, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        it.next();
        QString fileName = it.fileName();
        // 提取日期部分：app-YYYY-MM-DD.log
        if (fileName.startsWith("app-") && fileName.endsWith(".log")) {
            QString date = fileName.mid(4, 10); // "YYYY-MM-DD"
            dates << date;
        }
    }
    dates.sort(Qt::CaseInsensitive);
    return dates;
}

bool FileService::exportLogs(const QString &content, const QString &filePath)
{
    // 处理 QML 文件路径
    QString localPath = filePath;
    if (localPath.startsWith("file:///")) {
        localPath = QUrl(localPath).toLocalFile();
    }

    QFile file(localPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << content;
    file.close();
    return true;
}
```

- [ ] **Step 2: Commit**

```bash
git add src/Log/FileService.cpp
git commit -m "feat(log): implement FileService"
```

---

### Task 5: 创建 LogReader QML 模型

**Files:**
- Create: `ui/models/LogReader.qml`

- [ ] **Step 1: 创建 LogReader.qml**

```qml
pragma Singleton

import QtQuick

QtObject {
    id: root

    property var fileService: null

    property string currentDate: Qt.formatDate(new Date(), "yyyy-MM-dd")
    property string filterLevel: ""
    property string filterType: ""
    property string filterKeyword: ""
    property string filterDateStart: ""
    property string filterDateEnd: ""

    property var logEntries: []
    property var filteredEntries: []
    property int totalCount: 0

    property var availableDates: []

    function init(service) {
        fileService = service
        loadAvailableDates()
        loadLogs()
    }

    function loadAvailableDates() {
        if (!fileService) return
        availableDates = fileService.getLogFileDates()
    }

    function loadLogs() {
        if (!fileService) return

        logEntries = []

        // 确定要加载的日期范围
        var datesToLoad = []
        if (filterDateStart && filterDateEnd) {
            // 加载日期范围内的所有日志
            for (var i = 0; i < availableDates.length; i++) {
                var d = availableDates[i]
                if (d >= filterDateStart && d <= filterDateEnd) {
                    datesToLoad.push(d)
                }
            }
        } else if (filterDateStart) {
            datesToLoad.push(filterDateStart)
        } else if (filterDateEnd) {
            datesToLoad.push(filterDateEnd)
        } else {
            // 默认加载最新日期
            if (availableDates.length > 0) {
                datesToLoad.push(availableDates[availableDates.length - 1])
            }
        }

        // 读取并解析日志
        for (var di = 0; di < datesToLoad.length; di++) {
            var content = fileService.readLogFile(datesToLoad[di])
            if (content) {
                var lines = content.split("\n")
                for (var li = 0; li < lines.length; li++) {
                    var entry = parseLogLine(lines[li])
                    if (entry) {
                        logEntries.push(entry)
                    }
                }
            }
        }

        applyFilters()
    }

    function parseLogLine(line) {
        if (!line || line.trim().length === 0) return null

        // 格式: [YYYY-MM-DD HH:mm:ss] [LEVEL] [TYPE] message
        var regex = /^\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\] \[(\w+)\] \[(\w+)\] (.+)$/
        var match = line.match(regex)
        if (!match) return null

        return {
            "timestamp": match[1],
            "level": match[2],
            "type": match[3],
            "message": match[4],
            "raw": line
        }
    }

    function applyFilters() {
        var result = []

        for (var i = 0; i < logEntries.length; i++) {
            var entry = logEntries[i]

            // 级别筛选
            if (filterLevel && filterLevel !== "ALL" && entry.level !== filterLevel) {
                continue
            }

            // 类型筛选
            if (filterType && filterType !== "ALL" && entry.type !== filterType) {
                continue
            }

            // 关键词筛选
            if (filterKeyword && entry.raw.indexOf(filterKeyword) < 0) {
                continue
            }

            result.push(entry)
        }

        filteredEntries = result
        totalCount = result.length
    }

    function setFilterLevel(level) {
        filterLevel = level
        applyFilters()
    }

    function setFilterType(type) {
        filterType = type
        applyFilters()
    }

    function setFilterKeyword(keyword) {
        filterKeyword = keyword
        applyFilters()
    }

    function setDateRange(start, end) {
        filterDateStart = start
        filterDateEnd = end
        loadLogs()
    }

    function exportLogs(filePath) {
        if (!fileService) return false

        var content = ""
        for (var i = 0; i < filteredEntries.length; i++) {
            content += filteredEntries[i].raw + "\n"
        }

        return fileService.exportLogs(content, filePath)
    }

    function clearFilters() {
        filterLevel = ""
        filterType = ""
        filterKeyword = ""
        filterDateStart = ""
        filterDateEnd = ""
        loadLogs()
    }
}
```

- [ ] **Step 2: Commit**

```bash
git add ui/models/LogReader.qml
git commit -m "feat(log): add LogReader QML model"
```

---

### Task 6: 创建 PageLogViewer 页面

**Files:**
- Create: `ui/pages/PageLogViewer.qml`

- [ ] **Step 1: 创建 PageLogViewer.qml**

```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Rectangle {
    id: root
    color: Theme.bg

    property var sessionManager
    property var fileService

    Component.onCompleted: {
        LogReader.init(fileService)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingMd
        spacing: Theme.spacingMd

        // 页面标题
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("日志查看")
                font.pixelSize: Theme.fontXl
                font.bold: true
                color: Theme.text
            }
            Item { Layout.fillWidth: true }
            Label {
                text: qsTr("共 %1 条记录").arg(LogReader.totalCount)
                color: Theme.textMuted
            }
        }

        // 筛选栏
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: filterRow.implicitHeight + Theme.spacingMd * 2
            color: Theme.surface
            radius: Theme.radiusMd

            RowLayout {
                id: filterRow
                anchors.fill: parent
                anchors.margins: Theme.spacingMd
                spacing: Theme.spacingSm

                // 日期范围
                ColumnLayout {
                    spacing: 4
                    Label {
                        text: qsTr("日期范围")
                        font.pixelSize: Theme.fontXs
                        color: Theme.textMuted
                    }
                    RowLayout {
                        spacing: 4
                        TextField {
                            id: dateStart
                            Layout.preferredWidth: 100
                            placeholderText: qsTr("开始日期")
                            inputMethodHints: Qt.ImhDate
                            onTextChanged: {
                                if (text.length === 10) {
                                    LogReader.setDateRange(text, dateEnd.text)
                                }
                            }
                        }
                        Label { text: "至"; color: Theme.textMuted }
                        TextField {
                            id: dateEnd
                            Layout.preferredWidth: 100
                            placeholderText: qsTr("结束日期")
                            inputMethodHints: Qt.ImhDate
                            onTextChanged: {
                                if (text.length === 10) {
                                    LogReader.setDateRange(dateStart.text, text)
                                }
                            }
                        }
                    }
                }

                // 级别筛选
                ColumnLayout {
                    spacing: 4
                    Label {
                        text: qsTr("级别")
                        font.pixelSize: Theme.fontXs
                        color: Theme.textMuted
                    }
                    ComboBox {
                        id: levelCombo
                        Layout.preferredWidth: 100
                        model: ["ALL", "INFO", "WARN", "ERROR"]
                        onCurrentTextChanged: LogReader.setFilterLevel(currentText)
                    }
                }

                // 类型筛选
                ColumnLayout {
                    spacing: 4
                    Label {
                        text: qsTr("类型")
                        font.pixelSize: Theme.fontXs
                        color: Theme.textMuted
                    }
                    ComboBox {
                        id: typeCombo
                        Layout.preferredWidth: 100
                        model: ["ALL", "LOGIN", "DATA", "CONFIG", "SYSTEM"]
                        onCurrentTextChanged: LogReader.setFilterType(currentText)
                    }
                }

                // 关键词搜索
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    Label {
                        text: qsTr("关键词")
                        font.pixelSize: Theme.fontXs
                        color: Theme.textMuted
                    }
                    TextField {
                        id: keywordField
                        Layout.fillWidth: true
                        placeholderText: qsTr("搜索日志内容...")
                        onTextChanged: LogReader.setFilterKeyword(text)
                    }
                }

                // 操作按钮
                ColumnLayout {
                    spacing: 4
                    Label { text: " "; font.pixelSize: Theme.fontXs }
                    RowLayout {
                        spacing: Theme.spacingSm
                        Button {
                            text: qsTr("重置")
                            onClicked: {
                                dateStart.text = ""
                                dateEnd.text = ""
                                levelCombo.currentIndex = 0
                                typeCombo.currentIndex = 0
                                keywordField.text = ""
                                LogReader.clearFilters()
                            }
                        }
                        Button {
                            text: qsTr("导出")
                            highlighted: true
                            onClicked: {
                                fileDialog.open()
                            }
                        }
                    }
                }
            }
        }

        // 日志表格
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surface
            radius: Theme.radiusMd

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // 表头
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    color: Theme.surfaceAlt

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: Theme.spacingMd
                        anchors.rightMargin: Theme.spacingMd
                        spacing: Theme.spacingSm

                        Label {
                            Layout.preferredWidth: 160
                            text: qsTr("时间")
                            font.bold: true
                            color: Theme.text
                            font.pixelSize: Theme.fontSm
                        }
                        Label {
                            Layout.preferredWidth: 70
                            text: qsTr("级别")
                            font.bold: true
                            color: Theme.text
                            font.pixelSize: Theme.fontSm
                        }
                        Label {
                            Layout.preferredWidth: 80
                            text: qsTr("类型")
                            font.bold: true
                            color: Theme.text
                            font.pixelSize: Theme.fontSm
                        }
                        Label {
                            Layout.fillWidth: true
                            text: qsTr("详情")
                            font.bold: true
                            color: Theme.text
                            font.pixelSize: Theme.fontSm
                        }
                    }
                }

                // 日志列表
                ListView {
                    id: logList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: LogReader.filteredEntries

                    delegate: Rectangle {
                        required property var modelData
                        required property int index

                        width: logList.width
                        height: 40
                        color: modelData.level === "ERROR" ? "#fff1f0" : (index % 2 === 0 ? Theme.surface : Theme.surfaceAlt)

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: Theme.spacingMd
                            anchors.rightMargin: Theme.spacingMd
                            spacing: Theme.spacingSm

                            Label {
                                Layout.preferredWidth: 160
                                text: modelData.timestamp
                                color: Theme.text
                                font.pixelSize: Theme.fontXs
                                font.family: Theme.fontMono
                            }

                            Rectangle {
                                Layout.preferredWidth: 60
                                Layout.preferredHeight: 22
                                radius: 4
                                color: {
                                    switch (modelData.level) {
                                    case "INFO": return "#e6f7ff"
                                    case "WARN": return "#fffbe6"
                                    case "ERROR": return "#fff1f0"
                                    default: return Theme.surfaceAlt
                                    }
                                }

                                Label {
                                    anchors.centerIn: parent
                                    text: modelData.level
                                    font.pixelSize: Theme.fontXs
                                    font.bold: true
                                    color: {
                                        switch (modelData.level) {
                                        case "INFO": return "#1890ff"
                                        case "WARN": return "#faad14"
                                        case "ERROR": return "#ff4d4f"
                                        default: return Theme.textMuted
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: 70
                                Layout.preferredHeight: 22
                                radius: 4
                                color: Theme.surfaceAlt

                                Label {
                                    anchors.centerIn: parent
                                    text: modelData.type
                                    font.pixelSize: Theme.fontXs
                                    color: Theme.text
                                }
                            }

                            Label {
                                Layout.fillWidth: true
                                text: modelData.message
                                color: modelData.level === "ERROR" ? "#ff4d4f" : Theme.text
                                font.pixelSize: Theme.fontXs
                                elide: Text.ElideRight
                                wrapMode: Text.WrapAnywhere
                                maximumLineCount: 1
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                }
            }
        }
    }

    // 文件保存对话框
    FileDialog {
        id: fileDialog
        title: qsTr("导出日志")
        fileMode: FileDialog.SaveFile
        nameFilters: ["Text files (*.txt)", "Log files (*.log)", "All files (*)"]
        onAccepted: {
            var success = LogReader.exportLogs(selectedFile)
            if (success) {
                Logger.ok(qsTr("日志导出成功"))
            } else {
                Logger.error(qsTr("日志导出失败"))
            }
        }
    }
}
```

- [ ] **Step 2: Commit**

```bash
git add ui/pages/PageLogViewer.qml
git commit -m "feat(log): add PageLogViewer page"
```

---

### Task 7: 修改 PermissionCatalog 添加导航项

**Files:**
- Modify: `ui/models/PermissionCatalog.qml`

- [ ] **Step 1: 添加导航项**

在 `allNavItems` 数组末尾添加：

```qml
{ group: qsTr("其它") },
{ key: "logViewer", label: qsTr("日志查看"), icon: "📋" },
```

- [ ] **Step 2: 添加权限检查**

在 `canAccessNav` 函数的 switch 语句中添加：

```qml
case "logViewer":
    return hasRoleKey(sessionManager, "super_admin")
```

- [ ] **Step 3: 添加访问提示**

在 `accessRequirementHint` 函数的 switch 语句中添加：

```qml
case "logViewer":
    return roleDisplayName("super_admin")
```

- [ ] **Step 4: Commit**

```bash
git add ui/models/PermissionCatalog.qml
git commit -m "feat(log): add log viewer navigation and permissions"
```

---

### Task 8: 修改 PageHost 添加页面路由

**Files:**
- Modify: `ui/components/PageHost.qml`

- [ ] **Step 1: 添加属性**

在 Item 的属性声明中添加：

```qml
property var logWriter
property var fileService
```

- [ ] **Step 2: 添加路由**

在 `pageLoader` 的 `sourceComponent` switch 语句中添加：

```qml
case "logViewer": return compLogViewer
```

- [ ] **Step 3: 添加组件**

在文件末尾的 Component 声明区域添加：

```qml
Component {
    id: compLogViewer
    PageLogViewer {
        sessionManager: host.sessionManager
        fileService: host.fileService
    }
}
```

- [ ] **Step 4: Commit**

```bash
git add ui/components/PageHost.qml
git commit -m "feat(log): add log viewer route to PageHost"
```

---

### Task 9: 修改 main.cpp 注册服务

**Files:**
- Modify: `main.cpp`

- [ ] **Step 1: 添加头文件**

在文件顶部的 include 区域添加：

```cpp
#include "src/Log/LogWriter.h"
#include "src/Log/FileService.h"
```

- [ ] **Step 2: 创建服务实例**

在 `SessionManager` 创建之后添加：

```cpp
auto *logWriter = new LogWriter(&app);
auto *fileService = new FileService(&app);
```

- [ ] **Step 3: 设置引擎属性**

在 `engine.setInitialProperties` 中添加：

```qml
{ QStringLiteral("logWriter"), QVariant::fromValue(logWriter) },
{ QStringLiteral("fileService"), QVariant::fromValue(fileService) },
```

- [ ] **Step 4: 修改 qtMessageHandler**

在 `qtMessageHandler` 函数中，在 `fprintf` 之后添加日志写入：

```cpp
// 在函数开头声明外部变量引用
extern LogWriter *g_logWriter;

// 在 switch 语句后添加
if (g_logWriter) {
    switch (type) {
    case QtWarningMsg:
        g_logWriter->logError("SYSTEM", QString::fromLocal8Bit(localMsg));
        break;
    case QtCriticalMsg:
        g_logWriter->logError("SYSTEM", QString::fromLocal8Bit(localMsg));
        break;
    case QtFatalMsg:
        g_logWriter->logError("SYSTEM", QString::fromLocal8Bit(localMsg));
        break;
    default:
        break;
    }
}
```

- [ ] **Step 5: 声明全局变量**

在 main 函数之前添加：

```cpp
LogWriter *g_logWriter = nullptr;
```

在 logWriter 创建后添加：

```cpp
g_logWriter = logWriter;
```

- [ ] **Step 6: Commit**

```bash
git add main.cpp
git commit -m "feat(log): register LogWriter and FileService"
```

---

### Task 10: 修改 CMakeLists.txt

**Files:**
- Modify: `CMakeLists.txt`

- [ ] **Step 1: 添加 QML 文件**

在 `qt_add_qml_module` 的 `QML_FILES` 部分添加：

```
ui/models/LogReader.qml
ui/pages/PageLogViewer.qml
```

- [ ] **Step 2: 添加单例属性**

在 `set_source_files_properties` 中添加：

```
ui/models/LogReader.qml
```

并在 `PROPERTIES` 部分确保 `QT_QML_SINGLETON_TYPE TRUE` 包含它。

- [ ] **Step 3: 添加 C++ 源文件**

在 `qt_add_qml_module` 的 `SOURCES` 部分添加：

```
src/Log/LogWriter.h src/Log/LogWriter.cpp
src/Log/FileService.h src/Log/FileService.cpp
```

- [ ] **Step 4: Commit**

```bash
git add CMakeLists.txt
git commit -m "feat(log): add log files to CMakeLists"
```

---

### Task 11: 集成测试

- [ ] **Step 1: 构建项目**

```bash
cd build
cmake ..
cmake --build .
```

Expected: 构建成功，无错误

- [ ] **Step 2: 运行程序**

```bash
./appAttendanceAdmin
```

Expected: 程序正常启动

- [ ] **Step 3: 测试登录日志**

1. 使用超级管理员账号登录
2. 检查 `logs/` 目录是否生成日志文件
3. 检查日志文件内容是否包含登录记录

Expected: 日志文件包含 `[INFO] [LOGIN] 用户登录` 记录

- [ ] **Step 4: 测试日志查看页面**

1. 在导航栏点击"其它" -> "日志查看"
2. 验证日志列表显示
3. 测试筛选功能
4. 测试导出功能

Expected: 页面正常显示，筛选和导出功能正常

- [ ] **Step 5: 测试权限控制**

1. 使用非超级管理员账号登录
2. 检查导航栏是否显示"日志查看"

Expected: 非超级管理员看不到"日志查看"导航项

- [ ] **Step 6: Final Commit**

```bash
git add -A
git commit -m "feat(log): complete log persistence feature"
```

---

## 风险与缓解

| 风险 | 缓解措施 |
|------|----------|
| 日志写入影响性能 | 使用 QMutex 保护，写入操作简单快速 |
| 日志文件过大 | 按日期轮转，自动清理30天前的日志 |
| 并发写入冲突 | QMutex 互斥锁保护 |
| 磁盘空间不足 | 定期清理过期日志 |
