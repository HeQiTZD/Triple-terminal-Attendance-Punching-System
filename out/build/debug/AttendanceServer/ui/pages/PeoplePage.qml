import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    title: qsTr("人员管理")

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: qsTr("人员管理")
            font.pixelSize: 20
            font.weight: Font.DemiBold
        }

        Label {
            text: qsTr("人员信息增删改查、导入导出入口。")
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
