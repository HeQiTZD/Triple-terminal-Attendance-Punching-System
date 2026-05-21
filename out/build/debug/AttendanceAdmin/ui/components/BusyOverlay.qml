import QtQuick
import QtQuick.Controls

import AttendanceAdmin

Rectangle {
    id: root
    anchors.fill: parent
    visible: root.busy
    color: Qt.rgba(0.95, 0.96, 0.98, 0.65)
    z: 100

    property bool busy: false

    MouseArea {
        anchors.fill: parent
        enabled: root.busy
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: root.busy
    }
}
