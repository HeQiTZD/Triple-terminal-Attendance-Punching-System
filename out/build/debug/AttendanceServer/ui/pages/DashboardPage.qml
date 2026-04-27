import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Page {
    title: qsTr("概览")
    AppTheme { id: theme }

    ColumnLayout {
        anchors.fill: parent
        spacing: theme.gap

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Label {
                    text: qsTr("概览")
                    color: theme.text
                    font.pixelSize: 22
                    font.weight: Font.DemiBold
                }
                Label {
                    text: qsTr("快速查看在线设备、今日打卡与异常指标。")
                    color: theme.textMuted
                    font.pixelSize: 12
                }
            }

            Button {
                text: qsTr("导出报表")
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: width >= 980 ? 4 : (width >= 720 ? 2 : 1)
            columnSpacing: theme.gap
            rowSpacing: theme.gap

            MetricTile {
                Layout.fillWidth: true
                title: qsTr("在线设备")
                value: qsTr("--")
                accent: theme.primary
                hint: qsTr("最近 5 分钟")
            }
            MetricTile {
                Layout.fillWidth: true
                title: qsTr("今日打卡")
                value: qsTr("--")
                accent: theme.success
                hint: qsTr("已接收记录")
            }
            MetricTile {
                Layout.fillWidth: true
                title: qsTr("异常记录")
                value: qsTr("--")
                accent: theme.warning
                hint: qsTr("迟到/缺卡等")
            }
            MetricTile {
                Layout.fillWidth: true
                title: qsTr("人员总数")
                value: qsTr("--")
                accent: "#8B5CF6"
                hint: qsTr("已入库")
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: theme.gap

            AppCard {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 8

                    Label {
                        text: qsTr("实时日志")
                        color: theme.text
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }
                    Label {
                        text: qsTr("后续可在此展示 TCP 连接、同步、异常等日志流。")
                        color: theme.textMuted
                        font.pixelSize: 12
                        wrapMode: Text.Wrap
                    }
                    Item { Layout.fillHeight: true }
                }
            }

            AppCard {
                Layout.preferredWidth: 360
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 8

                    Label {
                        text: qsTr("快捷操作")
                        color: theme.text
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }

                    Button { text: qsTr("新增人员") }
                    Button { text: qsTr("录入人脸") }
                    Button { text: qsTr("同步到设备") }
                    Button { text: qsTr("查看异常") }

                    Item { Layout.fillHeight: true }
                }
            }
        }
    }
}
