import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceServer

/*
  通用数据表格：
    columns: [{ key, title, width?, align?, formatter?: function(v, row) }]
    rows:    JS 数组，每个元素是普通对象或 QObject（通过属性访问）
  支持：行高亮、行选中（currentRow / selectedRow）、双击、右键菜单（复制行 JSON）。
*/
Rectangle {
    id: root
    color: Theme.bg
    border.color: Theme.border
    border.width: 1
    radius: Theme.radiusSm
    clip: true

    property var columns: []
    property var rows: []
    property int currentRow: -1
    property var selectedRow: currentRow >= 0 && currentRow < (rows ? rows.length : 0) ? rows[currentRow] : null
    property string emptyText: qsTr("（无数据）")
    property bool stripeRows: true

    signal rowClicked(int index, var row)
    signal rowDoubleClicked(int index, var row)

    function _formatCell(col, row) {
        if (!col || !row) return ""
        let v
        try { v = row[col.key] } catch (e) { v = "" }
        if (col.formatter)
            return col.formatter(v, row)
        if (v === undefined || v === null) return ""
        if (v instanceof Date) return Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss")
        return String(v)
    }

    function _columnWidth(col, totalWidth) {
        if (col.width && col.width > 0) return col.width
        // 默认平均分配
        let fixedTotal = 0
        let flexCount = 0
        for (let i = 0; i < columns.length; ++i) {
            if (columns[i].width && columns[i].width > 0)
                fixedTotal += columns[i].width
            else
                ++flexCount
        }
        const remain = Math.max(120, totalWidth - fixedTotal - 16)
        return flexCount > 0 ? Math.max(80, Math.floor(remain / flexCount)) : 120
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ===== 表头 =====
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: Theme.surfaceAlt

            Row {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingSm
                anchors.rightMargin: Theme.spacingSm
                spacing: 0

                Repeater {
                    model: root.columns
                    delegate: Item {
                        required property var modelData
                        width: root._columnWidth(modelData, root.width)
                        height: parent.height

                        Label {
                            anchors.fill: parent
                            anchors.leftMargin: 6
                            anchors.rightMargin: 6
                            text: modelData.title || modelData.key
                            color: Theme.text
                            font.pixelSize: Theme.fontSm
                            font.family: Theme.fontFamily
                            font.bold: true
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: modelData.align === "right" ? Text.AlignRight :
                                                 modelData.align === "center" ? Text.AlignHCenter :
                                                 Text.AlignLeft
                            elide: Text.ElideRight
                        }

                        Rectangle {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: 1
                            color: Theme.border
                        }
                    }
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: Theme.border }

        // ===== 数据行 =====
        ListView {
            id: lv
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: root.rows
            clip: true
            interactive: true
            ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

            delegate: Rectangle {
                id: rowItem
                required property var modelData
                required property int index
                width: lv.width
                height: 28
                color: index === root.currentRow
                       ? Theme.selected
                       : (mouseArea.containsMouse
                          ? Theme.hover
                          : (root.stripeRows && index % 2 === 1 ? Theme.surfaceAlt : "transparent"))

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingSm
                    anchors.rightMargin: Theme.spacingSm
                    spacing: 0

                    Repeater {
                        model: root.columns
                        delegate: Item {
                            required property var modelData
                            width: root._columnWidth(modelData, root.width)
                            height: parent.height

                            Label {
                                anchors.fill: parent
                                anchors.leftMargin: 6
                                anchors.rightMargin: 6
                                text: root._formatCell(modelData, rowItem.modelData)
                                color: rowItem.index === root.currentRow ? Theme.text : Theme.textMuted
                                font.pixelSize: Theme.fontSm
                                font.family: Theme.fontFamily
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: modelData.align === "right" ? Text.AlignRight :
                                                     modelData.align === "center" ? Text.AlignHCenter :
                                                     Text.AlignLeft
                                elide: Text.ElideRight
                            }
                        }
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: function(mouse) {
                        root.currentRow = rowItem.index
                        root.rowClicked(rowItem.index, rowItem.modelData)
                        if (mouse.button === Qt.RightButton)
                            ctxMenu.popup()
                    }
                    onDoubleClicked: {
                        root.currentRow = rowItem.index
                        root.rowDoubleClicked(rowItem.index, rowItem.modelData)
                    }
                }

                Menu {
                    id: ctxMenu
                    MenuItem {
                        text: qsTr("复制行 JSON")
                        onTriggered: {
                            const obj = {}
                            for (let i = 0; i < root.columns.length; ++i) {
                                const c = root.columns[i]
                                obj[c.key] = root._formatCell(c, rowItem.modelData)
                            }
                            tmpEdit.text = JSON.stringify(obj)
                            tmpEdit.selectAll()
                            tmpEdit.copy()
                        }
                    }
                }
            }

            // 空提示
            Label {
                anchors.centerIn: parent
                visible: lv.count === 0
                text: root.emptyText
                color: Theme.textSubtle
                font.pixelSize: Theme.fontMd
                font.family: Theme.fontFamily
            }
        }
    }

    // 用于剪贴板复制
    TextEdit { id: tmpEdit; visible: false }
}
