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
    readonly property int connState: sessionManager ? sessionManager.connectionState : 0
    readonly property bool serverConnected: root.connState === 3

    readonly property string userLabel: {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return ""
        const u = sessionManager.currentUsername || ""
        if (sessionManager) {
            const _p = sessionManager.permissions
            const _r = sessionManager.roles
            void _p
            void _r
        }
        const roleText = PermissionCatalog.formatRoles(sessionManager)
        return u + (roleText.length ? (" · " + roleText) : "")
    }

    Rectangle { width: parent.width; height: 1; color: Theme.border; anchors.bottom: parent.bottom }

    MouseArea {
        anchors.fill: parent
        anchors.rightMargin: captionButtons.width + Theme.spacingMd
        acceptedButtons: Qt.LeftButton
        onPressed: function(mouse) {
            const w = Window.window
            if (w && typeof w.startSystemMove === "function")
                w.startSystemMove()
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingMd
        anchors.rightMargin: Theme.spacingMd
        spacing: Theme.spacingMd
        z: 1

        BadgeStatus {
            text: root.serverConnected ? qsTr("服务器已连接") : qsTr("服务器未连接")
            accent: root.serverConnected ? Theme.success : Theme.danger
        }

        Label {
            visible: root.userLabel.length > 0
            text: root.userLabel
            color: Theme.textMuted
            font.pixelSize: Theme.fontSm
            font.family: Theme.fontFamily
            elide: Text.ElideRight
            Layout.maximumWidth: 360
        }

        Item { Layout.fillWidth: true }
    }

    WindowCaptionButtons {
        id: captionButtons
        z: 2
        anchors.right: parent.right
        anchors.rightMargin: Theme.spacingXs
        anchors.verticalCenter: parent.verticalCenter
    }
}
