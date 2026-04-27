import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AppCard {
    id: root
    AppTheme { id: theme }

    property string title: ""
    property string value: ""
    property color accent: theme.primary
    property string hint: ""

    height: 96

    RowLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 12

        Rectangle {
            width: 40
            height: 40
            radius: 12
            color: Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.12)
            border.color: Qt.rgba(root.accent.r, root.accent.g, root.accent.b, 0.22)
            border.width: 1

            Rectangle {
                anchors.centerIn: parent
                width: 12
                height: 12
                radius: 6
                color: root.accent
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Label {
                text: root.title
                color: theme.textMuted
                font.pixelSize: 12
            }

            Label {
                text: root.value
                color: theme.text
                font.pixelSize: 20
                font.weight: Font.DemiBold
                elide: Text.ElideRight
            }

            Label {
                visible: root.hint.length > 0
                text: root.hint
                color: theme.textMuted
                font.pixelSize: 11
                elide: Text.ElideRight
            }
        }
    }
}
