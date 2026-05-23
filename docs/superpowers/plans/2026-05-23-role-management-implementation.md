# 角色管理界面实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在 PageRbac.qml 中增加"角色管理"Tab，提供角色的创建、删除和权限修改功能。

**Architecture:** 单文件改造 — 将现有 PageRbac.qml 的用户-角色分配内容包裹在 StackLayout 的第一个 Tab 中，新增第二个 Tab 实现角色 CRUD。权限按模块分组为 CheckBox 网格。利用现有 RbacServer API，后端零改动。

**Tech Stack:** Qt 6 QML, 复用现有组件 Card / DataTable / ConfirmDialog / PermissionButton / BusyOverlay / LabeledField

---

## 文件结构

| 文件 | 操作 | 职责 |
|------|------|------|
| `ui/pages/PageRbac.qml` | 修改 | 增加 TabBar + StackLayout，新增角色管理 Tab 的全部 UI 和逻辑 |

---

### Task 1: 添加 Tab 状态和新 state 属性

**Files:**
- Modify: `ui/pages/PageRbac.qml`（在现有属性区域之后，Component.onCompleted 之前插入）

- [ ] **Step 1: 在 `signal serviceResult(...)` 之后插入 Tab 和角色管理状态属性**

在现有 `signal serviceResult(...)` 行之后、`property int selectedUserId: -1` 行之前插入：

```qml
    // ── Tab 状态 ──
    property int currentTab: 0 // 0=用户权限分配, 1=角色管理

    // ── 角色管理状态 ──
    property int selectedRoleIndex: -1
    property bool isCreatingRole: false
    property var checkedPermissions: ({})
    property string formRoleKey: ""
    property string formRoleName: ""
    property string formDescription: ""
    property bool formIsSystem: false
    property bool formDirty: false

    // ── 权限分组（按模块） ──
    readonly property var permissionGroups: [
        {
            name: qsTr("人员管理"),
            perms: [
                { key: "person.read", label: qsTr("查询人员") },
                { key: "person.create", label: qsTr("新增人员") },
                { key: "person.update", label: qsTr("修改人员") },
                { key: "person.delete", label: qsTr("删除人员") }
            ]
        },
        {
            name: qsTr("考勤记录"),
            perms: [
                { key: "attendance.read", label: qsTr("查询考勤") },
                { key: "attendance.create", label: qsTr("录入考勤") },
                { key: "attendance.update", label: qsTr("修改考勤") },
                { key: "attendance.delete", label: qsTr("删除考勤") }
            ]
        },
        {
            name: qsTr("考勤归档"),
            perms: [
                { key: "attendance.archive.read", label: qsTr("查询归档") },
                { key: "attendance.archive.delete", label: qsTr("删除归档") }
            ]
        },
        {
            name: qsTr("设备管理"),
            perms: [
                { key: "device.read", label: qsTr("查询设备") },
                { key: "device.create", label: qsTr("新增设备") },
                { key: "device.update", label: qsTr("修改设备") },
                { key: "device.delete", label: qsTr("删除设备") },
                { key: "device.command", label: qsTr("设备指令") }
            ]
        },
        {
            name: qsTr("用户账号"),
            perms: [
                { key: "user.read", label: qsTr("查询用户") },
                { key: "user.create", label: qsTr("新增用户") },
                { key: "user.update", label: qsTr("修改用户") },
                { key: "user.delete", label: qsTr("删除用户") }
            ]
        },
        {
            name: qsTr("人脸库"),
            perms: [
                { key: "face.read", label: qsTr("查询人脸") },
                { key: "face.register", label: qsTr("注册人脸") },
                { key: "face.delete", label: qsTr("删除人脸") }
            ]
        },
        {
            name: qsTr("其他"),
            perms: [
                { key: "config.deploy", label: qsTr("配置下发") },
                { key: "event.subscribe", label: qsTr("订阅事件") }
            ]
        }
    ]
```

- [ ] **Step 2: 验证文件无语法错误**

运行：`cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1 | tail -20`
预期：编译通过（可能因后续引用未完成而警告，但不应因属性定义而报错）

- [ ] **Step 3: Commit**

```bash
git add ui/pages/PageRbac.qml
git commit -m "feat(ui): add tab state and permission group model for role management

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```

---

### Task 2: 添加角色管理辅助函数

**Files:**
- Modify: `ui/pages/PageRbac.qml`（在现有 `_saveRoles()` 函数之后，`Component.onCompleted` 之前插入）

- [ ] **Step 1: 在 `_saveRoles` 函数闭合大括号后插入辅助函数**

```qml
    // ── 角色管理辅助函数 ──

    function _permChecked(permKey) {
        return !!checkedPermissions[permKey]
    }

    function _setPerm(permKey, on) {
        if (!!checkedPermissions[permKey] === on)
            return
        const next = Object.assign({}, checkedPermissions)
        if (on)
            next[permKey] = true
        else
            delete next[permKey]
        checkedPermissions = next
        formDirty = true
    }

    function _collectCheckedPerms() {
        const keys = []
        for (const k in checkedPermissions) {
            if (checkedPermissions[k])
                keys.push(k)
        }
        return keys
    }

    function _loadRoleIntoForm(row) {
        selectedRoleIndex = -1
        isCreatingRole = false
        formDirty = false
        if (!row) {
            formRoleKey = ""
            formRoleName = ""
            formDescription = ""
            formIsSystem = false
            checkedPermissions = {}
            return
        }
        formRoleKey = row.roleKey || ""
        formRoleName = row.roleName || ""
        formDescription = row.description || ""
        formIsSystem = !!(row.isSystem)
        const perms = row.permissions
        const map = {}
        if (Array.isArray(perms)) {
            for (let i = 0; i < perms.length; ++i)
                map[perms[i]] = true
        }
        checkedPermissions = map
    }

    function _clearRoleForm() {
        selectedRoleIndex = -1
        isCreatingRole = false
        formDirty = false
        formRoleKey = ""
        formRoleName = ""
        formDescription = ""
        formIsSystem = false
        checkedPermissions = {}
    }

    function _beginCreateRole() {
        selectedRoleIndex = -1
        isCreatingRole = true
        formDirty = false
        formRoleKey = ""
        formRoleName = ""
        formDescription = ""
        formIsSystem = false
        checkedPermissions = {}
    }

    function _saveRole() {
        if (!formDirty)
            return
        const fields = {}
        fields["roleName"] = formRoleName
        fields["description"] = formDescription
        fields["permissions"] = _collectCheckedPerms()
        rbacServer.updateRole(formRoleKey, fields)
    }

    function _createRole() {
        if (!formRoleKey.length || !formRoleName.length)
            return
        rbacServer.createRole(formRoleKey, formRoleName, formDescription)
    }

    function _deleteRole() {
        if (!formRoleKey.length)
            return
        rbacServer.deleteRole(formRoleKey)
    }

    function _refreshRoles() {
        rbacServer.queryRoles()
        rbacServer.queryPermissions()
    }
```

- [ ] **Step 2: 修改 `Component.onCompleted`，增加权限查询**

将现有的：
```qml
    Component.onCompleted: {
        userServer.queryUsers("", "")
        rbacServer.queryRoles()
    }
```

改为：
```qml
    Component.onCompleted: {
        userServer.queryUsers("", "")
        rbacServer.queryRoles()
        rbacServer.queryPermissions()
    }
```

- [ ] **Step 3: Commit**

```bash
git add ui/pages/PageRbac.qml
git commit -m "feat(ui): add role management helper functions

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```

---

### Task 3: 重构布局 — 添加 TabBar 和 StackLayout 外壳

**Files:**
- Modify: `ui/pages/PageRbac.qml`（替换现有 `RowLayout { ... }` 顶级布局）

- [ ] **Step 1: 用 ColumnLayout + TabBar + StackLayout 包裹现有内容**

将现有的整个 `RowLayout { ... }`（从 `RowLayout {` 到对应的闭合 `}`，即用户列表 + 用户权限两个 Card）包裹在 StackLayout 的 Item 中。

具体替换：找到 `Component.onCompleted` 之后的 `RowLayout {` （大约第 79 行），将该 RowLayout 及其全部内容用以下结构包裹：

```qml
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: page.currentTab === 0 ? qsTr("用户权限") : qsTr("角色管理")
            subtitle: page.currentTab === 0 ? qsTr("分配和撤销用户角色") : qsTr("创建、修改和删除角色")
        }

        // ── Tab 切换 ──
        Row {
            spacing: 0
            Layout.fillWidth: true

            Rectangle {
                width: 140; height: 32
                radius: Theme.radiusSm
                color: page.currentTab === 0 ? Theme.accent : Theme.surfaceAlt
                Text {
                    anchors.centerIn: parent
                    text: qsTr("用户权限分配")
                    color: page.currentTab === 0 ? "#FFFFFF" : Theme.textMuted
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: page.currentTab = 0
                }
            }
            Rectangle {
                width: 100; height: 32
                radius: Theme.radiusSm
                color: page.currentTab === 1 ? Theme.accent : Theme.surfaceAlt
                Text {
                    anchors.centerIn: parent
                    text: qsTr("角色管理")
                    color: page.currentTab === 1 ? "#FFFFFF" : Theme.textMuted
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        page.currentTab = 1
                        page._refreshRoles()
                    }
                }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: page.currentTab

            // ── Tab 0: 用户权限分配（现有内容） ──
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // 【在此放置原来的 RowLayout 及其全部子内容】
                RowLayout {
                    // ... 现有的全部内容保持不变 ...
                }
            }

            // ── Tab 1: 角色管理（在后续 Task 中填充） ──
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
                // 占位，Task 4/5/6 填充
            }
        }
    }
```

> **注意：** 原有的 `BusyOverlay { ... }` 和 `Connections { ... }` 保持在 `Item { id: page }` 的顶层，不放入 ColumnLayout 内部。

- [ ] **Step 2: 编译验证**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1 | tail -20
```
预期：编译通过

- [ ] **Step 3: Commit**

```bash
git add ui/pages/PageRbac.qml
git commit -m "refactor(ui): wrap PageRbac content in TabBar + StackLayout shell

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```

---

### Task 4: 实现角色列表 Card

**Files:**
- Modify: `ui/pages/PageRbac.qml`（替换 Tab 1 的占位 Item 内容）

- [ ] **Step 1: 在 Tab 1 的 Item 中添加角色列表 ColumnLayout**

将 Tab 1 的 `Item { ... }` 替换为：

```qml
            // ── Tab 1: 角色管理 ──
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: Theme.spacingMd

                Card {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 220
                    stretchContent: true
                    title: qsTr("角色列表")

                    headerRight: Row {
                        spacing: Theme.spacingSm
                        PermissionButton {
                            sessionManager: page.sessionManager
                            requiredRole: "super_admin"
                            deniedDialog: page.deniedDialog
                            text: qsTr("新建角色")
                            highlighted: true
                            enabled: !rbacServer.busy
                            onClicked: guardedClick(page._beginCreateRole)
                        }
                        PermissionButton {
                            sessionManager: page.sessionManager
                            requiredRole: "super_admin"
                            deniedDialog: page.deniedDialog
                            text: qsTr("刷新")
                            enabled: !rbacServer.busy
                            onClicked: guardedClick(page._refreshRoles)
                        }
                    }

                    DataTable {
                        anchors.fill: parent
                        rows: rbacServer.roleRecords
                        columns: [
                            { key: "roleKey", title: qsTr("角色标识"), width: 140 },
                            { key: "roleName", title: qsTr("角色名称"), width: 120 },
                            { key: "description", title: qsTr("描述") },
                            { key: "isSystem", title: qsTr("系统角色"), width: 80,
                              formatter: function(v) { return v ? qsTr("系统") : "—" } },
                            { key: "createdAt", title: qsTr("创建时间"), width: 160 }
                        ]
                        onRowClicked: function(idx, row) {
                            page.selectedRoleIndex = idx
                            page._loadRoleIntoForm(row)
                        }
                    }
                }
```

- [ ] **Step 2: 编译验证**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1 | tail -20
```

- [ ] **Step 3: Commit**

```bash
git add ui/pages/PageRbac.qml
git commit -m "feat(ui): add role list DataTable to role management tab

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```

---

### Task 5: 实现角色详情 Card（表单 + 权限网格 + 操作按钮）

**Files:**
- Modify: `ui/pages/PageRbac.qml`（在角色列表 Card 下方继续添加）

- [ ] **Step 1: 在角色列表 Card 之后、ColumnLayout 闭合之前插入角色详情 Card**

在第 220 行高度的角色列表 Card 闭合 `}` 之后，添加：

```qml
                // ── 空状态提示 ──
                Card {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: page.selectedRoleIndex < 0 && !page.isCreatingRole
                    stretchContent: true
                    Label {
                        anchors.centerIn: parent
                        text: qsTr("请在上方选择一个角色以查看和编辑其详细信息")
                        color: Theme.textSubtle
                        font.pixelSize: Theme.fontMd
                        font.family: Theme.fontFamily
                    }
                }

                // ── 角色详情（选中或新建） ──
                Card {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: page.selectedRoleIndex >= 0 || page.isCreatingRole
                    stretchContent: true

                    title: page.isCreatingRole
                           ? qsTr("新建角色")
                           : qsTr("角色详情") + " · " + page.formRoleKey

                    headerRight: Row {
                        spacing: Theme.spacingSm

                        PermissionButton {
                            visible: !page.isCreatingRole
                            sessionManager: page.sessionManager
                            requiredRole: "super_admin"
                            deniedDialog: page.deniedDialog
                            text: qsTr("保存修改")
                            highlighted: true
                            enabled: page.formDirty && !rbacServer.busy
                            onClicked: guardedClick(page._saveRole)
                        }
                        PermissionButton {
                            sessionManager: page.sessionManager
                            requiredRole: "super_admin"
                            deniedDialog: page.deniedDialog
                            text: qsTr("创建角色")
                            highlighted: true
                            visible: page.isCreatingRole
                            enabled: page.formRoleKey.length > 0
                                     && page.formRoleName.length > 0
                                     && !rbacServer.busy
                            onClicked: guardedClick(page._createRole)
                        }
                        PermissionButton {
                            sessionManager: page.sessionManager
                            requiredRole: "super_admin"
                            deniedDialog: page.deniedDialog
                            text: qsTr("取消")
                            flat: true
                            visible: page.isCreatingRole
                            onClicked: guardedClick(page._clearRoleForm)
                        }
                        PermissionButton {
                            visible: !page.isCreatingRole && !page.formIsSystem
                            sessionManager: page.sessionManager
                            requiredRole: "super_admin"
                            deniedDialog: page.deniedDialog
                            text: qsTr("删除角色")
                            enabled: !rbacServer.busy
                            onClicked: guardedClick(function() { roleDeleteConfirm.open() })
                        }
                    }

                    Flickable {
                        anchors.fill: parent
                        contentHeight: detailColumn.implicitHeight
                        clip: true
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                        ColumnLayout {
                            id: detailColumn
                            width: parent.width
                            spacing: Theme.spacingMd

                            // ── 表单字段 ──
                            GridLayout {
                                Layout.fillWidth: true
                                columns: 3
                                rowSpacing: Theme.spacingSm
                                columnSpacing: Theme.spacingMd

                                LabeledField {
                                    label: qsTr("角色标识")
                                    Layout.fillWidth: true
                                    TextField {
                                        id: roleKeyField
                                        text: page.formRoleKey
                                        readOnly: !page.isCreatingRole
                                        enabled: page.isCreatingRole
                                        Layout.fillWidth: true
                                        onTextChanged: {
                                            if (page.isCreatingRole) {
                                                page.formRoleKey = text
                                                page.formDirty = true
                                            }
                                        }
                                    }
                                }
                                LabeledField {
                                    label: qsTr("角色名称")
                                    Layout.fillWidth: true
                                    TextField {
                                        id: roleNameField
                                        text: page.formRoleName
                                        Layout.fillWidth: true
                                        onTextChanged: {
                                            page.formRoleName = text
                                            if (!page.isCreatingRole)
                                                page.formDirty = true
                                        }
                                    }
                                }
                                LabeledField {
                                    label: qsTr("描述")
                                    Layout.fillWidth: true
                                    TextField {
                                        id: roleDescField
                                        text: page.formDescription
                                        Layout.fillWidth: true
                                        onTextChanged: {
                                            page.formDescription = text
                                            if (!page.isCreatingRole)
                                                page.formDirty = true
                                        }
                                    }
                                }
                            }

                            // ── 权限分配 ──
                            Label {
                                text: qsTr("权限分配")
                                color: Theme.text
                                font.pixelSize: Theme.fontLg
                                font.family: Theme.fontFamily
                                font.bold: true
                                Layout.fillWidth: true
                            }

                            Flow {
                                Layout.fillWidth: true
                                spacing: Theme.spacingMd

                                Repeater {
                                    model: page.permissionGroups
                                    delegate: Rectangle {
                                        required property var modelData
                                        width: 200
                                        height: permColumn.implicitHeight + Theme.spacingSm * 2
                                        color: Theme.surfaceAlt
                                        radius: Theme.radiusSm
                                        border.color: Theme.border
                                        border.width: 1

                                        Column {
                                            id: permColumn
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.top: parent.top
                                            anchors.margins: Theme.spacingSm
                                            spacing: 2

                                            Label {
                                                text: modelData.name
                                                color: Theme.text
                                                font.pixelSize: Theme.fontSm
                                                font.family: Theme.fontFamily
                                                font.bold: true
                                                width: parent.width
                                            }

                                            Repeater {
                                                model: modelData.perms
                                                delegate: CheckBox {
                                                    required property var modelData
                                                    text: modelData.label || modelData.key
                                                    font.pixelSize: Theme.fontXs
                                                    font.family: Theme.fontFamily
                                                    checked: page._permChecked(modelData.key)
                                                    onCheckedChanged: page._setPerm(modelData.key, checked)
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // ── 删除确认对话框 ──
                ConfirmDialog {
                    id: roleDeleteConfirm
                    message: qsTr("确认删除角色 ") + page.formRoleKey + "？\n" +
                             qsTr("删除后，已分配该角色的用户将失去对应权限。")
                    onAccepted: page._deleteRole()
                }
```

- [ ] **Step 2: 编译验证**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1 | tail -20
```

- [ ] **Step 3: Commit**

```bash
git add ui/pages/PageRbac.qml
git commit -m "feat(ui): add role detail form with permission checkbox grid

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```

---

### Task 6: 扩展 Connections 处理角色 CRUD 信号

**Files:**
- Modify: `ui/pages/PageRbac.qml`（在现有 `Connections { target: rbacServer ... }` 块中扩展）

- [ ] **Step 1: 在现有 rbacServer Connections 中扩展 onOperationSucceeded 和 onOperationFailed 处理**

将现有的 rbacServer Connections 块中 `onOperationSucceeded` 函数替换为：

```qml
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
            if (apiType === "user.role.assign" || apiType === "user.role.revoke") {
                rbacServer.queryUserRoles(page.selectedUserId)
            }
            // ── 角色 CRUD 处理 ──
            if (apiType === "role.create") {
                // 新建角色后如有权限选择，调用 updateRole 设置权限
                const perms = page._collectCheckedPerms()
                if (perms.length > 0) {
                    const fields = {}
                    fields["permissions"] = perms
                    rbacServer.updateRole(page.formRoleKey, fields)
                } else {
                    page._clearRoleForm()
                    page._refreshRoles()
                }
            }
            if (apiType === "role.update") {
                page.formDirty = false
                page._refreshRoles()
                if (page.isCreatingRole) {
                    // 新建角色后的权限更新完成
                    page._clearRoleForm()
                }
            }
            if (apiType === "role.delete") {
                page._clearRoleForm()
                page._refreshRoles()
            }
        }
```

在 `onOperationFailed` 中添加（保持现有内容，追加角色相关处理）：

```qml
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
            // 角色创建失败不清空表单，保留用户输入
            if (apiType === "role.create") {
                // 保留表单，不操作
            }
        }
```

- [ ] **Step 2: 编译验证**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1 | tail -20
```

- [ ] **Step 3: Commit**

```bash
git add ui/pages/PageRbac.qml
git commit -m "feat(ui): wire up role CRUD signals with Connections handlers

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```

---

### Task 7: 最终验证和启动测试

**Files:** 无新文件

- [ ] **Step 1: 完整编译**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1
```
预期：编译通过，无错误无警告

- [ ] **Step 2: 检查 QML 语法**

确认以下要点：
- 所有 `{` `}` 括号匹配
- `Repeater` 的 `delegate` 中正确使用 `required property var modelData`
- `Connections` 中的函数签名与原信号匹配
- `PermissionButton.guardedClick` 回调函数正确

- [ ] **Step 3: 功能验证清单**

启动应用后手动验证：
1. 以 super_admin 登录 → 导航到"用户权限"页面
2. 确认可见两个 Tab："用户权限分配" 和 "角色管理"
3. 切换到"角色管理" → 角色列表加载正常
4. 点击角色列表中的某行 → 详情表单显示正确数据，权限复选框正确勾选
5. 修改角色名称/描述/权限 → "保存修改"按钮激活 → 点击保存 → 列表刷新
6. 系统角色（如 super_admin）→ 角色标识字段只读，删除按钮隐藏
7. 点击"新建角色" → 空表单显示 → 填写标识/名称/描述 + 勾选权限 → 点击"创建角色"
8. 新建成功后自动刷新列表，表单清空
9. 自定义角色 → 点击"删除角色" → ConfirmDialog → 确认 → 列表刷新
10. 切换回"用户权限分配" Tab → 原有用户-角色分配功能正常

- [ ] **Step 4: Commit 最终版本**

```bash
git add ui/pages/PageRbac.qml
git commit -m "feat(ui): complete role management page with create/delete/permission editing

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>"
```
