import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ToolBar {
    id: root
    AppTheme { id: theme }

    signal toggleNavRequested()

    contentHeight: 52
    background: Rectangle {
        color: theme.surface
        border.color: theme.border
        border.width: 1
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 10

        ToolButton {
            text: "\u2630"
            onClicked: root.toggleNavRequested()
            Accessible.name: qsTr("Toggle navigation")
        }

        Label {
            text: qsTr("Attendance Admin")
            color: theme.text
            font.pixelSize: 15
            font.weight: Font.DemiBold
            Layout.alignment: Qt.AlignVCenter
        }

        Item { Layout.fillWidth: true }

        TextField {
            id: searchField
            Layout.preferredWidth: 320
            Layout.alignment: Qt.AlignVCenter
            placeholderText: qsTr("搜索人员 / 设备 / 记录…")
            font.pixelSize: 12
            selectByMouse: true
            background: Rectangle {
                radius: 10
                color: "#F1F5F9"
                border.color: theme.border
                border.width: 1
            }
        }

        ToolButton {
            text: qsTr("刷新")
            Accessible.name: qsTr("Refresh")
        }

        Label {
            text: qsTr("Server")
            color: theme.textMuted
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
