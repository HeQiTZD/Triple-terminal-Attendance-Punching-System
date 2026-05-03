import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    property string keyword: ""
    property string categoryFilter: ""
    property int selectedIndex: -1

    function _filteredRows() {
        const arr = []
        for (let i = 0; i < History.model.count; ++i) {
            const e = History.model.get(i)
            if (page.keyword.length > 0 &&
                e.target.indexOf(page.keyword) < 0 &&
                e.payload.indexOf(page.keyword) < 0 &&
                e.result.indexOf(page.keyword) < 0)
                continue
            if (page.categoryFilter.length > 0 && e.category !== page.categoryFilter)
                continue
            arr.push({
                index: i,
                time: e.time,
                direction: e.direction,
                target: e.target,
                category: e.category,
                ok: e.ok,
                result: e.result,
                payload: e.payload
            })
        }
        return arr
    }

    property var rows: _filteredRows()

    Connections {
        target: History
        function onEntryAdded() { page.rows = page._filteredRows() }
    }

    function _replay(entry) {
        const cat = entry.category
        if (cat === "tcp" && entry.direction === "OUT") {
            if (entry.target === "<broadcast>") {
                const ok = testApi.broadcastJson(entry.payload)
                Logger.logResult("重放：广播", ok, "")
            } else {
                const ok = testApi.sendToClientJson(entry.target, entry.payload)
                Logger.logResult("重放：发送", ok, "设备=" + entry.target)
            }
        } else {
            Logger.warn("当前条目暂不支持重放：" + entry.target)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("请求 / 响应历史")
            subtitle: qsTr("所有调用与 TCP 收发记录 · 支持重放、保存、导出")
            actions: [
                Button {
                    text: qsTr("导出 JSON")
                    onClicked: {
                        const path = testApi.selectSaveFile(qsTr("导出历史"),
                            "history-" + Qt.formatDateTime(new Date(), "yyyyMMdd-HHmmss") + ".json",
                            "JSON (*.json);;所有文件 (*)")
                        if (!path) return
                        testApi.writeTextFile(path, History.exportJsonText())
                    }
                },
                Button {
                    text: qsTr("清空")
                    onClicked: { History.clear(); page.rows = page._filteredRows() }
                }
            ]
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("过滤")

            RowLayout {
                anchors.fill: parent
                spacing: Theme.spacingMd

                LabeledField {
                    label: qsTr("关键词"); Layout.fillWidth: true
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: qsTr("搜索 target / payload / result")
                        onTextChanged: { page.keyword = text; page.rows = page._filteredRows() }
                    }
                }
                LabeledField {
                    label: qsTr("分类"); Layout.fillWidth: true
                    ComboBox {
                        Layout.fillWidth: true
                        model: [
                            { text: qsTr("全部"), value: "" },
                            { text: "tcp",       value: "tcp" },
                            { text: "person",    value: "person" },
                            { text: "device",    value: "device" },
                            { text: "attendance",value: "attendance" },
                            { text: "face",      value: "face" },
                            { text: "sync",      value: "sync" },
                            { text: "export",    value: "export" },
                            { text: "analytics", value: "analytics" },
                            { text: "other",     value: "other" }
                        ]
                        textRole: "text"
                        valueRole: "value"
                        onActivated: { page.categoryFilter = currentValue; page.rows = page._filteredRows() }
                    }
                }
            }
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            Card {
                SplitView.fillWidth: true
                SplitView.preferredWidth: 600
                title: qsTr("调用列表")

                DataTable {
                    id: histTable
                    anchors.fill: parent
                    rows: page.rows
                    columns: [
                        { key: "time", title: qsTr("时间"), width: 100 },
                        { key: "direction", title: qsTr("方向"), width: 60 },
                        { key: "category", title: qsTr("分类"), width: 80 },
                        { key: "target", title: qsTr("目标 / 接口"), width: 200 },
                        { key: "result", title: qsTr("结果") }
                    ]
                    emptyText: qsTr("（暂无历史）")
                    onRowClicked: function(idx, row) { page.selectedIndex = row.index }
                }
            }

            Card {
                SplitView.preferredWidth: 480
                SplitView.minimumWidth: 320
                title: qsTr("详情")
                headerRight: [
                    Button {
                        text: qsTr("重放")
                        flat: true
                        enabled: page.selectedIndex >= 0
                        onClicked: {
                            const e = History.get(page.selectedIndex)
                            if (e) page._replay(e)
                        }
                    }
                ]

                JsonViewer {
                    anchors.fill: parent
                    title: page.selectedIndex >= 0
                           ? (History.get(page.selectedIndex).direction + " · "
                              + History.get(page.selectedIndex).target + " · "
                              + History.get(page.selectedIndex).result)
                           : qsTr("（未选中）")
                    text: page.selectedIndex >= 0 ? History.get(page.selectedIndex).payload : ""
                }
            }
        }
    }
}
