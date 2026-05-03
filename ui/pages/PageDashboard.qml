import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    function refreshAll() {
        const persons = dataService.getAllPerson()
        const devices = dataService.getAllDevices()
        const faces   = dataService.getAllFaceData()
        statPersons.value = persons.length
        statDevices.value = devices.length
        statFaces.value   = faces.length
    }

    Component.onCompleted: refreshAll()

    Connections {
        target: dataService
        function onConnectionStateChanged() { if (dataService.isConnected) page.refreshAll() }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingLg

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("概览")
            subtitle: qsTr("AttendanceServer 测试工作台 · 总览数据库 / TCP / 客户端 / 关键指标")
            actions: [
                Button {
                    text: qsTr("刷新统计")
                    onClicked: page.refreshAll()
                }
            ]
        }

        // ===== 关键状态卡 =====
        GridLayout {
            Layout.fillWidth: true
            columns: 4
            rowSpacing: Theme.spacingMd
            columnSpacing: Theme.spacingMd

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("数据库连接")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingSm
                    BadgeStatus {
                        text: dataService.isConnected ? "online" : "offline"
                    }
                    Label {
                        text: dataService.isConnected
                              ? qsTr("MySQL 已连接")
                              : qsTr("未连接，请检查配置")
                        color: Theme.text
                        font.pixelSize: Theme.fontMd
                    }
                    Item { Layout.fillHeight: true }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("TCP 服务")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingSm
                    BadgeStatus {
                        text: tcpServer.isRunning ? "running" : "stopped"
                    }
                    Label {
                        text: tcpServer.isRunning ? qsTr("已启动") : qsTr("未运行")
                        color: Theme.text
                        font.pixelSize: Theme.fontMd
                    }
                    Item { Layout.fillHeight: true }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("在线客户端")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingXs
                    Label {
                        text: tcpServer.clientCount
                        color: Theme.accent
                        font.pixelSize: 32
                        font.bold: true
                    }
                    Label {
                        text: qsTr("个 已连接")
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontSm
                    }
                }
            }

            Card {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                title: qsTr("最近日志")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingXs
                    Label {
                        text: Logger.lastMessage.length ? Logger.lastMessage : qsTr("（暂无）")
                        color: Theme.text
                        font.pixelSize: Theme.fontSm
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                    Label {
                        text: Logger.lastError.length ? (qsTr("最近错误：") + Logger.lastError) : ""
                        color: Theme.danger
                        font.pixelSize: Theme.fontXs
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                        visible: text.length > 0
                    }
                    Item { Layout.fillHeight: true }
                }
            }
        }

        // ===== 数据指标 =====
        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: Theme.spacingMd
            columnSpacing: Theme.spacingMd

            Card {
                id: statPersons
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                title: qsTr("人员")
                property int value: 0

                RowLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingMd

                    Label {
                        text: statPersons.value
                        color: Theme.accent
                        font.pixelSize: 36
                        font.bold: true
                    }
                    Label {
                        text: qsTr("条记录")
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontSm
                    }
                    Item { Layout.fillWidth: true }
                }
            }
            Card {
                id: statDevices
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                title: qsTr("设备")
                property int value: 0

                RowLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingMd

                    Label {
                        text: statDevices.value
                        color: Theme.accent
                        font.pixelSize: 36
                        font.bold: true
                    }
                    Label {
                        text: qsTr("条记录")
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontSm
                    }
                    Item { Layout.fillWidth: true }
                }
            }
            Card {
                id: statFaces
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                title: qsTr("人脸库")
                property int value: 0

                RowLayout {
                    anchors.fill: parent
                    spacing: Theme.spacingMd

                    Label {
                        text: statFaces.value
                        color: Theme.accent
                        font.pixelSize: 36
                        font.bold: true
                    }
                    Label {
                        text: qsTr("条记录")
                        color: Theme.textMuted
                        font.pixelSize: Theme.fontSm
                    }
                    Item { Layout.fillWidth: true }
                }
            }
        }

        // ===== 快捷操作 =====
        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("快捷操作")
            subtitle: qsTr("常用测试操作一键触发")

            Flow {
                anchors.fill: parent
                spacing: Theme.spacingMd

                Button {
                    text: tcpServer.isRunning ? qsTr("停止 TCP 服务") : qsTr("启动 TCP 服务（默认端口）")
                    onClicked: {
                        if (tcpServer.isRunning) {
                            tcpServer.stopServer()
                            Logger.logResult("停止 TCP", true, "")
                        } else {
                            const port = Presets.tcpPort > 0 ? Presets.tcpPort : 8080
                            const ok = tcpServer.startServer(port)
                            Logger.logResult("启动 TCP", ok, "端口=" + port)
                        }
                    }
                }
                Button {
                    text: qsTr("刷新所有列表")
                    onClicked: page.refreshAll()
                }
                Button {
                    text: qsTr("清空日志")
                    onClicked: Logger.clear()
                }
                Button {
                    text: qsTr("记录状态快照")
                    onClicked: Logger.info("快照：db=" + dataService.isConnected
                                          + ", tcp=" + tcpServer.isRunning
                                          + ", clients=" + tcpServer.clientCount)
                }
            }
        }
    }
}
