import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var rbacServer
    required property var sessionManager
    property var deniedDialog: null
    signal serviceResult(string apiType, int code, string message)

    property string selectedRoleKey: ""
    property var selectedPermKeys: ({})

    function _permChecked(key) {
        return !!selectedPermKeys[key]
    }

    function _setPerm(key, on) {
        const next = Object.assign({}, selectedPermKeys)
        if (on)
            next[key] = true
        else
            delete next[key]
        selectedPermKeys = next
    }

    function _collectPerms() {
        const list = []
        for (const k in selectedPermKeys)
            if (selectedPermKeys[k])
                list.push(k)
        return list
    }

    function _loadRolePerms(row) {
        selectedPermKeys = {}
        if (!row || !row.permissions)
            return
        const arr = row.permissions
        const next = {}
        if (Array.isArray(arr)) {
            for (let i = 0; i < arr.length; ++i)
                next[arr[i]] = true
        }
        selectedPermKeys = next
    }

    Component.onCompleted: {
        rbacServer.queryRoles()
        rbacServer.queryPermissions()
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        Card {
            Layout.preferredWidth: 320
            Layout.fillHeight: true
            title: qsTr("角色列表")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                Row {
                    spacing: Theme.spacingSm
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredRole: "super_admin"
                        deniedDialog: page.deniedDialog
                        text: qsTr("刷新")
                        enabled: !rbacServer.busy
                        onClicked: guardedClick(function() { rbacServer.queryRoles() })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredRole: "super_admin"
                        deniedDialog: page.deniedDialog
                        text: qsTr("新建")
                        enabled: !rbacServer.busy
                        onClicked: guardedClick(function() {
                            roleDlg.mode = "create"
                            roleDlg.open()
                        })
                    }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: rbacServer.roleRecords
                    delegate: Rectangle {
                        required property int index
                        required property var modelData
                        width: ListView.view.width
                        height: 40
                        color: page.selectedRoleKey === modelData.roleKey
                               ? Theme.selected : (ma.containsMouse ? Theme.hover : "transparent")
                        radius: Theme.radiusSm

                        Label {
                            anchors.left: parent.left
                            anchors.leftMargin: Theme.spacingSm
                            anchors.verticalCenter: parent.verticalCenter
                            text: (modelData.roleName || modelData.roleKey || "")
                            color: Theme.text
                            font.pixelSize: Theme.fontSm
                        }

                        MouseArea {
                            id: ma
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                page.selectedRoleKey = modelData.roleKey || ""
                                roleKeyField.text = modelData.roleKey || ""
                                roleNameField.text = modelData.roleName || ""
                                roleDescField.text = modelData.description || ""
                                page._loadRolePerms(modelData)
                            }
                        }
                    }
                }

                PermissionButton {
                    sessionManager: page.sessionManager
                    requiredRole: "super_admin"
                    deniedDialog: page.deniedDialog
                    text: qsTr("删除角色")
                    enabled: page.selectedRoleKey.length > 0 && !rbacServer.busy
                    onClicked: guardedClick(function() { delRoleConfirm.open() })
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("权限与用户角色")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingMd

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    rowSpacing: Theme.spacingSm
                    columnSpacing: Theme.spacingMd

                    LabeledField { label: qsTr("roleKey"); Layout.fillWidth: true
                        TextField { id: roleKeyField; readOnly: true; Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("roleName"); Layout.fillWidth: true
                        TextField { id: roleNameField; Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("描述"); Layout.columnSpan: 2; Layout.fillWidth: true
                        TextField { id: roleDescField; Layout.fillWidth: true }
                    }
                }

                PermissionButton {
                    sessionManager: page.sessionManager
                    requiredRole: "super_admin"
                    deniedDialog: page.deniedDialog
                    text: qsTr("保存角色权限")
                    highlighted: true
                    enabled: page.selectedRoleKey.length > 0 && !rbacServer.busy
                    onClicked: guardedClick(function() {
                        const fields = {
                            roleName: roleNameField.text.trim(),
                            description: roleDescField.text.trim(),
                            permissions: page._collectPerms()
                        }
                        rbacServer.updateRole(page.selectedRoleKey, fields)
                    })
                }

                Label {
                    text: qsTr("权限列表（勾选后保存）")
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSm
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    clip: true
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    Flow {
                        width: parent.width
                        spacing: Theme.spacingXs

                        Repeater {
                            model: rbacServer.permissionRecords
                            delegate: CheckBox {
                                required property var modelData
                                text: modelData.permKey || ""
                                checked: page._permChecked(modelData.permKey || "")
                                onCheckedChanged: page._setPerm(modelData.permKey || "", checked)
                            }
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingMd

                    LabeledField {
                        label: qsTr("用户 ID")
                        Layout.preferredWidth: 120
                        SpinBox {
                            id: userIdSpin
                            from: 1
                            to: 999999
                            editable: true
                            value: 1
                        }
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "user.read"
                        deniedDialog: page.deniedDialog
                        text: qsTr("查询用户角色")
                        enabled: !rbacServer.busy
                        onClicked: guardedClick(function() {
                            rbacServer.queryUserRoles(userIdSpin.value)
                        })
                    }
                }

                Label {
                    text: qsTr("当前角色：") + rbacServer.userRoleKeys.join(", ")
                    color: Theme.text
                    font.pixelSize: Theme.fontSm
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Row {
                    spacing: Theme.spacingSm
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredRole: "super_admin"
                        deniedDialog: page.deniedDialog
                        text: qsTr("分配角色")
                        enabled: page.selectedRoleKey.length > 0 && !rbacServer.busy
                        onClicked: guardedClick(function() {
                            rbacServer.assignUserRole(userIdSpin.value, page.selectedRoleKey)
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredRole: "super_admin"
                        deniedDialog: page.deniedDialog
                        text: qsTr("撤销角色")
                        enabled: page.selectedRoleKey.length > 0 && !rbacServer.busy
                        onClicked: guardedClick(function() {
                            rbacServer.revokeUserRole(userIdSpin.value, page.selectedRoleKey)
                        })
                    }
                }
            }
        }
    }

    BusyOverlay { busy: rbacServer.busy }

    ConfirmDialog {
        id: delRoleConfirm
        message: qsTr("确认删除角色 ") + page.selectedRoleKey + "？"
        onAccepted: rbacServer.deleteRole(page.selectedRoleKey)
    }

    Dialog {
        id: roleDlg
        modal: true
        anchors.centerIn: Overlay.overlay
        title: mode === "create" ? qsTr("新建角色") : qsTr("编辑角色")
        standardButtons: Dialog.Ok | Dialog.Cancel
        property string mode: "create"

        property string inpKey: ""
        property string inpName: ""
        property string inpDesc: ""

        onAccepted: {
            if (mode === "create")
                rbacServer.createRole(inpKey, inpName, inpDesc)
        }

        contentItem: ColumnLayout {
            spacing: Theme.spacingSm
            TextField {
                placeholderText: qsTr("roleKey")
                text: roleDlg.inpKey
                onTextChanged: roleDlg.inpKey = text
                Layout.fillWidth: true
            }
            TextField {
                placeholderText: qsTr("roleName")
                text: roleDlg.inpName
                onTextChanged: roleDlg.inpName = text
                Layout.fillWidth: true
            }
            TextField {
                placeholderText: qsTr("description")
                text: roleDlg.inpDesc
                onTextChanged: roleDlg.inpDesc = text
                Layout.fillWidth: true
            }
        }
    }

    Connections {
        target: rbacServer
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
            if (apiType.indexOf("role.") === 0)
                rbacServer.queryRoles()
            if (apiType.indexOf("user.role") === 0)
                rbacServer.queryUserRoles(userIdSpin.value)
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
    }
}
