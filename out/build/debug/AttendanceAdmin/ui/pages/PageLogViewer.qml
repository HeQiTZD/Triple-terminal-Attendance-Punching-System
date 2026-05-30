import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import AttendanceAdmin

Item {
    id: page

    required property var sessionManager
    required property var fileService

    Component.onCompleted: {
        if (fileService) {
            LogReader.init(fileService)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd

        ToolBarRow {
            Layout.fillWidth: true
            title: qsTr("日志查看")
            subtitle: qsTr("共 ") + LogReader.filteredCount + qsTr(" 条日志")
        }

        Card {
            Layout.fillWidth: true
            title: qsTr("筛选条件")

            ColumnLayout {
                width: parent.width
                spacing: Theme.spacingSm

                GridLayout {
                    Layout.fillWidth: true
                    columns: 6
                    rowSpacing: Theme.spacingSm
                    columnSpacing: Theme.spacingMd

                    LabeledField { label: qsTr("起始日期"); Layout.fillWidth: true
                        TextField {
                            id: dateFrom
                            Layout.fillWidth: true
                            placeholderText: "yyyy-MM-dd"
                        }
                    }
                    LabeledField { label: qsTr("结束日期"); Layout.fillWidth: true
                        TextField {
                            id: dateTo
                            Layout.fillWidth: true
                            placeholderText: "yyyy-MM-dd"
                        }
                    }
                    LabeledField { label: qsTr("级别"); Layout.fillWidth: true
                        ComboBox {
                            id: levelCombo
                            Layout.fillWidth: true
                            model: [
                                { text: qsTr("ALL"),   value: "" },
                                { text: qsTr("INFO"),  value: "INFO" },
                                { text: qsTr("WARN"),  value: "WARN" },
                                { text: qsTr("ERROR"), value: "ERROR" }
                            ]
                            textRole: "text"
                            valueRole: "value"
                        }
                    }
                    LabeledField { label: qsTr("类型"); Layout.fillWidth: true
                        ComboBox {
                            id: typeCombo
                            Layout.fillWidth: true
                            model: [
                                { text: qsTr("ALL"),    value: "" },
                                { text: qsTr("LOGIN"),  value: "LOGIN" },
                                { text: qsTr("DATA"),   value: "DATA" },
                                { text: qsTr("CONFIG"), value: "CONFIG" },
                                { text: qsTr("SYSTEM"), value: "SYSTEM" }
                            ]
                            textRole: "text"
                            valueRole: "value"
                        }
                    }
                    LabeledField { label: qsTr("关键词"); Layout.fillWidth: true
                        TextField {
                            id: keywordField
                            Layout.fillWidth: true
                            placeholderText: qsTr("搜索日志内容...")
                        }
                    }

                    Row {
                        spacing: Theme.spacingSm
                        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom

                        Button {
                            text: qsTr("重置")
                            onClicked: {
                                dateFrom.text = ""
                                dateTo.text = ""
                                levelCombo.currentIndex = 0
                                typeCombo.currentIndex = 0
                                keywordField.text = ""
                                LogReader.clearFilters()
                            }
                        }
                        Button {
                            text: qsTr("导出")
                            highlighted: true
                            enabled: LogReader.filteredCount > 0
                            onClicked: exportDialog.open()
                        }
                    }
                }
            }
        }

        Card {
            Layout.fillWidth: true
            Layout.fillHeight: true
            stretchContent: true
            title: qsTr("日志列表")

            headerRight: Label {
                text: LogReader.loading ? qsTr("加载中...") : ""
                color: Theme.textSubtle
                font.pixelSize: Theme.fontSm
                font.family: Theme.fontFamily
            }

            // Custom table instead of DataTable for level/type badges and row coloring
            Rectangle {
                anchors.fill: parent
                color: Theme.bg
                border.color: Theme.border
                border.width: 1
                radius: Theme.radiusSm
                clip: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    // Table header
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        color: Theme.surfaceAlt

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: Theme.spacingSm
                            anchors.rightMargin: Theme.spacingSm
                            spacing: 0

                            // Timestamp
                            Label {
                                width: 180
                                height: parent.height
                                text: qsTr("时间")
                                color: Theme.text
                                font.pixelSize: Theme.fontSm
                                font.family: Theme.fontFamily
                                font.bold: true
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 6
                            }
                            // Level
                            Label {
                                width: 70
                                height: parent.height
                                text: qsTr("级别")
                                color: Theme.text
                                font.pixelSize: Theme.fontSm
                                font.family: Theme.fontFamily
                                font.bold: true
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 6
                            }
                            // Type
                            Label {
                                width: 90
                                height: parent.height
                                text: qsTr("类型")
                                color: Theme.text
                                font.pixelSize: Theme.fontSm
                                font.family: Theme.fontFamily
                                font.bold: true
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 6
                            }
                            // Message
                            Label {
                                width: parent.parent.width - 180 - 70 - 90 - Theme.spacingSm * 2
                                height: parent.height
                                text: qsTr("消息")
                                color: Theme.text
                                font.pixelSize: Theme.fontSm
                                font.family: Theme.fontFamily
                                font.bold: true
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: 6
                            }
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

                    // Table body
                    ListView {
                        id: logListView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: LogReader.filteredEntries
                        clip: true
                        ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

                        delegate: Rectangle {
                            id: rowDelegate
                            required property var model
                            required property int index
                            width: logListView.width
                            height: 28
                            color: model.level === "ERROR"
                                   ? Qt.rgba(1, 0.9, 0.9, 1)  // light red background for ERROR rows
                                   : (index % 2 === 1 ? Theme.surfaceAlt : "transparent")

                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: Theme.spacingSm
                                anchors.rightMargin: Theme.spacingSm
                                spacing: 0

                                // Timestamp (monospace)
                                Label {
                                    width: 180
                                    height: parent.height
                                    text: model.timestamp || ""
                                    color: Theme.textMuted
                                    font.pixelSize: Theme.fontSm
                                    font.family: Theme.fontMono
                                    verticalAlignment: Text.AlignVCenter
                                    leftPadding: 6
                                    elide: Text.ElideRight
                                }

                                // Level badge
                                Item {
                                    width: 70
                                    height: parent.height

                                    Rectangle {
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: parent.left
                                        anchors.leftMargin: 6
                                        width: levelLabel.implicitWidth + 12
                                        height: 20
                                        radius: Theme.radiusSm
                                        color: {
                                            switch (model.level) {
                                            case "INFO":  return Qt.rgba(0.08, 0.4, 0.75, 0.15)
                                            case "WARN":  return Qt.rgba(0.6, 0.24, 0.02, 0.15)
                                            case "ERROR": return Qt.rgba(0.78, 0.16, 0.16, 0.15)
                                            default:      return Qt.rgba(0.42, 0.45, 0.5, 0.15)
                                            }
                                        }

                                        Label {
                                            id: levelLabel
                                            anchors.centerIn: parent
                                            text: model.level || ""
                                            font.pixelSize: Theme.fontXs
                                            font.family: Theme.fontMono
                                            font.bold: true
                                            color: {
                                                switch (model.level) {
                                                case "INFO":  return Theme.info
                                                case "WARN":  return Theme.warning
                                                case "ERROR": return Theme.danger
                                                default:      return Theme.textMuted
                                                }
                                            }
                                        }
                                    }
                                }

                                // Type badge
                                Item {
                                    width: 90
                                    height: parent.height

                                    Rectangle {
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: parent.left
                                        anchors.leftMargin: 6
                                        width: typeLabel.implicitWidth + 12
                                        height: 20
                                        radius: Theme.radiusSm
                                        color: Qt.rgba(0.42, 0.45, 0.5, 0.12)

                                        Label {
                                            id: typeLabel
                                            anchors.centerIn: parent
                                            text: model.type || ""
                                            font.pixelSize: Theme.fontXs
                                            font.family: Theme.fontMono
                                            color: Theme.textMuted
                                        }
                                    }
                                }

                                // Message
                                Label {
                                    width: parent.parent.width - 180 - 70 - 90 - Theme.spacingSm * 2
                                    height: parent.height
                                    text: model.message || ""
                                    color: model.level === "ERROR" ? Theme.danger : Theme.text
                                    font.pixelSize: Theme.fontSm
                                    font.family: Theme.fontMono
                                    verticalAlignment: Text.AlignVCenter
                                    leftPadding: 6
                                    elide: Text.ElideRight
                                }
                            }
                        }

                        // Empty state
                        Label {
                            anchors.centerIn: parent
                            visible: logListView.count === 0
                            text: LogReader.loading ? qsTr("正在加载...") :
                                  LogReader.lastError ? LogReader.lastError :
                                  qsTr("暂无日志数据")
                            color: Theme.textSubtle
                            font.pixelSize: Theme.fontMd
                            font.family: Theme.fontFamily
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: exportDialog
        title: qsTr("导出日志")
        nameFilters: [qsTr("日志文件 (*.log)"), qsTr("文本文件 (*.txt)")]
        fileMode: FileDialog.SaveFile
        defaultSuffix: "log"
        currentFile: "logs_" + Qt.formatDateTime(new Date(), "yyyyMMdd_HHmmss") + ".log"
        onAccepted: {
            LogReader.exportLogs(selectedFile)
        }
    }

    // Apply filters when inputs change
    Connections {
        target: LogReader
        function onLogsLoaded() {
            // Re-apply current filters after loading
        }
    }

    // Debounce timer for keyword input
    Timer {
        id: keywordTimer
        interval: 300
        onTriggered: LogReader.setFilterKeyword(keywordField.text)
    }

    // Watch for filter changes
    Binding {
        target: LogReader
        property: "filterLevel"
        value: levelCombo.currentValue || ""
        when: levelCombo.currentValue !== undefined
    }

    Binding {
        target: LogReader
        property: "filterType"
        value: typeCombo.currentValue || ""
        when: typeCombo.currentValue !== undefined
    }

    Connections {
        target: keywordField
        function onTextChanged() {
            keywordTimer.restart()
        }
    }

    Connections {
        target: dateFrom
        function onTextChanged() {
            LogReader.setFilterDateRange(dateFrom.text, dateTo.text)
        }
    }

    Connections {
        target: dateTo
        function onTextChanged() {
            LogReader.setFilterDateRange(dateFrom.text, dateTo.text)
        }
    }
}
