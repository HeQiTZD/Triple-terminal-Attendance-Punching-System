import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var deviceServer
    required property var sessionManager
    property var deniedDialog: null
    signal serviceResult(string apiType, int code, string message)

    readonly property bool canUpdate: sessionManager && sessionManager.hasPermission("device.update")

    function _query() {
        deviceServer.queryDevices(dId.text.trim(), dName.text.trim(), dIp.text.trim())
    }

    function _statusText() {
        return dStatus.currentText || "offline"
    }

    function _setStatusFromRow(s) {
        const i = dStatus.model.indexOf(s)
        dStatus.currentIndex = i >= 0 ? i : 0
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("设备管理")
            subtitle: qsTr("注册 / 修改设备 · 状态查看 · 远程指令")
            actions: [
                PermissionButton {
                    sessionManager: page.sessionManager
                    requiredPermission: "device.read"
                    deniedDialog: page.deniedDialog
                    text: qsTr("刷新")
                    enabled: !deviceServer.busy
                    onClicked: guardedClick(page._query)
                }
            ]
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("设备信息")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    rowSpacing: Theme.spacingSm
                    columnSpacing: Theme.spacingMd

                    LabeledField { label: qsTr("设备 ID"); Layout.fillWidth: true
                        TextField {
                            id: dId
                            readOnly: !page.canUpdate
                            placeholderText: qsTr("DEV001")
                            Layout.fillWidth: true
                        }
                    }
                    LabeledField { label: qsTr("名称"); Layout.fillWidth: true
                        TextField {
                            id: dName
                            readOnly: !page.canUpdate
                            Layout.fillWidth: true
                        }
                    }
                    LabeledField { label: qsTr("IP"); Layout.fillWidth: true
                        TextField {
                            id: dIp
                            readOnly: !page.canUpdate
                            Layout.fillWidth: true
                        }
                    }
                    LabeledField { label: qsTr("状态"); Layout.fillWidth: true
                        ComboBox {
                            id: dStatus
                            enabled: page.canUpdate
                            model: ["online", "offline", "maintenance"]
                            Layout.fillWidth: true
                        }
                    }
                }

                Row {
                    spacing: Theme.spacingSm
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "device.create"
                        deniedDialog: page.deniedDialog
                        text: qsTr("新增设备")
                        highlighted: true
                        enabled: !deviceServer.busy
                        onClicked: guardedClick(function() {
                            deviceServer.createDevice(dId.text.trim(), dName.text.trim(),
                                                      dIp.text.trim(), page._statusText())
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "device.update"
                        deniedDialog: page.deniedDialog
                        text: qsTr("修改设备")
                        enabled: !deviceServer.busy
                        onClicked: guardedClick(function() {
                            deviceServer.updateDevice(dId.text.trim(), dName.text.trim(),
                                                      dIp.text.trim(), page._statusText())
                        })
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "device.read"
                        deniedDialog: page.deniedDialog
                        text: qsTr("查询")
                        enabled: !deviceServer.busy
                        onClicked: guardedClick(page._query)
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "device.delete"
                        deniedDialog: page.deniedDialog
                        text: qsTr("删除")
                        enabled: !deviceServer.busy
                        onClicked: guardedClick(function() { confirm.open() })
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            visible: sessionManager && sessionManager.hasPermission("device.command")
            title: qsTr("设备指令")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingMd
                    LabeledField {
                        label: qsTr("指令")
                        Layout.preferredWidth: 200
                        TextField {
                            id: cmdName
                            placeholderText: qsTr("reboot")
                            Layout.fillWidth: true
                        }
                    }
                    PermissionButton {
                        sessionManager: page.sessionManager
                        requiredPermission: "device.command"
                        deniedDialog: page.deniedDialog
                        text: qsTr("发送指令")
                        highlighted: true
                        enabled: !deviceServer.busy && dId.text.trim().length > 0
                        onClicked: guardedClick(function() {
                            deviceServer.sendCommand(dId.text.trim(), cmdName.text.trim(), cmdParams.text)
                        })
                    }
                }

                Label {
                    text: qsTr("params（JSON 对象，可为空 {}）")
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontXs
                }

                JsonEditor {
                    id: cmdParams
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    text: "{}"
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("设备列表")

            DataTable {
                anchors.fill: parent
                rows: deviceServer.records
                columns: [
                    { key: "id", title: "ID", width: 60, align: "right" },
                    { key: "deviceId", title: qsTr("设备 ID"), width: 130 },
                    { key: "deviceName", title: qsTr("名称"), width: 160 },
                    { key: "ipAddress", title: qsTr("IP"), width: 130 },
                    { key: "status", title: qsTr("状态"), width: 100 },
                    { key: "lastOnline", title: qsTr("最近在线") }
                ]
                onRowClicked: function(idx, row) {
                    if (!page.canUpdate) return
                    dId.text = row.deviceId || ""
                    dName.text = row.deviceName || ""
                    dIp.text = row.ipAddress || ""
                    page._setStatusFromRow(row.status || "offline")
                }
            }
        }
    }

    BusyOverlay { busy: deviceServer.busy }

    ConfirmDialog {
        id: confirm
        message: qsTr("确认删除设备 ") + dId.text + "？"
        onAccepted: deviceServer.deleteDevice(dId.text.trim())
    }

    Connections {
        target: deviceServer
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
