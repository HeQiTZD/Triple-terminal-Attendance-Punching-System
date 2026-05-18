import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    required property var sessionManager
    property var eventService
    signal navigateRequested(string pageKey)

    readonly property int connState: sessionManager ? sessionManager.connectionState : 0
    readonly property string connLabel: ErrorCatalog.connectionStateLabel(connState)
    readonly property int subscribedCount: eventService
        ? eventService.subscribedTopics.length : 0

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingLg

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("概览")
            subtitle: qsTr("连接状态、权限摘要与最近动态")
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 4
            rowSpacing: Theme.spacingMd
            columnSpacing: Theme.spacingMd

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("服务器连接")

                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingSm
                    BadgeStatus {
                        text: page.connLabel
                        accent: {
                            switch (page.connState) {
                            case 3: return Theme.success
                            case 2: return Theme.info
                            case 1: return Theme.warning
                            default: return Theme.danger
                            }
                        }
                    }
                    Label {
                        text: sessionManager && sessionManager.isLoggedIn
                              ? qsTr("已认证，可进行数据管理")
                              : qsTr("请先登录")
                        color: Theme.text
                        font.pixelSize: Theme.fontMd
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("当前用户 / 角色")

                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingXs
                    Label {
                        text: sessionManager && sessionManager.isLoggedIn
                              ? (sessionManager.currentUsername || "—")
                              : qsTr("未登录")
                        color: Theme.text
                        font.pixelSize: Theme.fontMd
                    }
                    Label {
                        visible: sessionManager && sessionManager.roles.length > 0
                        text: sessionManager.roles.join(", ")
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontSm
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("已订阅主题")

                Label {
                    text: String(page.subscribedCount)
                    color: Theme.text
                    font.pixelSize: Theme.fontXl
                    font.bold: true
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("今日推送")

                Label {
                    text: String(PushFeed.todayCount)
                    color: Theme.text
                    font.pixelSize: Theme.fontXl
                    font.bold: true
                }
            }
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("权限摘要")
            visible: sessionManager && sessionManager.isLoggedIn

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingXs
                Label {
                    text: qsTr("权限数：") + sessionManager.permissions.length
                          + "  ·  " + qsTr("可管理：")
                          + PermissionCatalog.managedModulesSummary(sessionManager)
                    color: Theme.text
                    font.pixelSize: Theme.fontSm
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("快捷操作")

            Row {
                spacing: Theme.spacingSm
                PermissionButton {
                    sessionManager: page.sessionManager
                    requiredPermission: "person.read"
                    text: qsTr("人员查询")
                    onClicked: page.navigateRequested("person")
                }
                PermissionButton {
                    sessionManager: page.sessionManager
                    requiredPermission: "attendance.read"
                    text: qsTr("考勤查询")
                    onClicked: page.navigateRequested("attendance")
                }
                PermissionButton {
                    sessionManager: page.sessionManager
                    requiredPermission: "face.register"
                    text: qsTr("人脸注册")
                    onClicked: page.navigateRequested("face")
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Theme.spacingMd

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                stretchContent: true
                title: qsTr("最近调用")

                ListView {
                    anchors.fill: parent
                    clip: true
                    model: History.model
                    orientation: ListView.Vertical
                    delegate: Item {
                        required property int index
                        width: ListView.view.width
                        height: rowLbl.implicitHeight + 8
                        readonly property var entry: History.get(index)
                        Label {
                            id: rowLbl
                            width: parent.width - Theme.spacingMd * 2
                            x: Theme.spacingMd
                            anchors.verticalCenter: parent.verticalCenter
                            text: entry ? ("[" + entry.time + "] " + entry.direction + " " + entry.target) : ""
                            color: entry && entry.ok ? Theme.textMuted : Theme.danger
                            font.pixelSize: Theme.fontXs
                            font.family: Theme.fontMono
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.fillHeight: true
                stretchContent: true
                title: qsTr("最近推送")

                ListView {
                    anchors.fill: parent
                    clip: true
                    model: PushFeed.model
                    delegate: Item {
                        required property int index
                        width: ListView.view.width
                        height: lbl.implicitHeight + 8
                        readonly property var entry: PushFeed.model.get(index)
                        Label {
                            id: lbl
                            width: parent.width - Theme.spacingMd * 2
                            x: Theme.spacingMd
                            anchors.verticalCenter: parent.verticalCenter
                            text: entry ? ("[" + entry.time + "] " + entry.type) : ""
                            color: Theme.textMuted
                            font.pixelSize: Theme.fontXs
                            font.family: Theme.fontMono
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }
}
