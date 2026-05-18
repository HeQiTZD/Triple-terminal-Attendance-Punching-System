import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Rectangle {
    id: root
    color: Theme.surface
    border.color: Theme.border
    border.width: 0
    implicitHeight: Theme.statusBarHeight

    property var sessionManager
    property string lastError: ""
    property string lastInfo: ""

    readonly property int connState: sessionManager ? sessionManager.connectionState : 0
    readonly property string connLabel: ErrorCatalog.connectionStateLabel(connState)
    readonly property string userLabel: {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return ""
        const u = sessionManager.currentUsername || ""
        const roles = sessionManager.roles
        const role = roles && roles.length > 0 ? roles[0] : ""
        return role ? (u + " · " + role) : u
    }

    Rectangle { width: parent.width; height: 1; color: Theme.border; anchors.bottom: parent.bottom }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingMd
        anchors.rightMargin: Theme.spacingMd
        spacing: Theme.spacingMd

        Label {
            text: qsTr("考勤管理端")
            color: Theme.text
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
            font.bold: true
        }

        BadgeStatus {
            text: root.connLabel
            accent: {
                switch (root.connState) {
                case 3: return Theme.success
                case 2: return Theme.info
                case 1: return Theme.warning
                default: return Theme.danger
                }
            }
        }

        Label {
            visible: root.userLabel.length > 0
            text: root.userLabel
            color: Theme.textMuted
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
        }

        Item { Layout.fillWidth: true }

        Label {
            text: root.lastError.length
                  ? (qsTr("错误：") + root.lastError)
                  : (root.lastInfo.length ? (qsTr("提示：") + root.lastInfo) : "")
            color: root.lastError.length ? Theme.danger : Theme.textMuted
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            Layout.maximumWidth: 520
            horizontalAlignment: Text.AlignRight
        }
    }
}
