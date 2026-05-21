import QtQuick
import QtQuick.Controls

import AttendanceAdmin

Rectangle {
    id: root
    property string text: ""
    property color accent: Theme.statusColor(text)
    property bool dotOnly: false

    implicitWidth: dotOnly ? 12 : (label.implicitWidth + 22)
    implicitHeight: dotOnly ? 12 : 22
    radius: dotOnly ? 6 : 11
    color: Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.18)
    border.color: Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.5)
    border.width: 1

    Row {
        anchors.centerIn: parent
        spacing: 6
        visible: !root.dotOnly

        Rectangle {
            width: 8; height: 8; radius: 4
            color: root.accent
            anchors.verticalCenter: parent.verticalCenter
        }
        Label {
            id: label
            text: root.text
            color: root.accent
            font.pixelSize: Theme.fontXs
            font.family: Theme.fontFamily
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        visible: root.dotOnly
        anchors.centerIn: parent
        width: 8; height: 8; radius: 4
        color: root.accent
    }
}
