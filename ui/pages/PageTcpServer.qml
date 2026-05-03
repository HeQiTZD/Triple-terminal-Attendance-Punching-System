import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    property var clients: []

    function refreshClients() {
        clients = tcpServer.connectedClients()
    }

    Component.onCompleted: refreshClients()

    Connections {
        target: tcpServer
        function onClientsChanged() { page.refreshClients() }
        function onClientCountChanged() { page.refreshClients() }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("TCP 服务器")
            subtitle: qsTr("启动 / 停止监听 · 查看在线设备 · 单发或广播 JSON 报文")
        }

        // ===== 控制 =====
        Card {
            Layout.fillWidth: true
            title: qsTr("服务控制")

            RowLayout {
                width: parent.width
                spacing: Theme.spacingMd

                LabeledField {
                    label: qsTr("端口")
                    labelWidth: 40
                    Layout.preferredWidth: 200
                    SpinBox {
                        id: portInput
                        from: 1
                        to: 65535
                        value: Presets.tcpPort
                        editable: true
                        Layout.preferredWidth: 120
                        onValueModified: Presets.tcpPort = value
                    }
                }

                Button {
                    text: tcpServer.isRunning ? qsTr("已在运行") : qsTr("启动服务")
                    enabled: !tcpServer.isRunning
                    highlighted: true
                    onClicked: {
                        const ok = tcpServer.startServer(portInput.value)
                        Logger.logResult("启动 TCP 服务", ok, "端口=" + portInput.value)
                        History.record({
                            direction: "INVOKE",
                            target: "tcpServer.startServer",
                            payload: { port: portInput.value },
                            ok: ok,
                            result: ok ? "OK" : "FAIL",
                            category: "tcp"
                        })
                    }
                }
                Button {
                    text: qsTr("停止服务")
                    enabled: tcpServer.isRunning
                    onClicked: {
                        tcpServer.stopServer()
                        Logger.logResult("停止 TCP 服务", true, "")
                        History.record({
                            direction: "INVOKE",
                            target: "tcpServer.stopServer",
                            payload: "",
                            ok: true,
                            result: "OK",
                            category: "tcp"
                        })
                    }
                }
                Button {
                    text: qsTr("刷新客户端")
                    onClicked: page.refreshClients()
                }

                Item { Layout.fillWidth: true }

                BadgeStatus {
                    text: tcpServer.isRunning ? "running" : "stopped"
                }
            }
        }

        // ===== 主体两栏 =====
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            // 左：在线客户端表格
            Card {
                SplitView.preferredWidth: 460
                SplitView.minimumWidth: 300
                stretchContent: true
                title: qsTr("在线客户端")
                subtitle: qsTr("点击行选择目标设备")

                DataTable {
                    anchors.fill: parent
                    columns: [
                        { key: "deviceId",  title: qsTr("设备 ID"), width: 130 },
                        { key: "ipAddress", title: qsTr("IP"), width: 140 },
                        { key: "authenticated", title: qsTr("认证"),
                          formatter: function(v) { return v ? qsTr("是") : qsTr("否") } }
                    ]
                    rows: page.clients
                    emptyText: qsTr("暂无在线客户端")
                    onRowClicked: function(idx, row) {
                        targetInput.text = row.deviceId
                    }
                }
            }

            // 右：JSON 编辑 + 操作
            Card {
                SplitView.fillWidth: true
                stretchContent: true
                title: qsTr("发送 / 广播 JSON")

                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingMd

                    LabeledField {
                        Layout.fillWidth: true
                        label: qsTr("目标设备 ID")

                        TextField {
                            id: targetInput
                            text: Presets.defaultDeviceId
                            Layout.fillWidth: true
                            placeholderText: qsTr("点击左侧客户端或手动输入")
                        }
                        Button {
                            text: qsTr("发送")
                            highlighted: true
                            onClicked: {
                                const ok = testApi.sendToClientJson(targetInput.text, jsonEd.text)
                                Logger.logResult("发送 JSON 到设备", ok, "设备=" + targetInput.text)
                                History.record({
                                    direction: "OUT",
                                    target: targetInput.text,
                                    payload: jsonEd.text,
                                    ok: ok,
                                    result: ok ? "OK" : ("FAIL: " + testApi.lastError),
                                    category: "tcp"
                                })
                            }
                        }
                        Button {
                            text: qsTr("广播")
                            onClicked: {
                                const ok = testApi.broadcastJson(jsonEd.text)
                                Logger.logResult("广播 JSON", ok, "长度=" + jsonEd.text.length)
                                History.record({
                                    direction: "OUT",
                                    target: "<broadcast>",
                                    payload: jsonEd.text,
                                    ok: ok,
                                    result: ok ? "OK" : ("FAIL: " + testApi.lastError),
                                    category: "tcp"
                                })
                            }
                        }
                    }

                    Row {
                        spacing: Theme.spacingSm
                        Button {
                            text: qsTr("模板：ping")
                            flat: true
                            onClicked: jsonEd.text = '{\n  "type": "ping",\n  "timestamp": "manual-test"\n}'
                        }
                        Button {
                            text: qsTr("模板：sync_request")
                            flat: true
                            onClicked: jsonEd.text = '{\n  "type": "sync_request"\n}'
                        }
                        Button {
                            text: qsTr("模板：device_status")
                            flat: true
                            onClicked: jsonEd.text = '{\n  "type": "device_status",\n  "status": "online"\n}'
                        }
                    }

                    JsonEditor {
                        id: jsonEd
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: '{\n  "type": "ping",\n  "timestamp": "manual-test"\n}'
                    }
                }
            }
        }
    }
}
