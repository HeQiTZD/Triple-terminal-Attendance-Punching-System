import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

Item {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("配置预设")
            subtitle: qsTr("服务器连接 / 默认值 · 自动持久化到本机配置")
            actions: [
                Button {
                    text: qsTr("恢复默认")
                    onClicked: { Presets.reset(); Logger.info("已恢复默认配置") }
                },
                Button {
                    text: qsTr("应用 & 通知")
                    highlighted: true
                    onClicked: {
                        Presets.notifyChanged()
                        Logger.ok("已保存预设到本机注册表")
                    }
                }
            ]
        }

        ScrollView {
            id: settingsScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: Theme.spacingMd

                Card {
                    Layout.fillWidth: true
                    title: qsTr("服务器连接")

                    GridLayout {
                        width: parent.width
                        columns: 2
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("服务器地址"); Layout.fillWidth: true
                            TextField { text: Presets.serverHost; Layout.fillWidth: true; onEditingFinished: Presets.serverHost = text }
                        }
                        LabeledField { label: qsTr("端口"); Layout.fillWidth: true
                            SpinBox {
                                from: 1; to: 65535; editable: true
                                value: Presets.serverPort
                                Layout.fillWidth: true
                                onValueModified: Presets.serverPort = value
                            }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    title: qsTr("默认值")

                    GridLayout {
                        width: parent.width
                        columns: 2
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("默认设备 ID"); Layout.fillWidth: true
                            TextField { text: Presets.defaultDeviceId; Layout.fillWidth: true; onEditingFinished: Presets.defaultDeviceId = text }
                        }
                        LabeledField { label: qsTr("默认工号"); Layout.fillWidth: true
                            TextField { text: Presets.defaultEmployeeId; Layout.fillWidth: true; onEditingFinished: Presets.defaultEmployeeId = text }
                        }
                        LabeledField { label: qsTr("默认起始时间"); Layout.fillWidth: true
                            TextField { text: Presets.defaultStartTime; Layout.fillWidth: true; onEditingFinished: Presets.defaultStartTime = text }
                        }
                        LabeledField { label: qsTr("默认结束时间"); Layout.fillWidth: true
                            TextField { text: Presets.defaultEndTime; Layout.fillWidth: true; onEditingFinished: Presets.defaultEndTime = text }
                        }
                    }
                }
            }
        }
    }
}
