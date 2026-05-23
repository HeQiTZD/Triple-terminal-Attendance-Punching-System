import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var userServer
    required property var sessionManager
    property var deniedDialog: null
    signal serviceResult(string apiType, int code, string message)

    property string passwordHint: ""

    readonly property bool formReadOnly: !sessionManager
        || (!PermissionCatalog.hasPerm(sessionManager, "user.update")
            && !PermissionCatalog.hasPerm(sessionManager, "user.create"))  // viewer：只读

    function _query() {
        userServer.queryUsers(uEmp.text.trim(), "")
    }

    function _passwordError(pw, required) {
        if (!required && !pw.length)
            return ""
        return PermissionCatalog.validatePassword(pw)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("用户账号")
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("账号信息")

            GridLayout {
                width: parent.width
                columns: 3
                rowSpacing: Theme.spacingSm
                columnSpacing: Theme.spacingMd

                LabeledField { label: qsTr("工号"); Layout.fillWidth: true
                    TextField {
                        id: uEmp
                        readOnly: page.formReadOnly
                        Layout.fillWidth: true
                    }
                }
                LabeledField { label: qsTr("显示名称"); Layout.fillWidth: true
                    TextField {
                        id: uName
                        readOnly: page.formReadOnly
                        Layout.fillWidth: true
                    }
                }
                LabeledField { label: qsTr("密码"); Layout.fillWidth: true
                    TextField {
                        id: uPwd
                        echoMode: TextInput.Password
                        placeholderText: qsTr("留空表示不修改")
                        readOnly: page.formReadOnly
                        Layout.fillWidth: true
                    }
                }

                Label {
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    visible: page.passwordHint.length > 0
                    text: page.passwordHint
                    color: Theme.danger
                    font.pixelSize: Theme.fontSm
                }

                Row {
                    Layout.columnSpan: 3
                    spacing: Theme.spacingSm

                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "user.create"
                        deniedDialog: page.deniedDialog
                        text: qsTr("新增")
                        highlighted: true
                        enabled: !userServer.busy
                        onClicked: guardedClick(function() {
                            const err = page._passwordError(uPwd.text, true)
                            if (err.length) { page.passwordHint = err; return }
                            page.passwordHint = ""
                            userServer.createUser(uEmp.text.trim(), uPwd.text, uName.text.trim())
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "user.update"
                        deniedDialog: page.deniedDialog
                        text: qsTr("修改")
                        enabled: !userServer.busy
                        onClicked: guardedClick(function() {
                            if (!uPwd.text.length && !uName.text.trim().length) {
                                page.passwordHint = qsTr("请填写新名称或新密码")
                                return
                            }
                            const err = page._passwordError(uPwd.text, false)
                            if (err.length) { page.passwordHint = err; return }
                            page.passwordHint = ""
                            userServer.updateUser(uEmp.text.trim(), uName.text.trim(), uPwd.text)
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "user.delete"
                        deniedDialog: page.deniedDialog
                        text: qsTr("删除")
                        enabled: !userServer.busy
                        onClicked: guardedClick(function() { confirm.open() })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "user.read"
                        deniedDialog: page.deniedDialog
                        text: qsTr("查询")
                        enabled: !userServer.busy
                        onClicked: guardedClick(page._query)
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "user.read"
                        deniedDialog: page.deniedDialog
                        text: qsTr("清空表单")
                        flat: true
                        onClicked: guardedClick(function() {
                            uEmp.text = ""; uName.text = ""; uPwd.text = ""; page.passwordHint = ""
                        })
                    }
                }
            }

        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("用户列表")

            DataTable {
                anchors.fill: parent
                rows: userServer.records
                columns: [
                    { key: "id", title: "ID", width: 60, align: "right" },
                    { key: "employeeId", title: qsTr("工号"), width: 120 },
                    { key: "name", title: qsTr("名称"), width: 140 },
                    { key: "createdAt", title: qsTr("创建时间"), width: 170 },
                    { key: "lastLoginTime", title: qsTr("最近登录") }
                ]
                onRowClicked: function(idx, row) {
                    if (!PermissionCatalog.hasPerm(sessionManager, "user.update"))
                        return
                    uEmp.text = row.employeeId || ""
                    uName.text = row.name || ""
                    uPwd.text = ""
                    page.passwordHint = ""
                }
            }
        }
    }

    BusyOverlay { busy: userServer.busy }

    ConfirmDialog {
        id: confirm
        message: qsTr("确认删除用户 ") + uEmp.text + "？"
        onAccepted: userServer.deleteUser(uEmp.text.trim())
    }

    Connections {
        target: userServer
        function onOperationSucceeded(apiType, message) {
            page.serviceResult(apiType, 0, message)
            if (apiType.indexOf("create") >= 0 || apiType.indexOf("update") >= 0
                    || apiType.indexOf("delete") >= 0)
                page._query()
        }
        function onOperationFailed(apiType, code, message) {
            page.serviceResult(apiType, code, message)
        }
    }
}
