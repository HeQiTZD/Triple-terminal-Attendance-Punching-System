import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

/*
  以标签形式展示当前账号权限（可限制条数）。
*/
Flow {
    id: root

    property var sessionManager
    property int maxVisible: 12
    property bool showOverflow: true

    spacing: Theme.spacingXs

    readonly property var permList: {
        if (!sessionManager || !sessionManager.isLoggedIn)
            return []
        const p = sessionManager.permissions
        void p
        return sessionManager.permissions || []
    }

    readonly property int overflowCount: Math.max(0, permList.length - maxVisible)

    Repeater {
        model: Math.min(root.permList.length, root.maxVisible)
        delegate: Rectangle {
            required property int index
            radius: Theme.radiusSm
            color: Theme.accentSubtle
            border.color: Theme.accent
            border.width: 1
            implicitHeight: chipLbl.implicitHeight + 6
            implicitWidth: chipLbl.implicitWidth + 14

            Label {
                id: chipLbl
                anchors.centerIn: parent
                text: PermissionCatalog.permissionDisplayName(root.permList[index])
                color: Theme.accent
                font.pixelSize: Theme.fontXs
                font.family: Theme.fontFamily
            }
        }
    }

    Rectangle {
        visible: root.showOverflow && root.overflowCount > 0
        radius: Theme.radiusSm
        color: Theme.surfaceAlt
        border.color: Theme.border
        border.width: 1
        implicitHeight: moreLbl.implicitHeight + 6
        implicitWidth: moreLbl.implicitWidth + 14

        Label {
            id: moreLbl
            anchors.centerIn: parent
            text: qsTr("+%1").arg(root.overflowCount)
            color: Theme.textMuted
            font.pixelSize: Theme.fontXs
            font.family: Theme.fontFamily
        }
    }
}
