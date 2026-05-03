import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Rectangle {
    id: root
    color: Theme.surface
    border.color: Theme.border
    border.width: 0
    implicitHeight: Theme.statusBarHeight

    property bool dbConnected: false
    property bool tcpRunning: false
    property int  tcpPort: 0
    property int  clientCount: 0
    property string lastError: ""
    property string lastInfo: ""

    Rectangle { width: parent.width; height: 1; color: Theme.border; anchors.bottom: parent.bottom }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingMd
        anchors.rightMargin: Theme.spacingMd
        spacing: Theme.spacingLg

        // 数据库
        RowLayout {
            spacing: 6
            BadgeStatus {
                text: root.dbConnected ? "online" : "offline"
                width: 12; height: 12
                dotOnly: true
            }
            Label {
                text: qsTr("数据库 ") + (root.dbConnected ? qsTr("已连接") : qsTr("未连接"))
                color: Theme.text
                font.pixelSize: Theme.fontSm
                font.family: Theme.fontFamily
            }
        }

        Rectangle { width: 1; Layout.fillHeight: true; color: Theme.border }

        // TCP
        RowLayout {
            spacing: 6
            BadgeStatus {
                text: root.tcpRunning ? "online" : "offline"
                width: 12; height: 12
                dotOnly: true
            }
            Label {
                text: root.tcpRunning
                      ? (qsTr("TCP 监听 :") + root.tcpPort)
                      : qsTr("TCP 未运行")
                color: Theme.text
                font.pixelSize: Theme.fontSm
                font.family: Theme.fontFamily
            }
        }

        Rectangle { width: 1; Layout.fillHeight: true; color: Theme.border }

        // 客户端数
        Label {
            text: qsTr("在线客户端：") + root.clientCount
            color: Theme.text
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
        }

        Item { Layout.fillWidth: true }

        // 错误/提示
        Label {
            text: root.lastError.length
                  ? (qsTr("错误：") + root.lastError)
                  : (root.lastInfo.length ? (qsTr("提示：") + root.lastInfo) : "")
            color: root.lastError.length ? Theme.danger : Theme.textMuted
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            Layout.maximumWidth: 600
            horizontalAlignment: Text.AlignRight
        }
    }
}
