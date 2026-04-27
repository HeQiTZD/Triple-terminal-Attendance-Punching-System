import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    AppTheme { id: theme }
    color: theme.surface
    radius: theme.radius
    border.color: theme.border
    border.width: 1

    // lightweight shadow (no GraphicalEffects dependency)
    Rectangle {
        anchors.fill: parent
        anchors.margins: -1
        radius: root.radius + 2
        color: "#000000"
        opacity: 0.04
        z: -1
    }
}
