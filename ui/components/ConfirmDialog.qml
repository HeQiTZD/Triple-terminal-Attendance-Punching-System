import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Dialog {
    id: root
    title: qsTr("确认")
    modal: true
    standardButtons: Dialog.Yes | Dialog.No

    property string message: ""

    contentItem: Label {
        text: root.message
        color: Theme.text
        font.pixelSize: Theme.fontMd
        font.family: Theme.fontFamily
        wrapMode: Text.WordWrap
        padding: Theme.spacingMd
    }

    background: Rectangle {
        color: Theme.surface
        border.color: Theme.border
        border.width: 1
        radius: Theme.radiusMd
    }
}
