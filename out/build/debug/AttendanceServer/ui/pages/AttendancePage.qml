import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    title: qsTr("打卡记录")

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: qsTr("打卡记录")
            font.pixelSize: 20
            font.weight: Font.DemiBold
        }

        Label {
            text: qsTr("接收、查询、筛选、导出打卡记录入口。")
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
