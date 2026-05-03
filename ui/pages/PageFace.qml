import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

Item {
    id: page

    property bool engineOk: false
    property bool feature1Ok: false
    property var rows: []

    function refresh() {
        rows = dataService.getAllFaceData()
        Logger.logResult("加载人脸库", true, "数量=" + rows.length)
    }

    Component.onCompleted: refresh()

    function _record(target, args, ok, detail) {
        History.record({
            direction: "INVOKE",
            target: target,
            payload: args,
            ok: ok,
            result: ok ? "OK" : ("FAIL: " + (detail || "")),
            category: "face"
        })
    }

    function _maskSecret(text) {
        if (!text || text.length === 0) return ""
        if (text.length <= 4) return "****"
        return text.slice(0, 2) + "****" + text.slice(text.length - 2)
    }

    function _parseOptionalDateTime(text) {
        const t = String(text).trim()
        if (!t.length) return new Date(NaN)
        const d = Date.fromLocaleString(Qt.locale(), t, "yyyy-MM-dd HH:mm:ss")
        return isNaN(d.getTime()) ? new Date(NaN) : d
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("人脸引擎与特征")
            subtitle: qsTr("ArcFace 初始化 · 提取特征 · 人脸库 CRUD")
            actions: [
                Button { text: qsTr("刷新人脸库"); onClicked: page.refresh() }
            ]
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("ArcFace 引擎")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingSm

                RowLayout {
                    Layout.fillWidth: true
                    LabeledField {
                        label: qsTr("AppId"); Layout.fillWidth: true
                        TextField {
                            id: appId
                            text: Presets.faceAppId
                            Layout.fillWidth: true
                            onEditingFinished: Presets.faceAppId = text
                        }
                    }
                    LabeledField {
                        label: qsTr("SdkKey"); Layout.fillWidth: true
                        TextField {
                            id: sdkKey
                            text: Presets.faceSdkKey
                            echoMode: TextInput.Password
                            Layout.fillWidth: true
                            onEditingFinished: Presets.faceSdkKey = text
                        }
                    }
                    Button {
                        text: qsTr("初始化引擎")
                        highlighted: true
                        onClicked: {
                            const ok = testApi.initFaceEngine(appId.text, sdkKey.text)
                            page.engineOk = ok
                            Logger.logResult("初始化人脸引擎", ok,
                                ok ? ("SdkKey=" + page._maskSecret(sdkKey.text)) : testApi.lastError)
                            page._record("testApi.initFaceEngine", { appId: appId.text, sdkKey: page._maskSecret(sdkKey.text) }, ok, testApi.lastError)
                        }
                    }
                    BadgeStatus {
                        text: page.engineOk ? "ok" : "offline"
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("特征提取")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingSm

                LabeledField {
                    label: qsTr("图片路径")
                    Layout.fillWidth: true

                    TextField {
                        id: imgPath
                        text: Presets.lastImagePath
                        Layout.fillWidth: true
                        onEditingFinished: Presets.lastImagePath = text
                    }
                    Button {
                        text: qsTr("浏览…")
                        onClicked: {
                            const path = testApi.selectImageFile()
                            if (path && path.length) {
                                imgPath.text = path
                                Presets.lastImagePath = path
                            }
                        }
                    }
                    Button {
                        text: qsTr("提取特征")
                        highlighted: true
                        onClicked: {
                            const b64 = testApi.extractFeatureBase64(imgPath.text)
                            const ok = b64.length > 0
                            featureText.text = b64
                            page.feature1Ok = ok
                            Logger.logResult("提取特征", ok, ok ? ("Base64 长度=" + b64.length) : testApi.lastError)
                            page._record("testApi.extractFeatureBase64", { imagePath: imgPath.text }, ok, testApi.lastError)
                        }
                    }
                    Label {
                        text: page.feature1Ok ? qsTr("已提取") : qsTr("未提取")
                        color: page.feature1Ok ? Theme.success : Theme.textMuted
                        font.pixelSize: Theme.fontSm
                    }
                }
                TextArea {
                    id: featureText
                    visible: false
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("人脸库")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingSm

                RowLayout {
                    Layout.fillWidth: true

                    LabeledField {
                        label: qsTr("工号"); Layout.fillWidth: true
                        TextField {
                            id: faceEmp
                            text: Presets.defaultEmployeeId
                            Layout.fillWidth: true
                        }
                    }
                    Button {
                        text: qsTr("新增（用上方特征）")
                        highlighted: true
                        enabled: page.feature1Ok
                        onClicked: {
                            const ok = testApi.addFaceDataBase64(faceEmp.text, featureText.text)
                            Logger.logResult("新增人脸特征", ok, "工号=" + faceEmp.text)
                            page._record("testApi.addFaceDataBase64", { employeeId: faceEmp.text }, ok, testApi.lastError)
                            if (ok) page.refresh()
                        }
                    }
                    Button {
                        text: qsTr("更新")
                        enabled: page.feature1Ok
                        onClicked: {
                            const ok = testApi.updateFaceDataBase64(faceEmp.text, featureText.text)
                            Logger.logResult("更新人脸特征", ok, "工号=" + faceEmp.text)
                            page._record("testApi.updateFaceDataBase64", { employeeId: faceEmp.text }, ok, testApi.lastError)
                            if (ok) page.refresh()
                        }
                    }
                    Button {
                        text: qsTr("删除")
                        onClicked: confirm.open()
                    }
                    Button {
                        text: qsTr("查询")
                        onClicked: {
                            const obj = dataService.getFaceDataByEmployeeId(faceEmp.text)
                            const ok = obj !== null
                            Logger.logResult("查询人脸特征", ok, ok ? ("size=" + obj.featureSize) : "未命中")
                            page._record("dataService.getFaceDataByEmployeeId", { employeeId: faceEmp.text }, ok)
                        }
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    columnSpacing: Theme.spacingMd
                    rowSpacing: Theme.spacingSm

                    LabeledField { label: qsTr("创建起"); Layout.fillWidth: true
                        TextField { id: cs; placeholderText: qsTr("可选 yyyy-MM-dd HH:mm:ss"); Layout.fillWidth: true } }
                    LabeledField { label: qsTr("创建止"); Layout.fillWidth: true
                        TextField { id: ce; placeholderText: qsTr("可选"); Layout.fillWidth: true } }
                    LabeledField { label: qsTr("更新起"); Layout.fillWidth: true
                        TextField { id: us; placeholderText: qsTr("可选"); Layout.fillWidth: true } }
                    LabeledField { label: qsTr("更新止"); Layout.fillWidth: true
                        TextField { id: ue; placeholderText: qsTr("可选"); Layout.fillWidth: true } }
                }

                Row {
                    spacing: Theme.spacingSm
                    Button {
                        text: qsTr("条件查询人脸库")
                        onClicked: {
                            const list = dataService.selectFaceData(
                                faceEmp.text.trim(),
                                page._parseOptionalDateTime(cs.text),
                                page._parseOptionalDateTime(ce.text),
                                page._parseOptionalDateTime(us.text),
                                page._parseOptionalDateTime(ue.text))
                            page.rows = list
                            Logger.logResult("条件查询人脸库", true, "数量=" + list.length)
                            page._record("dataService.selectFaceData",
                                { employeeId: faceEmp.text, createdStart: cs.text, createdEnd: ce.text,
                                  updatedStart: us.text, updatedEnd: ue.text }, true, "count=" + list.length)
                        }
                    }
                }

                DataTable {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    rows: page.rows
                    columns: [
                        { key: "id", title: "ID", width: 60, align: "right" },
                        { key: "employeeId", title: qsTr("工号"), width: 120 },
                        { key: "personName", title: qsTr("姓名"), width: 120 },
                        { key: "featureSize", title: qsTr("特征长度"), width: 100, align: "right" },
                        { key: "createdAt", title: qsTr("创建时间"), width: 170,
                          formatter: function(v) { return v ? Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss") : "" } },
                        { key: "updatedAt", title: qsTr("更新时间"),
                          formatter: function(v) { return v ? Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss") : "" } }
                    ]
                    onRowClicked: function(idx, row) {
                        faceEmp.text = row.employeeId
                    }
                }
            }
        }
    }

    ConfirmDialog {
        id: confirm
        message: qsTr("确认删除工号 ") + faceEmp.text + qsTr(" 的人脸特征？")
        onAccepted: {
            const ok = testApi.deleteFaceDataByEmployeeId(faceEmp.text)
            Logger.logResult("删除人脸特征", ok, "工号=" + faceEmp.text)
            page._record("testApi.deleteFaceDataByEmployeeId", { employeeId: faceEmp.text }, ok, testApi.lastError)
            if (ok) page.refresh()
        }
    }
}
