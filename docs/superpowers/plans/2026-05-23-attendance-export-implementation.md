# 考勤记录 CSV 导出实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在实时考勤 Tab 中增加 CSV 导出功能，将当前内存中的记录导出为 CSV 文件。

**Architecture:** C++ 侧新增 `exportToFile` 方法写文件；QML 侧用 `FileDialog` 选路径，JS 遍历 `attendanceService.records` 生成 CSV 字符串，调用 C++ 方法保存。

**Tech Stack:** Qt 6 C++ (QFile/QTextStream), QML FileDialog, JS

---

## 文件结构

| 文件 | 操作 | 职责 |
|------|------|------|
| `src/Attendance/AttendanceService.h` | 修改 | 声明 `exportToFile` 方法 |
| `src/Attendance/AttendanceService.cpp` | 修改 | 实现文件写入 |
| `ui/pages/PageAttendance.qml` | 修改 | 导出按钮 + FileDialog + CSV 生成 |

---

### Task 1: C++ 后端 — 添加 exportToFile 方法

**Files:**
- Modify: `src/Attendance/AttendanceService.h`
- Modify: `src/Attendance/AttendanceService.cpp`

- [ ] **Step 1: 在头文件中声明方法**

在 `AttendanceService.h` 中，找到 `Q_INVOKABLE void deleteArchive(...)` 之后，添加：

```cpp
    // 导出文件到本地（客户端侧写文件），成功后 emit operationSucceeded("attendance.export", ...)
    Q_INVOKABLE void exportToFile(const QString& filePath, const QString& content);
```

插入位置：第 56 行 `Q_INVOKABLE void deleteArchive(const QString& employeeId);` 之后，`signals:` 之前。

- [ ] **Step 2: 在 cpp 中实现方法**

在 `AttendanceService.cpp` 末尾（`deleteArchive` 实现之后，文件结束前）添加：

```cpp
void AttendanceService::exportToFile(const QString& filePath, const QString& content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit operationFailed(QStringLiteral("attendance.export"), -1,
                             QStringLiteral("无法打开文件: ") + file.errorString());
        return;
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << content;
    file.close();
    emit operationSucceeded(QStringLiteral("attendance.export"), QStringLiteral("导出成功"));
}
```

需要在 cpp 文件顶部添加 include：

```cpp
#include <QFile>
#include <QTextStream>
```

检查是否已有 `QFile` 和 `QTextStream` 的 include，如果没有则添加。

- [ ] **Step 3: 编译验证**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1 | tail -20
```
预期：编译通过

- [ ] **Step 4: Commit**

```bash
git add src/Attendance/AttendanceService.h src/Attendance/AttendanceService.cpp
git commit -m "feat(attendance): add exportToFile method for CSV export

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```

---

### Task 2: QML 前端 — 导出按钮、FileDialog、CSV 生成

**Files:**
- Modify: `ui/pages/PageAttendance.qml`

- [ ] **Step 0: 添加 FileDialog 所需的 import**

在文件顶部 import 区域（约第 1-6 行），在现有 `import QtQuick.Layouts` 之后添加：

```qml
import QtQuick.Dialogs
```

目前顶部 import 为：
```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin
```

改为：
```qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import AttendanceAdmin
```

- [ ] **Step 1: 在"实时记录" Card 的 headerRight 添加导出按钮**

找到"实时记录" Card（`title: qsTr("实时记录")`，约第 137 行），在 DataTable 前面、Card 内部，为 Card 添加 `headerRight` 属性：

修改前（约第 137-142 行）：
```qml
                Card {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    stretchContent: true
                    title: qsTr("实时记录")
```

修改后：
```qml
                Card {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    stretchContent: true
                    title: qsTr("实时记录")

                    headerRight: Row {
                        spacing: Theme.spacingSm
                        PermissionButton {
                            sessionManager: page.sessionManager
                            requiredPermission: "attendance.read"
                            deniedDialog: page.deniedDialog
                            text: qsTr("导出 CSV")
                            enabled: attendanceService.records
                                     && attendanceService.records.length > 0
                                     && !attendanceService.busy
                            onClicked: guardedClick(function() { exportDialog.open() })
                        }
                    }
```

- [ ] **Step 2: 在"实时考勤" ColumnLayout 内添加 FileDialog 和 CSV 生成逻辑**

在实时考勤 Tab 的 ColumnLayout 末尾（在 DataTable 的 Card 闭合 `}` 之后、ColumnLayout 闭合 `}` 之前），添加 FileDialog 和 JS 函数。

先找到实时考勤 Tab 的 ColumnLayout 结束位置（约第 162 行 `}` 关闭 DataTable Card，然后 `}` 关闭 ColumnLayout）。在最后一个 Card 的 `}` 之后、ColumnLayout 的 `}` 之前插入：

```qml
                FileDialog {
                    id: exportDialog
                    title: qsTr("导出 CSV")
                    nameFilters: [qsTr("CSV 文件 (*.csv)")]
                    fileMode: FileDialog.SaveFile
                    defaultSuffix: "csv"
                    currentFile: "attendance_" + qsTr("实时记录") + "_"
                                 + Qt.formatDateTime(new Date(), "yyyyMMdd") + ".csv"
                    onAccepted: {
                        const csv = page._generateCsv()
                        if (csv.length > 0)
                            attendanceService.exportToFile(selectedFile, csv)
                    }
                }
```

- [ ] **Step 3: 添加 _generateCsv JS 函数**

在 PageAttendance 的 `function _liveQuery()` 函数之后（约第 24 行），添加：

```qml
    function _generateCsv() {
        const records = attendanceService.records
        if (!records || !records.length)
            return ""
        // UTF-8 BOM
        var csv = "﻿"
        // 表头
        csv += qsTr("序号") + "," + qsTr("工号") + "," + qsTr("打卡时间") + ","
            + qsTr("设备ID") + "," + qsTr("状态") + "\n"
        for (var i = 0; i < records.length; i++) {
            var r = records[i]
            var row = [
                String(i + 1),
                r.employeeId || "",
                r.checkTime || "",
                r.deviceId || "",
                Theme.formatAttendanceStatus(r.status)
            ]
            // 逗号转义
            for (var j = 0; j < row.length; j++) {
                if (row[j].indexOf(",") >= 0 || row[j].indexOf("\"") >= 0) {
                    row[j] = "\"" + row[j].replace(/"/g, "\"\"") + "\""
                }
            }
            csv += row.join(",") + "\n"
        }
        return csv
    }
```

- [ ] **Step 4: 在 Connections 中添加导出结果处理**

找到 `Connections { target: attendanceService ... }`（约第 244 行），在 `onOperationSucceeded` / `onOperationFailed` 中确认已有 `page.serviceResult` 调用（已有，无需额外添加），导出成功/失败直接通过现有的 `page.serviceResult` 传递。

现有代码已满足需求：
```qml
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
```

无需修改 Connections 块。

- [ ] **Step 5: 编译验证**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1 | tail -20
```

- [ ] **Step 6: Commit**

```bash
git add ui/pages/PageAttendance.qml
git commit -m "feat(ui): add CSV export button and FileDialog to attendance page

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```

---

### Task 3: 最终编译验证

**Files:** 无新文件

- [ ] **Step 1: 完整编译**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1
```
预期：编译通过

- [ ] **Step 2: 功能验证清单**

启动应用后手动验证：
1. 登录 → 导航到"考勤记录" → "实时考勤" Tab
2. 先点"查询"加载数据 → "导出 CSV" 按钮变为可用
3. 无数据时"导出 CSV"按钮禁用
4. 点击"导出 CSV" → FileDialog 弹出，默认文件名含当天日期
5. 选择保存路径 → CSV 文件生成
6. 用 Excel/WPS 打开 CSV → 中文表头正常显示（UTF-8 BOM），状态列为中文
7. 状态栏/Logger 显示"导出成功"

- [ ] **Step 3: Commit**

```bash
git add ui/pages/PageAttendance.qml
git commit -m "chore: finalize attendance CSV export feature

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```
