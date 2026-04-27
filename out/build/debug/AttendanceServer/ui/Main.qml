pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: win
    width: 1200
    height: 800
    visible: true
    title: qsTr("AttendanceServer - 测试面板")

    ScrollView {
        anchors.fill: parent
        padding: 10

        ColumnLayout {
            id: testRoot
            width: parent.width
            spacing: 10

            function appendLog(s) {
                const ts = Qt.formatDateTime(new Date(), "HH:mm:ss")
                logArea.text += "[" + ts + "] " + s + "\n"
                logArea.cursorPosition = logArea.text.length
            }

            Connections {
                target: tcpServer
                function onClientConnected(deviceId, ipAddress) { appendLog("客户端连接: " + deviceId + " @ " + ipAddress) }
                function onClientDisconnected(deviceId) { appendLog("客户端断开: " + deviceId) }
                function onDeviceStatusReceived(deviceId, status) { appendLog("设备状态上报: " + deviceId + " => " + JSON.stringify(status)) }
                function onErrorOccurred(errorString) { appendLog("TCP错误: " + errorString) }
                function onIsRunningChanged() { appendLog("TCP运行状态=" + tcpServer.isRunning) }
                function onClientCountChanged() { appendLog("在线客户端数=" + tcpServer.clientCount) }
            }

            Connections {
                target: dataManager
                function onConnectionStateChanged() { appendLog("数据库连接状态=" + dataManager.isConnected) }
                function onErrorOccurred(errorString) { appendLog("数据库错误: " + errorString) }
                function onDeviceStatusChanged(devicdId, status) { appendLog("设备状态变更: " + devicdId + " => " + status) }
            }

            GroupBox {
                title: qsTr("状态")
                Layout.fillWidth: true
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 12
                    Label { text: qsTr("数据库") }
                    Label { text: dataManager ? (dataManager.isConnected ? qsTr("已连接") : qsTr("未连接")) : qsTr("不可用") }
                    Label { text: qsTr("TCP服务") }
                    Label { text: tcpServer ? (tcpServer.isRunning ? qsTr("运行中") : qsTr("已停止")) : qsTr("不可用") }
                    Label { text: qsTr("客户端") }
                    Label { text: tcpServer ? tcpServer.clientCount : 0 }
                    Item { Layout.fillWidth: true }
                }
            }

            GroupBox {
                title: qsTr("TCP 控制")
                Layout.fillWidth: true
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8
                    Label { text: qsTr("端口") }
                    TextField { id: tcpPort; text: "8080"; width: 120 }
                    Button {
                        text: qsTr("启动")
                        onClicked: {
                            const p = parseInt(tcpPort.text)
                            const ok = tcpServer.startServer(p)
                            appendLog("启动服务(" + p + ") => " + ok)
                        }
                    }
                    Button {
                        text: qsTr("停止")
                        onClicked: {
                            tcpServer.stopServer()
                            appendLog("停止服务()")
                        }
                    }
                    Item { Layout.fillWidth: true }
                }
            }

            GroupBox {
                title: qsTr("导出测试（CSV）")
                Layout.fillWidth: true
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    RowLayout {
                        spacing: 8
                        Label { text: qsTr("文件路径") }
                        TextField { id: exportPath; Layout.fillWidth: true; placeholderText: qsTr("例如：E:/temp/persons.csv") }
                        Button {
                            text: qsTr("导出人员")
                            onClicked: {
                                const ok = exportManager.exportPersonsCsv(exportPath.text)
                                appendLog("导出人员CSV => " + ok + (ok ? "" : (" 错误=" + exportManager.lastError)))
                            }
                        }
                        Button {
                            text: qsTr("导出设备")
                            onClicked: {
                                const ok = exportManager.exportDeviceCsv(exportPath.text)
                                appendLog("导出设备CSV => " + ok + (ok ? "" : (" 错误=" + exportManager.lastError)))
                            }
                        }
                    }

                    RowLayout {
                        spacing: 8
                        Label { text: qsTr("开始时间") }
                        TextField { id: exportStart; text: "2026-04-01 00:00:00"; width: 180 }
                        Label { text: qsTr("结束时间") }
                        TextField { id: exportEnd; text: "2026-04-30 23:59:59"; width: 180 }
                        Button {
                            text: qsTr("导出考勤")
                            onClicked: {
                                const startDate = Date.fromLocaleString(Qt.locale(), exportStart.text, "yyyy-MM-dd HH:mm:ss")
                                const endDate = Date.fromLocaleString(Qt.locale(), exportEnd.text, "yyyy-MM-dd HH:mm:ss")
                                const ok = exportManager.exportAttendanceRecordsCsv(exportPath.text, startDate, endDate)
                                appendLog("导出考勤CSV => " + ok + (ok ? "" : (" 错误=" + exportManager.lastError)))
                            }
                        }
                        Item { Layout.fillWidth: true }
                    }
                }
            }

            GroupBox {
                title: qsTr("数据层 CRUD 快测")
                Layout.fillWidth: true
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    RowLayout {
                        spacing: 8
                        TextField { id: pName; placeholderText: qsTr("姓名"); width: 140 }
                        TextField { id: pEmpId; placeholderText: qsTr("工号"); width: 140 }
                        TextField { id: pDept; placeholderText: qsTr("部门"); width: 140 }
                        TextField { id: pPos; placeholderText: qsTr("岗位"); width: 140 }
                        Button {
                            text: qsTr("新增人员")
                            onClicked: {
                                const ok = dataManager.addPerson(pName.text, pEmpId.text, pDept.text, pPos.text)
                                appendLog("新增人员 => " + ok)
                            }
                        }
                        Item { Layout.fillWidth: true }
                    }

                    RowLayout {
                        spacing: 8
                        Button {
                            text: qsTr("加载人员列表")
                            onClicked: {
                                testRoot.personModel = dataManager.getAllPerson()
                                appendLog("加载人员列表 => " + (testRoot.personModel ? testRoot.personModel.length : 0))
                            }
                        }
                        Button {
                            text: qsTr("加载设备列表")
                            onClicked: {
                                testRoot.deviceModel = dataManager.getAllDevices()
                                appendLog("加载设备列表 => " + (testRoot.deviceModel ? testRoot.deviceModel.length : 0))
                            }
                        }
                        Item { Layout.fillWidth: true }
                    }

                    RowLayout {
                        spacing: 12
                        ListView {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 120
                            clip: true
                            model: testRoot.personModel
                            delegate: Text {
                                text: (modelData && modelData.name) ? (modelData.employeeId + " - " + modelData.name) : JSON.stringify(modelData)
                            }
                        }
                        ListView {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 120
                            clip: true
                            model: testRoot.deviceModel
                            delegate: Text {
                                text: (modelData && modelData.deviceId) ? (modelData.deviceId + " - " + modelData.status) : JSON.stringify(modelData)
                            }
                        }
                    }
                }
            }

            GroupBox {
                title: qsTr("Sync 测试")
                Layout.fillWidth: true
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8
                    Label { text: qsTr("设备ID") }
                    TextField { id: syncDeviceId; text: "dev001"; width: 160 }
                    Button {
                        text: qsTr("下发人员同步")
                        onClicked: {
                            testApi.requestPersonSync(syncDeviceId.text)
                            appendLog("下发人员同步(" + syncDeviceId.text + ") 错误=" + testApi.lastError)
                        }
                    }
                    Item { Layout.fillWidth: true }
                }
            }

            GroupBox {
                title: qsTr("人脸测试（Base64 特征）")
                Layout.fillWidth: true
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    RowLayout {
                        spacing: 8
                        TextField { id: faceAppId; placeholderText: qsTr("AppId"); Layout.fillWidth: true }
                        TextField { id: faceSdkKey; placeholderText: qsTr("SdkKey"); Layout.fillWidth: true }
                        Button {
                            text: qsTr("初始化引擎")
                            onClicked: {
                                const ok = testApi.initFaceEngine(faceAppId.text, faceSdkKey.text)
                                appendLog("初始化引擎 => " + ok + (ok ? "" : (" 错误=" + testApi.lastError)))
                            }
                        }
                    }

                    RowLayout {
                        spacing: 8
                        TextField { id: img1; placeholderText: qsTr("图片路径1"); Layout.fillWidth: true }
                        Button {
                            text: qsTr("提取特征1")
                            onClicked: {
                                feat1.text = testApi.extractFeatureBase64(img1.text)
                                appendLog("提取特征1 长度=" + feat1.text.length + (feat1.text.length ? "" : (" 错误=" + testApi.lastError)))
                            }
                        }
                    }

                    RowLayout {
                        spacing: 8
                        TextField { id: img2; placeholderText: qsTr("图片路径2"); Layout.fillWidth: true }
                        Button {
                            text: qsTr("提取特征2")
                            onClicked: {
                                feat2.text = testApi.extractFeatureBase64(img2.text)
                                appendLog("提取特征2 长度=" + feat2.text.length + (feat2.text.length ? "" : (" 错误=" + testApi.lastError)))
                            }
                        }
                    }

                    RowLayout {
                        spacing: 8
                        Button {
                            text: qsTr("比对相似度")
                            onClicked: {
                                const sim = testApi.compareFeatureBase64(feat1.text, feat2.text)
                                appendLog("比对相似度 => " + sim + (sim >= 0 ? "" : (" 错误=" + testApi.lastError)))
                            }
                        }
                        Item { Layout.fillWidth: true }
                    }

                    TextField { id: feat1; placeholderText: qsTr("特征1（base64）"); Layout.fillWidth: true }
                    TextField { id: feat2; placeholderText: qsTr("特征2（base64）"); Layout.fillWidth: true }
                }
            }

            GroupBox {
                title: qsTr("日志")
                Layout.fillWidth: true
                Layout.preferredHeight: 240
                TextArea {
                    id: logArea
                    anchors.fill: parent
                    anchors.margins: 8
                    readOnly: true
                    wrapMode: TextArea.WrapAnywhere
                }
            }

            property var personModel: []
            property var deviceModel: []
        }
    }
}
