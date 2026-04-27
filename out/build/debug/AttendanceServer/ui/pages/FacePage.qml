import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    title: qsTr("人脸数据")

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: qsTr("人脸数据")
            font.pixelSize: 20
            font.weight: Font.DemiBold
        }

        Label {
            text: qsTr("录入、更新、删除人脸特征数据入口。")
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
