# 考勤记录 CSV 导出

## 目标

在"实时考勤"Tab 中增加 CSV 导出功能，将当前内存中的实时记录导出为 CSV 文件。

## 架构

```
用户点击"导出 CSV" → FileDialog 选保存路径 →
QML/JS 生成 CSV 字符串（UTF-8 BOM） →
AttendanceService.exportToFile(path, csv) → C++ 写文件 →
emit 成功/失败信号 → QML 反馈
```

## CSV 格式

列：序号, 工号, 打卡时间, 设备ID, 状态

```csv
序号,工号,打卡时间,设备ID,状态
1,E001,2025-06-15 08:00:00,DEV01,正常
```

- UTF-8 BOM 头，确保 Excel/WPS 正确识别中文
- 状态字段用中文显示（正常/迟到/早退/缺勤/补签），复用 `Theme.formatAttendanceStatus()`
- 逗号分隔，字段值含逗号时用双引号包裹

## 变更范围

| 文件 | 改动 | 描述 |
|------|------|------|
| `src/Attendance/AttendanceService.h` | +1 行 | `Q_INVOKABLE void exportToFile(const QString& filePath, const QString& content)` |
| `src/Attendance/AttendanceService.cpp` | +15 行 | 用 `QFile` 写文件，UTF-8 编码，emit 结果信号 |
| `ui/pages/PageAttendance.qml` | +70 行 | 导出按钮、FileDialog、CSV 字符串生成、结果提示 |

### C++ 方法签名

```cpp
Q_INVOKABLE void exportToFile(const QString& filePath, const QString& content);
```

实现：`QFile` 以 `WriteOnly | Text` 打开，`QTextStream` 设 UTF-8 编码写 `content`，成功 emit `operationSucceeded("attendance.export", ...)`，失败 emit `operationFailed`。

### QML 逻辑

1. `PermissionButton`（`requiredPermission: "attendance.read"`）放在"实时记录" Card 的 `headerRight`
2. 点击 → `FileDialog` 以 `saveFile` 模式打开，默认文件名 `attendance_实时记录_YYYYMMDD.csv`
3. 用户选择路径 → JS 遍历 `attendanceService.records` 生成 CSV 字符串
4. 调用 `attendanceService.exportToFile(path, csv)`
5. 在 `onOperationSucceeded` / `onOperationFailed` 中通过 `page.serviceResult` 传递结果

## 不涉及的文件

- `PermissionCatalog.qml` — 复用已有 `attendance.read` 权限
- `main.cpp` — 无需注册新对象
- `PageHost.qml` — 无需修改
