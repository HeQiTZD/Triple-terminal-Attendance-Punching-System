import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: container

    property string title: ""
    property var chart: null
    property var legendItems: []
    property bool showLegend: true
    property bool showHeader: true
    property int headerHeight: 40

    color: Theme.surface
    radius: Theme.radiusMd
    border.color: Theme.border
    border.width: 1

    implicitWidth: parent ? parent.width : 400
    implicitHeight: showHeader ? 300 + headerHeight : 300

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: container.headerHeight
            visible: container.showHeader
            color: Theme.surfaceAlt
            radius: container.title.length > 0 ? Theme.radiusMd : 0

            Label {
                anchors.left: parent.left
                anchors.leftMargin: Theme.spacingLg
                anchors.verticalCenter: parent.verticalCenter
                text: container.title
                font.pixelSize: Theme.fontLg
                font.family: Theme.fontFamily
                font.bold: true
                color: Theme.text
                visible: container.title.length > 0
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 1
                color: Theme.border
                visible: container.title.length > 0
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Loader {
                id: chartLoader
                anchors.fill: parent
                anchors.margins: Theme.spacingMd
                sourceComponent: container.chart
                active: container.chart !== null

                onLoaded: {
                    if (item) {
                        item.anchors.fill = parent
                    }
                }
            }

            Label {
                anchors.centerIn: parent
                visible: container.chart === null
                text: qsTr("暂无图表数据")
                font.pixelSize: Theme.fontMd
                font.family: Theme.fontFamily
                color: Theme.textMuted
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            Layout.leftMargin: Theme.spacingMd
            Layout.rightMargin: Theme.spacingMd
            Layout.bottomMargin: Theme.spacingMd
            Layout.topMargin: Theme.spacingSm
            color: "transparent"
            visible: container.showLegend && container.legendItems.length > 0

            ChartLegend {
                anchors.centerIn: parent
                items: container.legendItems
                horizontal: true
            }
        }
    }
}
