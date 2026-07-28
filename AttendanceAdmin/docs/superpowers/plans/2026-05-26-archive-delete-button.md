# 归档记录删除按钮实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在考勤记录页面的归档记录标签页中添加删除按钮，允许管理员根据工号删除对应的归档考勤记录。

**Architecture:** 在现有的归档筛选区域添加删除按钮，复用工号输入框，使用ConfirmDialog确认删除，删除成功后自动刷新归档列表。

**Tech Stack:** QML, Qt Quick Controls 2, PermissionButton, ConfirmDialog

---

## 文件结构

- **Modify**: `ui/pages/PageAttendance.qml` - 添加删除按钮和确认对话框
- **Test**: 手动测试 - 权限控制、删除功能、UI交互

---

### Task 1: 添加删除按钮

**Files:**
- Modify: `ui/pages/PageAttendance.qml:253-267`

- [ ] **Step 1: 定位归档筛选区域的按钮行**

在`ui/pages/PageAttendance.qml`中找到第253-267行的归档筛选区域按钮行。

- [ ] **Step 2: 在查询归档按钮右侧添加删除按钮**

```qml
Row {
    spacing: Theme.spacingSm
    PermissionButton {
        sessionManager: page.sessionManager
        requiredPermission: "attendance.archive.read"
        deniedDialog: page.deniedDialog
        text: qsTr("查询归档")
        enabled: !attendanceService.busy
        onClicked: guardedClick(function() {
            attendanceService.queryArchive(
                -1, archEmp.text.trim(), archName.text.trim(),
                archDept.text.trim(), "", "", "", "", "", "", "")
        })
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

- [ ] **Step 3: 验证按钮添加成功**

运行应用程序，切换到归档记录标签页，确认：
- 删除按钮显示在查询归档按钮右侧
- 工号为空时按钮不可用
- 有权限的用户可以看到按钮

---

### Task 2: 添加确认对话框

**Files:**
- Modify: `ui/pages/PageAttendance.qml:300` (BusyOverlay之前)

- [ ] **Step 1: 在BusyOverlay之前添加ConfirmDialog**

```qml
ConfirmDialog {
    id: confirmDeleteArchive
    message: qsTr("确认删除工号 ") + archEmp.text.trim() + qsTr(" 的所有归档记录？")
    onAccepted: attendanceService.deleteArchive(archEmp.text.trim())
}

BusyOverlay { busy: attendanceService.busy }
```

- [ ] **Step 2: 验证确认对话框功能**

运行应用程序，输入工号，点击删除按钮，确认：
- 确认对话框正确显示工号
- 点击取消不执行删除
- 点击确认执行删除

---

### Task 3: 修改信号处理

**Files:**
- Modify: `ui/pages/PageAttendance.qml:303-311`

- [ ] **Step 1: 找到现有的Connections组件**

在`ui/pages/PageAttendance.qml`中找到第303-311行的Connections组件。

- [ ] **Step 2: 添加删除成功后刷新逻辑**

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

- [ ] **Step 3: 验证自动刷新功能**

运行应用程序，执行删除操作，确认：
- 删除成功后归档列表自动刷新
- 刷新时使用当前的筛选条件

---

### Task 4: 权限控制测试

- [ ] **Step 1: 测试无权限用户**

使用没有`attendance.archive.delete`权限的用户登录，确认：
- 删除按钮不可见或不可用
- 点击按钮显示权限拒绝对话框

- [ ] **Step 2: 测试有权限用户**

使用有`attendance.archive.delete`权限的用户登录，确认：
- 删除按钮可见且可用
- 可以正常执行删除操作

---

### Task 5: 边界条件测试

- [ ] **Step 1: 测试空工号**

清空工号输入框，确认：
- 删除按钮不可用
- 无法触发删除操作

- [ ] **Step 2: 测试不存在的工号**

输入不存在的工号，执行删除，确认：
- 显示适当的错误信息
- 列表保持不变

- [ ] **Step 3: 测试网络异常**

模拟网络异常，执行删除，确认：
- 显示超时错误信息
- 应用程序不会崩溃

---

### Task 6: UI一致性验证

- [ ] **Step 1: 验证按钮位置**

确认删除按钮：
- 位于查询归档按钮右侧
- 与其他按钮间距一致
- 样式与其他PermissionButton一致

- [ ] **Step 2: 验证确认对话框**

确认对话框：
- 消息清晰显示工号
- 按钮布局合理
- 样式与其他ConfirmDialog一致

---

## 提交信息

完成所有任务后，提交代码：

```bash
git add ui/pages/PageAttendance.qml
git commit -m "feat: add delete button for archive attendance records"
```
