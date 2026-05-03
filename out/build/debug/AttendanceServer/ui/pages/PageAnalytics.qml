import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    function _parseDate(text) {
        const d = Date.fromLocaleString(Qt.locale(), text, "yyyy-MM-dd")
        return isNaN(d.getTime()) ? new Date() : d
    }

    function _record(target, args, ok, detail) {
        History.record({
            direction: "INVOKE",
            target: target,
            payload: args,
            ok: ok,
            result: ok ? ("OK | " + (detail || "")) : "FAIL",
            category: "analytics"
        })
    }

    function _stripDate(text) {
        return String(text || "").length >= 10 ? String(text).substring(0, 10) : ""
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("统计分析")
            subtitle: qsTr("日报 / 人员汇总 / 部门汇总 · JSON 输出")
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("查询参数")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingSm

                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    columnSpacing: Theme.spacingMd
                    rowSpacing: Theme.spacingSm

                    LabeledField { label: qsTr("起始日期"); Layout.fillWidth: true
                        TextField { id: startDate; text: page._stripDate(Presets.defaultStartTime); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("结束日期"); Layout.fillWidth: true
                        TextField { id: endDate; text: page._stripDate(Presets.defaultEndTime); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("部门"); Layout.fillWidth: true
                        TextField { id: deptInput; placeholderText: qsTr("可空"); Layout.fillWidth: true }
                    }
                    LabeledField { label: qsTr("工号"); Layout.fillWidth: true
                        TextField { id: empInput; placeholderText: qsTr("仅日报可指定"); Layout.fillWidth: true }
                    }
                }

                Row {
                    spacing: Theme.spacingSm
                    Button {
                        text: qsTr("日报汇总")
                        highlighted: true
                        onClicked: {
                            const result = attendanceAnalyzer.dailySummary(
                                page._parseDate(startDate.text), page._parseDate(endDate.text),
                                deptInput.text, empInput.text)
                            outputView.jsonObject = result
                            Logger.logResult("日报汇总", true, "条数=" + (result.length || 0))
                            page._record("attendanceAnalyzer.dailySummary",
                                { start: startDate.text, end: endDate.text, dept: deptInput.text, emp: empInput.text },
                                true, "size=" + (result.length || 0))
                        }
                    }
                    Button {
                        text: qsTr("人员汇总")
                        onClicked: {
                            const result = attendanceAnalyzer.personSummary(
                                page._parseDate(startDate.text), page._parseDate(endDate.text), deptInput.text)
                            outputView.jsonObject = result
                            Logger.logResult("人员汇总", true, "条数=" + (result.length || 0))
                            page._record("attendanceAnalyzer.personSummary",
                                { start: startDate.text, end: endDate.text, dept: deptInput.text },
                                true, "size=" + (result.length || 0))
                        }
                    }
                    Button {
                        text: qsTr("部门汇总")
                        onClicked: {
                            const result = attendanceAnalyzer.departmentSummary(
                                page._parseDate(startDate.text), page._parseDate(endDate.text))
                            outputView.jsonObject = result
                            Logger.logResult("部门汇总", true, "条数=" + (result.length || 0))
                            page._record("attendanceAnalyzer.departmentSummary",
                                { start: startDate.text, end: endDate.text },
                                true, "size=" + (result.length || 0))
                        }
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("结果")

            JsonViewer {
                id: outputView
                anchors.fill: parent
                title: qsTr("JSON 输出（只读）")
            }
        }
    }
}
