import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    // 测试结果模型，每条：{ name, ok, message, durationMs }
    property ListModel results: ListModel {}

    // ===== 内置脚本：每条由若干 step 组成，step 是 function() 返回 { ok, message } =====
    readonly property var scripts: [
        {
            id: "smoke",
            name: qsTr("冒烟：连接 + TCP + 计数"),
            description: qsTr("验证数据库连通、启动 TCP 监听、获取在线客户端数"),
            steps: [
                {
                    name: qsTr("数据库已连接"),
                    run: function() {
                        const ok = !!dataService.isConnected
                        return { ok: ok, message: ok ? "isConnected=true" : "数据库未连接" }
                    }
                },
                {
                    name: qsTr("启动 TCP（如未运行）"),
                    run: function() {
                        if (tcpServer.isRunning) return { ok: true, message: "已在运行" }
                        const ok = tcpServer.startServer(Presets.tcpPort > 0 ? Presets.tcpPort : 8080)
                        return { ok: ok, message: ok ? "启动成功" : "启动失败" }
                    }
                },
                {
                    name: qsTr("读取在线客户端"),
                    run: function() {
                        const list = tcpServer.connectedClients()
                        return { ok: true, message: "在线 " + list.length + " 个" }
                    }
                }
            ]
        },
        {
            id: "personCrud",
            name: qsTr("人员链路：添加→查询→删除"),
            description: qsTr("使用临时工号端到端验证人员 CRUD"),
            steps: [
                {
                    name: qsTr("新增临时人员"),
                    run: function() {
                        const emp = "TST" + Math.floor(Math.random() * 100000)
                        page._tmpEmp = emp
                        const ok = dataService.addPerson("自动测试 " + emp, emp, "QA", "Tester")
                        return { ok: ok, message: "工号=" + emp }
                    }
                },
                {
                    name: qsTr("按工号查询命中"),
                    run: function() {
                        const obj = dataService.getPersonByEmployeeId(page._tmpEmp)
                        return { ok: obj !== null, message: obj ? ("id=" + obj.id) : "未命中" }
                    }
                },
                {
                    name: qsTr("删除临时人员"),
                    run: function() {
                        const ok = dataService.deletePerson(page._tmpEmp)
                        return { ok: ok, message: ok ? "已删除" : "删除失败" }
                    }
                }
            ]
        },
        {
            id: "deviceCrud",
            name: qsTr("设备链路：注册→改状态→筛选"),
            description: qsTr("注册临时设备、切换状态、按筛选条件查询"),
            steps: [
                {
                    name: qsTr("注册临时设备"),
                    run: function() {
                        const did = "test-" + Math.floor(Math.random() * 100000)
                        page._tmpDev = did
                        const ok = dataService.addOrUpdateDevice(did, "AutoDev", "127.0.0.1", "online")
                        return { ok: ok, message: "deviceId=" + did }
                    }
                },
                {
                    name: qsTr("更新状态为 maintenance"),
                    run: function() {
                        const ok = dataService.updateDeviceStatus(page._tmpDev, "maintenance")
                        return { ok: ok, message: ok ? "OK" : "更新失败" }
                    }
                },
                {
                    name: qsTr("按 deviceId 查询"),
                    run: function() {
                        const obj = dataService.getDeviceById(page._tmpDev)
                        return { ok: obj !== null, message: obj ? ("status=" + obj.status) : "未命中" }
                    }
                }
            ]
        },
        {
            id: "exportPersons",
            name: qsTr("导出：人员 CSV 到临时文件"),
            description: qsTr("将人员表导出为 CSV，校验文件读取成功"),
            steps: [
                {
                    name: qsTr("导出 CSV"),
                    run: function() {
                        const path = "test_persons_" + Date.now() + ".csv"
                        page._tmpExport = path
                        const ok = exportManager.exportPersonsCsv(path)
                        return { ok: ok, message: ok ? path : exportManager.lastError }
                    }
                },
                {
                    name: qsTr("校验可读"),
                    run: function() {
                        const text = testApi.readTextFile(page._tmpExport)
                        return { ok: text.length > 0, message: "字节=" + text.length }
                    }
                }
            ]
        },
        {
            id: "tcpBroadcast",
            name: qsTr("TCP：广播 ping 到所有客户端"),
            description: qsTr("向所有在线客户端发送 ping，无客户端时跳过"),
            steps: [
                {
                    name: qsTr("广播 ping"),
                    run: function() {
                        const list = tcpServer.connectedClients()
                        if (list.length === 0)
                            return { ok: true, message: "无在线客户端，已跳过" }
                        const ok = testApi.broadcastJson('{"type":"ping","timestamp":"batch-test"}')
                        return { ok: ok, message: ok ? ("发送给 " + list.length + " 个客户端") : testApi.lastError }
                    }
                }
            ]
        }
    ]

    property string _tmpEmp: ""
    property string _tmpDev: ""
    property string _tmpExport: ""

    function runScript(scriptIdx) {
        results.clear()
        const sc = scripts[scriptIdx]
        Logger.info("开始批量测试：" + sc.name)

        for (let i = 0; i < sc.steps.length; ++i) {
            const step = sc.steps[i]
            const t0 = Date.now()
            let r = { ok: false, message: "" }
            try {
                r = step.run() || { ok: false, message: "步骤未返回结果" }
            } catch (e) {
                r = { ok: false, message: "异常：" + e.message }
            }
            const cost = Date.now() - t0
            results.append({
                "name": step.name,
                "ok": !!r.ok,
                "message": String(r.message || ""),
                "durationMs": cost
            })
            Logger.logResult(sc.name + " > " + step.name, r.ok, r.message + " (" + cost + "ms)")
            if (!r.ok)
                break
        }
    }

    function runAll() {
        for (let i = 0; i < scripts.length; ++i)
            runScript(i)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("批量测试脚本")
            subtitle: qsTr("一键执行常用测试链路 · 实时显示 PASS / FAIL")
            actions: [
                Button {
                    text: qsTr("清空结果")
                    onClicked: page.results.clear()
                }
            ]
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            // 左：脚本列表
            Card {
                SplitView.preferredWidth: 380
                SplitView.minimumWidth: 280
                stretchContent: true
                title: qsTr("脚本")

                ColumnLayout {
                    width: parent.width
                    spacing: Theme.spacingSm

                    Repeater {
                        model: page.scripts
                        delegate: Rectangle {
                            required property var modelData
                            required property int index
                            Layout.fillWidth: true
                            Layout.preferredHeight: 64
                            color: mouse.containsMouse ? Theme.hover : "transparent"
                            radius: Theme.radiusSm
                            border.color: Theme.border
                            border.width: 1

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: Theme.spacingSm
                                spacing: 2
                                Label {
                                    text: modelData.name
                                    color: Theme.text
                                    font.bold: true
                                    font.pixelSize: Theme.fontMd
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                                Label {
                                    text: modelData.description
                                    color: Theme.textMuted
                                    font.pixelSize: Theme.fontXs
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                            }

                            MouseArea {
                                id: mouse
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: page.runScript(index)
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }

                    Button {
                        Layout.fillWidth: true
                        text: qsTr("一键全部运行")
                        highlighted: true
                        onClicked: page.runAll()
                    }
                }
            }

            // 右：结果
            Card {
                SplitView.fillWidth: true
                stretchContent: true
                title: qsTr("执行结果")

                DataTable {
                    anchors.fill: parent
                    rows: {
                        let arr = []
                        for (let i = 0; i < page.results.count; ++i) {
                            const e = page.results.get(i)
                            arr.push({
                                idx: i + 1,
                                name: e.name,
                                status: e.ok ? "PASS" : "FAIL",
                                message: e.message,
                                ms: e.durationMs
                            })
                        }
                        return arr
                    }
                    columns: [
                        { key: "idx", title: "#", width: 40, align: "right" },
                        { key: "status", title: qsTr("结果"), width: 70 },
                        { key: "name", title: qsTr("步骤"), width: 220 },
                        { key: "message", title: qsTr("说明") },
                        { key: "ms", title: qsTr("耗时(ms)"), width: 90, align: "right" }
                    ]
                    emptyText: qsTr("点击左侧脚本运行测试")
                }
            }
        }
    }
}
