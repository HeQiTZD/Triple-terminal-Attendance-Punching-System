import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingLg

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("概览")
            subtitle: qsTr("AttendanceAdmin 管理端 · 连接服务端后查看关键指标")
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
                    BadgeStatus { text: "offline" }
                    Label {
                        text: qsTr("未连接，请在配置中设置服务器地址")
                        color: Theme.text
                        font.pixelSize: Theme.fontMd
                    }
                    Item { Layout.fillHeight: true }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("最近日志")

                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingXs
                    Label {
                        text: Logger.lastMessage.length ? Logger.lastMessage : qsTr("（暂无）")
                        color: Theme.text
                        font.pixelSize: Theme.fontSm
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                    Label {
                        text: Logger.lastError.length ? (qsTr("最近错误：") + Logger.lastError) : ""
                        color: Theme.danger
                        font.pixelSize: Theme.fontXs
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                        visible: text.length > 0
                    }
                    Item { Layout.fillHeight: true }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("快捷操作")

            Flow {
                width: parent.width
                spacing: Theme.spacingMd

                Button {
                    text: qsTr("清空日志")
                    onClicked: Logger.clear()
                }
            }
        }
    }
}
