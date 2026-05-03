import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("配置预设")
            subtitle: qsTr("数据库 / TCP / ArcFace / 默认值 · 自动持久化到本机配置")
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
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingMd

                Card {
                    Layout.fillWidth: true
                    title: qsTr("数据库（仅记录，重启后由 main.cpp 使用）")

                    GridLayout {
                        anchors.fill: parent
                        columns: 2
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("Host"); Layout.fillWidth: true
                            TextField { text: Presets.dbHost; Layout.fillWidth: true; onEditingFinished: Presets.dbHost = text }
                        }
                        LabeledField { label: qsTr("DB Name"); Layout.fillWidth: true
                            TextField { text: Presets.dbName; Layout.fillWidth: true; onEditingFinished: Presets.dbName = text }
                        }
                        LabeledField { label: qsTr("User"); Layout.fillWidth: true
                            TextField { text: Presets.dbUser; Layout.fillWidth: true; onEditingFinished: Presets.dbUser = text }
                        }
                        LabeledField { label: qsTr("Password"); Layout.fillWidth: true
                            TextField {
                                text: Presets.dbPassword; Layout.fillWidth: true
                                echoMode: TextInput.Password
                                onEditingFinished: Presets.dbPassword = text
                            }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    title: qsTr("TCP & 默认设备")

                    GridLayout {
                        anchors.fill: parent
                        columns: 2
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("默认端口"); Layout.fillWidth: true
                            SpinBox {
                                from: 1; to: 65535; editable: true
                                value: Presets.tcpPort
                                Layout.fillWidth: true
                                onValueModified: Presets.tcpPort = value
                            }
                        }
                        LabeledField { label: qsTr("默认 deviceId"); Layout.fillWidth: true
                            TextField { text: Presets.defaultDeviceId; Layout.fillWidth: true; onEditingFinished: Presets.defaultDeviceId = text }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    title: qsTr("ArcFace 授权")

                    GridLayout {
                        anchors.fill: parent
                        columns: 2
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("AppId"); Layout.fillWidth: true
                            TextField { text: Presets.faceAppId; Layout.fillWidth: true; onEditingFinished: Presets.faceAppId = text }
                        }
                        LabeledField { label: qsTr("SdkKey"); Layout.fillWidth: true
                            TextField {
                                text: Presets.faceSdkKey; Layout.fillWidth: true
                                echoMode: TextInput.Password
                                onEditingFinished: Presets.faceSdkKey = text
                            }
                        }
                    }
                }

                Card {
                    Layout.fillWidth: true
                    title: qsTr("默认时间窗 & 工号")

                    GridLayout {
                        anchors.fill: parent
                        columns: 2
                        rowSpacing: Theme.spacingSm
                        columnSpacing: Theme.spacingMd

                        LabeledField { label: qsTr("默认工号"); Layout.fillWidth: true
                            TextField { text: Presets.defaultEmployeeId; Layout.fillWidth: true; onEditingFinished: Presets.defaultEmployeeId = text }
                        }
                        LabeledField { label: qsTr("默认起始时间"); Layout.fillWidth: true
                            TextField { text: Presets.defaultStartTime; Layout.fillWidth: true; onEditingFinished: Presets.defaultStartTime = text }
                        }
                        LabeledField { label: qsTr("默认结束时间"); Layout.fillWidth: true
                            TextField { text: Presets.defaultEndTime; Layout.fillWidth: true; onEditingFinished: Presets.defaultEndTime = text }
                        }
                        LabeledField { label: qsTr("默认导出路径"); Layout.fillWidth: true
                            TextField { text: Presets.lastExportPath; Layout.fillWidth: true; onEditingFinished: Presets.lastExportPath = text }
                        }
                    }
                }
            }
        }
    }
}
