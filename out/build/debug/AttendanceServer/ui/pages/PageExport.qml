import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    function _parseDateTime(text) {
        const d = Date.fromLocaleString(Qt.locale(), text, "yyyy-MM-dd HH:mm:ss")
        return isNaN(d.getTime()) ? new Date() : d
    }

    function _record(target, args, ok, detail) {
        History.record({
            direction: "INVOKE",
            target: target,
            payload: args,
            ok: ok,
            result: ok ? ("OK: " + (detail || "")) : ("FAIL: " + (detail || "")),
            category: "export"
        })
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("CSV 导出")
            subtitle: qsTr("将人员 / 设备 / 考勤记录导出为 UTF-8 BOM 的 CSV")
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("导出设置")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                LabeledField {
                    Layout.fillWidth: true
                    label: qsTr("文件路径")

                    TextField {
                        id: filePath
                        text: Presets.lastExportPath
                        Layout.fillWidth: true
                        placeholderText: qsTr("例如：E:/temp/attendance.csv")
                        onEditingFinished: Presets.lastExportPath = text
                    }
                    Button {
                        text: qsTr("另存为…")
                        onClicked: {
                            const path = testApi.selectSaveFile(qsTr("选择导出路径"),
                                "export-" + Qt.formatDateTime(new Date(), "yyyyMMdd-HHmmss") + ".csv",
                                "CSV (*.csv);;所有文件 (*)")
                            if (path) { filePath.text = path; Presets.lastExportPath = path }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    LabeledField {
                        label: qsTr("起始时间"); Layout.fillWidth: true
                        TextField { id: startTime; text: Presets.defaultStartTime; Layout.fillWidth: true }
                    }
                    LabeledField {
                        label: qsTr("结束时间"); Layout.fillWidth: true
                        TextField { id: endTime; text: Presets.defaultEndTime; Layout.fillWidth: true }
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("导出动作")

            Flow {
                width: parent.width
                spacing: Theme.spacingMd

                Button {
                    text: qsTr("导出人员 CSV")
                    highlighted: true
                    onClicked: {
                        const ok = exportManager.exportPersonsCsv(filePath.text)
                        Logger.logResult("导出人员 CSV", ok, ok ? filePath.text : exportManager.lastError)
                        page._record("exportManager.exportPersonsCsv", { path: filePath.text }, ok, exportManager.lastError || filePath.text)
                    }
                }
                Button {
                    text: qsTr("导出设备 CSV")
                    onClicked: {
                        const ok = exportManager.exportDeviceCsv(filePath.text)
                        Logger.logResult("导出设备 CSV", ok, ok ? filePath.text : exportManager.lastError)
                        page._record("exportManager.exportDeviceCsv", { path: filePath.text }, ok, exportManager.lastError || filePath.text)
                    }
                }
                Button {
                    text: qsTr("导出考勤 CSV")
                    onClicked: {
                        const ok = exportManager.exportAttendanceRecordsCsv(filePath.text,
                            page._parseDateTime(startTime.text), page._parseDateTime(endTime.text))
                        Logger.logResult("导出考勤 CSV", ok, ok ? filePath.text : exportManager.lastError)
                        page._record("exportManager.exportAttendanceRecordsCsv",
                            { path: filePath.text, startTime: startTime.text, endTime: endTime.text },
                            ok, exportManager.lastError || filePath.text)
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
