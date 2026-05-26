# 归档记录删除按钮设计文档

## 概述

在考勤记录页面的归档记录标签页中添加删除按钮，允许管理员根据工号删除对应的归档考勤记录。

## 需求

- 在"查询归档"按钮右侧添加"删除归档记录"按钮
- 根据工号删除对应的归档记录
- 使用确认对话框防止误操作
- 删除成功后自动刷新归档列表

## 设计方案

### 方案选择

**推荐方案：复用筛选条件中的工号输入框**

理由：
1. 实现简单，符合"根据工号删除"的需求
2. 用户查询时已输入工号，可直接复用
3. 与现有UI风格一致
4. 有确认对话框保护

### 实现细节

#### 1. UI变更

**文件**: `ui/pages/PageAttendance.qml`

在归档筛选区域（第253-267行）的"查询归档"按钮右侧添加删除按钮：

```qml
Row {
    spacing: Theme.spacingSm
    PermissionButton {
        // 现有的"查询归档"按钮
        ...
    }
    PermissionButton {
        sessionManager: page.sessionManager
        requiredPermission: "attendance.archive.delete"
        deniedDialog: page.deniedDialog
        text: qsTr("删除归档记录")
        enabled: !attendanceService.busy && archEmp.text.trim() !== ""
        onClicked: guardedClick(function() { confirmDeleteArchive.open() })
    }
}
```

#### 2. 确认对话框

在页面底部添加确认对话框：

```qml
ConfirmDialog {
    id: confirmDeleteArchive
    message: qsTr("确认删除工号 ") + archEmp.text.trim() + qsTr(" 的所有归档记录？")
    onAccepted: attendanceService.deleteArchive(archEmp.text.trim())
}
```

#### 3. 信号处理

在现有的`Connections`组件中添加删除归档的成功处理：

```qml
Connections {
    target: attendanceService
    function onOperationSucceeded(apiType, message) {
        page.serviceResult(apiType, 0, message)
        // 删除归档成功后刷新列表
        if (apiType === "attendance.archive.delete") {
            attendanceService.queryArchive(
                -1, archEmp.text.trim(), archName.text.trim(),
                archDept.text.trim(), "", "", "", "", "", "", "")
        }
    }
    function onOperationFailed(apiType, code, message) {
        page.serviceResult(apiType, code, message)
    }
}
```

### 权限要求

- 权限名称：`attendance.archive.delete`
- 显示名称：删除归档考勤
- 分类：考勤管理

### 依赖项

1. **后端API**: `AttendanceService.deleteArchive(employeeId)` - 已实现
2. **权限系统**: `PermissionCatalog` - 已支持
3. **确认对话框**: `ConfirmDialog` - 已存在
4. **权限按钮**: `PermissionButton` - 已存在

## 测试计划

1. **功能测试**
   - 输入工号后点击删除按钮
   - 确认对话框正确显示
   - 确认后成功删除归档记录
   - 删除后列表自动刷新

2. **权限测试**
   - 无权限用户看不到删除按钮
   - 无权限用户点击按钮显示权限拒绝对话框

3. **边界测试**
   - 工号为空时按钮不可用
   - 删除不存在的工号时显示错误信息
   - 网络异常时显示超时错误

4. **UI测试**
   - 按钮位置正确（查询归档按钮右侧）
   - 按钮样式与其他按钮一致
   - 确认对话框消息清晰

## 实现步骤

1. 在`PageAttendance.qml`中添加删除按钮
2. 添加确认对话框组件
3. 修改`Connections`处理删除成功后刷新列表
4. 测试权限控制和删除功能
