import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    title: qsTr("统计分析")

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: qsTr("统计分析")
            font.pixelSize: 20
            font.weight: Font.DemiBold
        }

        Label {
            text: qsTr("按部门/人员/时间维度生成报表与图表的入口。")
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
