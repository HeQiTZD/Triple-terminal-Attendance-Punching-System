import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    title: qsTr("设置")

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: qsTr("设置")
            font.pixelSize: 20
            font.weight: Font.DemiBold
        }

        Label {
            text: qsTr("端口、数据库、ArcSoft Key、日志等配置入口。")
            opacity: 0.75
            wrapMode: Text.Wrap
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 10
            color: "#FFFFFF"
            border.color: "#E6E8F0"
            border.width: 1
        }
    }
}
