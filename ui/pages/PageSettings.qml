import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    property var sessionManager
    property var deniedDialog: null

    property bool showPermDetails: false

    readonly property bool canViewAuthDetails: {
        if (sessionManager) {
            const _p = sessionManager.permissions
            const _r = sessionManager.roles
            void _p
            void _r
        }
        return PermissionCatalog.canViewFullPermissions(sessionManager)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("配置预设")
            subtitle: qsTr("服务器连接 / 默认值 · 自动持久化到本机配置")
            actions: [
                Button {
                    text: qsTr("恢复默认")
                    onClicked: { Presets.reset(); Logger.info("已恢复默认配置") }
                },
                Button {
                    text: qsTr("应用 & 通知")
                    highlighted: true
                    onClicked: {
                        Presets.notifyChanged()
                        Logger.ok("已保存预设到本机注册表")
                    }
                }
            ]
        }

        ScrollView {
            id: settingsScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: Theme.spacingMd

                Card {
                    Layout.fillWidth: true
                    title: qsTr("会话")

                    RowLayout {
                        width: parent.width
                        spacing: Theme.spacingMd

                        Label {
                            text: sessionManager && sessionManager.isLoggedIn
                                  ? (qsTr("已登录：") + (sessionManager.currentUsername || ""))
                                  : qsTr("未登录")
                            color: Theme.text
                            font.pixelSize: Theme.fontMd
                            Layout.fillWidth: true
                        }
                        Button {
                            visible: sessionManager && sessionManager.isLoggedIn
                            text: qsTr("退出登录")
                            onClicked: sessionManager.logout()
                        }
                        Button {
                            visible: sessionManager && sessionManager.isLoggedIn
                            text: qsTr("刷新权限")
                            onClicked: sessionManager.refreshPermissions()
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    visible: sessionManager && sessionManager.isLoggedIn
                    title: qsTr("认证信息")

                    ColumnLayout {
                        width: parent.width
                        spacing: Theme.spacingSm

                        Label {
                            text: qsTr("角色：") + PermissionCatalog.formatRoles(sessionManager)
                            color: Theme.text
                            font.pixelSize: Theme.fontSm
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        Label {
                            visible: !page.canViewAuthDetails
                            text: qsTr("详细权限列表仅对拥有「查询用户」权限或超级管理员可见。")
                            color: Theme.textMuted
                            font.pixelSize: Theme.fontSm
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        Label {
                            visible: page.canViewAuthDetails
                            text: qsTr("权限数：") + sessionManager.permissions.length
                            color: Theme.textMuted
                            font.pixelSize: Theme.fontSm
                        }

                        Button {
                            visible: page.canViewAuthDetails
                            text: page.showPermDetails ? qsTr("收起权限列表") : qsTr("展开权限列表")
                            flat: true
                            onClicked: page.showPermDetails = !page.showPermDetails
                        }

                        PermissionChipRow {
                            visible: page.canViewAuthDetails && page.showPermDetails
                            sessionManager: page.sessionManager
                            maxVisible: 64
                            showOverflow: false
                            Layout.fillWidth: true
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    title: qsTr("服务器连接")

                    GridLayout {
                        width: parent.width
                        columns: 2
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("服务器地址"); Layout.fillWidth: true
                            TextField { text: Presets.serverHost; Layout.fillWidth: true; onEditingFinished: Presets.serverHost = text }
                        }
                        LabeledField { label: qsTr("端口"); Layout.fillWidth: true
                            SpinBox {
                                from: 1; to: 65535; editable: true
                                value: Presets.serverPort
                                Layout.fillWidth: true
                                onValueModified: Presets.serverPort = value
                            }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    title: qsTr("默认值")

                    GridLayout {
                        width: parent.width
                        columns: 2
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("默认设备 ID"); Layout.fillWidth: true
                            TextField { text: Presets.defaultDeviceId; Layout.fillWidth: true; onEditingFinished: Presets.defaultDeviceId = text }
                        }
                        LabeledField { label: qsTr("默认工号"); Layout.fillWidth: true
                            TextField { text: Presets.defaultEmployeeId; Layout.fillWidth: true; onEditingFinished: Presets.defaultEmployeeId = text }
                        }
                        LabeledField { label: qsTr("默认起始时间"); Layout.fillWidth: true
                            TextField { text: Presets.defaultStartTime; Layout.fillWidth: true; onEditingFinished: Presets.defaultStartTime = text }
                        }
                        LabeledField { label: qsTr("默认结束时间"); Layout.fillWidth: true
                            TextField { text: Presets.defaultEndTime; Layout.fillWidth: true; onEditingFinished: Presets.defaultEndTime = text }
                        }
                    }
                }
            }
        }
    }
}
