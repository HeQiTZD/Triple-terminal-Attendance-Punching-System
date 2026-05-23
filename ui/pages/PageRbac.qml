import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var rbacServer
    required property var userServer
    required property var sessionManager
    property var deniedDialog: null
    signal serviceResult(string apiType, int code, string message)

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

    property int selectedUserId: -1
    property string selectedUserName: ""
    property string selectedUserEmployeeId: ""
    property var selectedRoles: ({})
    property var originalRoles: ({})
    property bool rolesDirty: false

    function _roleChecked(roleKey) {
        return !!selectedRoles[roleKey]
    }

    function _setRole(roleKey, on) {
        if (page.selectedUserId < 0)
            return
        if (!!selectedRoles[roleKey] === on)
            return
        const next = Object.assign({}, selectedRoles)
        if (on)
            next[roleKey] = true
        else
            delete next[roleKey]
        selectedRoles = next
        rolesDirty = true
    }

    function _loadUserRoles(roleKeys) {
        const next = {}
        const orig = {}
        if (Array.isArray(roleKeys)) {
            for (let i = 0; i < roleKeys.length; ++i) {
                next[roleKeys[i]] = true
                orig[roleKeys[i]] = true
            }
        }
        selectedRoles = next
        originalRoles = orig
        rolesDirty = false
    }

    function _saveRoles() {
        if (page.selectedUserId < 0)
            return
        const allKeys = {}
        for (const k in selectedRoles) { if (selectedRoles[k]) allKeys[k] = true }
        for (const k in originalRoles) { if (originalRoles[k]) allKeys[k] = true }

        const keys = Object.keys(allKeys)
        for (let i = 0; i < keys.length; ++i) {
            const key = keys[i]
            const was = !!originalRoles[key]
            const now = !!selectedRoles[key]
            if (now && !was)
                rbacServer.assignUserRole(page.selectedUserId, key)
            else if (!now && was)
                rbacServer.revokeUserRole(page.selectedUserId, key)
        }
    }

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

    Component.onCompleted: {
        userServer.queryUsers("", "")
        rbacServer.queryRoles()
        rbacServer.queryPermissions()
    }

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

            // ── Tab 0: 用户权限分配 ──
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingMd

                    Card {
                        Layout.preferredWidth: 320
                        Layout.fillHeight: true
                        stretchContent: true
                        title: qsTr("用户列表")

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: Theme.spacingSm

                            PermissionButton {
                                sessionManager: page.sessionManager
                                requiredPermission: "user.read"
                                deniedDialog: page.deniedDialog
                                text: qsTr("刷新")
                                enabled: !userServer.busy
                                onClicked: guardedClick(function() { userServer.queryUsers("", "") })
                            }

                            ListView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
                                model: userServer.records
                                ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }
                                delegate: Rectangle {
                                    required property int index
                                    required property var modelData
                                    width: ListView.view.width
                                    height: 40
                                    color: page.selectedUserId === (modelData.id != null ? modelData.id : -1)
                                           ? Theme.selected : (ma.containsMouse ? Theme.hover : "transparent")
                                    radius: Theme.radiusSm

                                    Column {
                                        anchors.left: parent.left
                                        anchors.leftMargin: Theme.spacingSm
                                        anchors.verticalCenter: parent.verticalCenter

                                        Label {
                                            text: (modelData.name || modelData.employeeId || "")
                                            color: Theme.text
                                            font.pixelSize: Theme.fontSm
                                        }
                                        Label {
                                            text: modelData.employeeId || ""
                                            color: Theme.textMuted
                                            font.pixelSize: Theme.fontXs
                                        }
                                    }

                                    MouseArea {
                                        id: ma
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            page.selectedUserId = modelData.id != null ? modelData.id : -1
                                            page.selectedUserName = modelData.name || ""
                                            page.selectedUserEmployeeId = modelData.employeeId || ""
                                            rbacServer.queryUserRoles(page.selectedUserId)
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Card {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        stretchContent: true
                        title: qsTr("用户权限")

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: Theme.spacingMd

                            Label {
                                visible: page.selectedUserId < 0
                                text: qsTr("请在左侧选择一个用户")
                                color: Theme.textMuted
                                font.pixelSize: Theme.fontMd
                                font.family: Theme.fontFamily
                                Layout.fillWidth: true
                            }

                            GridLayout {
                                visible: page.selectedUserId >= 0
                                Layout.fillWidth: true
                                columns: 2
                                rowSpacing: Theme.spacingSm
                                columnSpacing: Theme.spacingMd

                                LabeledField {
                                    label: qsTr("用户 ID")
                                    Layout.fillWidth: true
                                    Label {
                                        text: page.selectedUserEmployeeId || "—"
                                        color: Theme.text
                                        font.pixelSize: Theme.fontSm
                                        font.family: Theme.fontFamily
                                        Layout.fillWidth: true
                                    }
                                }
                                LabeledField {
                                    label: qsTr("姓名")
                                    Layout.fillWidth: true
                                    Label {
                                        text: page.selectedUserName || "—"
                                        color: Theme.text
                                        font.pixelSize: Theme.fontSm
                                        font.family: Theme.fontFamily
                                        Layout.fillWidth: true
                                    }
                                }
                            }

                            Label {
                                visible: page.selectedUserId >= 0
                                text: {
                                    const keys = Object.keys(selectedRoles).filter(k => selectedRoles[k])
                                    if (keys.length === 0)
                                        return qsTr("当前角色：无")
                                    const names = keys.map(k => {
                                        const rec = rbacServer.roleRecords
                                        for (let i = 0; i < rec.length; ++i) {
                                            if (rec[i].roleKey === k)
                                                return rec[i].roleName || k
                                        }
                                        return k
                                    })
                                    return qsTr("当前角色：") + names.join("、")
                                }
                                color: Theme.text
                                font.pixelSize: Theme.fontSm
                                font.family: Theme.fontFamily
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }

                            Label {
                                visible: page.selectedUserId >= 0
                                text: qsTr("角色列表")
                                color: Theme.textMuted
                                font.pixelSize: Theme.fontSm
                                font.family: Theme.fontFamily
                            }

                            ScrollView {
                                visible: page.selectedUserId >= 0
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
                                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                                Flow {
                                    width: parent.width
                                    spacing: Theme.spacingXs

                                    Repeater {
                                        model: rbacServer.roleRecords
                                        delegate: CheckBox {
                                            required property var modelData
                                            text: {
                                                const name = modelData.roleName || ""
                                                const key = modelData.roleKey || ""
                                                if (name && key)
                                                    return name + " (" + key + ")"
                                                return name || key
                                            }
                                            checked: page._roleChecked(modelData.roleKey || "")
                                            onCheckedChanged: page._setRole(modelData.roleKey || "", checked)
                                        }
                                    }
                                }
                            }

                            PermissionButton {
                                visible: page.selectedUserId >= 0
                                sessionManager: page.sessionManager
                                requiredRole: "super_admin"
                                deniedDialog: page.deniedDialog
                                text: qsTr("保存角色变更")
                                highlighted: true
                                enabled: page.rolesDirty && !rbacServer.busy
                                onClicked: guardedClick(function() { page._saveRoles() })
                            }
                        }
                    }
                }
            }

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
            }
        }
    }

    BusyOverlay { busy: rbacServer.busy || userServer.busy }

    Connections {
        target: rbacServer
        function onUserRoleKeysChanged() {
            page._loadUserRoles(rbacServer.userRoleKeys)
        }
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
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
    }
}
