import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Row {
    id: legend

    property var items: []
    property bool horizontal: true
    property int itemSpacing: Theme.spacingLg
    property int iconSize: 12

    spacing: itemSpacing

    function getItemColor(index) {
        return Theme.chartColor(index)
    }

    Repeater {
        model: legend.items

        Item {
            id: legendItem
            implicitWidth: row.width
            implicitHeight: 20

            readonly property var itemData: modelData
            readonly property int itemIndex: index

            Row {
                id: row
                spacing: 6
                anchors.verticalCenter: parent.verticalCenter

                Rectangle {
                    id: colorBox
                    width: legend.iconSize
                    height: legend.iconSize
                    radius: 2
                    color: legend.getItemColor(index)
                    anchors.verticalCenter: parent.verticalCenter
                }

                Label {
                    id: label
                    text: modelData.label || modelData.name || ""
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                    color: Theme.textMuted
                    anchors.verticalCenter: parent.verticalCenter

                    Behavior on color {
                        ColorAnimation {
                            duration: 150
                        }
                    }
                }

                Label {
                    id: valueLabel
                    text: modelData.value !== undefined ? `: ${modelData.value}` : ""
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                    font.bold: true
                    color: Theme.text
                    anchors.verticalCenter: parent.verticalCenter

                    Behavior on color {
                        ColorAnimation {
                            duration: 150
                        }
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onEntered: {
                    label.color = Theme.text
                    valueLabel.color = Theme.primary
                }

                onExited: {
                    label.color = Theme.textMuted
                    valueLabel.color = Theme.text
                }
            }
        }
    }
}
