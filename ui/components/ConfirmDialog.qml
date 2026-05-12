import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Dialog {
    id: root
    title: qsTr("确认")
    modal: true
    standardButtons: Dialog.Yes | Dialog.No

    readonly property real _maxW: Overlay.overlay && Overlay.overlay.width > 0
        ? Math.max(320, Overlay.overlay.width - 48)
        : 480
    readonly property int _bodyW: Math.min(440, Math.max(260, Math.floor(_maxW) - 40))

    implicitWidth: Math.min(480, _maxW)

    property string message: ""

    contentItem: Item {
        implicitWidth: root._bodyW
        implicitHeight: dlgLabel.implicitHeight + Theme.spacingMd * 2

        Label {
            id: dlgLabel
            width: parent.width
            wrapMode: Text.WordWrap
            text: root.message
            color: Theme.text
            font.pixelSize: Theme.fontMd
            font.family: Theme.fontFamily
        }
    }

    background: Rectangle {
        color: Theme.surface
        border.color: Theme.border
        border.width: 1
        radius: Theme.radiusMd
    }
}
