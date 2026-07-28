import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import AttendanceAdmin

/*
  通用数据表格：
    columns: [{ key, title, width?, align?, formatter?: function(v, row, index) }]
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
    property int minColWidth: 40

    signal rowClicked(int index, var row)
    signal rowDoubleClicked(int index, var row)

    // 拖拽调整列宽
    property var _userWidths: ({})
    property int _dragIdx: -1
    property real _dragStartX: 0
    property real _dragStartW: 0
    property real _dragNextStartW: 0
    property int _widthVersion: 0

    function _beginDrag(idx, mouseX) {
        _dragIdx = idx
        _dragStartX = mouseX
        _dragStartW = _colWidth(idx, root.width)
        _dragNextStartW = idx + 1 < columns.length ? _colWidth(idx + 1, root.width) : 0
    }

    function _doDrag(mouseX) {
        if (_dragIdx < 0) return
        const delta = mouseX - _dragStartX
        const newW = Math.max(minColWidth, _dragStartW + delta)
        _userWidths[_dragIdx] = newW
        _widthVersion++
    }

    function _endDrag() {
        _dragIdx = -1
    }

    function _colWidth(idx, totalWidth) {
        void _widthVersion
        if (_userWidths[idx] !== undefined)
            return _userWidths[idx]
        const col = columns[idx]
        if (col && col.width && col.width > 0)
            return col.width
        // 平均分配
        let fixedTotal = 0
        let flexCount = 0
        for (let i = 0; i < columns.length; ++i) {
            if (_userWidths[i] !== undefined)
                fixedTotal += _userWidths[i]
            else if (columns[i].width && columns[i].width > 0)
                fixedTotal += columns[i].width
            else
                ++flexCount
        }
        const remain = Math.max(120, totalWidth - fixedTotal - 16)
        return flexCount > 0 ? Math.max(80, Math.floor(remain / flexCount)) : 120
    }

    function _formatCell(col, row, idx) {
        if (!col || !row) return ""
        let v
        try { v = row[col.key] } catch (e) { v = "" }
        if (col.formatter)
            return col.formatter(v, row, idx !== undefined ? idx : -1)
        if (v === undefined || v === null) return ""
        if (v instanceof Date) return Qt.formatDateTime(v, "yyyy-MM-dd HH:mm:ss")
        return String(v)
    }

    function _columnWidth(col, totalWidth) {
        // 保持兼容旧 API
        for (let i = 0; i < columns.length; ++i) {
            if (columns[i] === col)
                return _colWidth(i, totalWidth)
        }
        if (col.width && col.width > 0) return col.width
        return 120
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
                        required property int index
                        width: root._colWidth(index, root.width)
                        height: parent.height

                        Label {
                            anchors.fill: parent
                            anchors.leftMargin: 6
                            anchors.rightMargin: 10
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

                        // 拖拽手柄
                        Rectangle {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: 6
                            color: "transparent"
                            z: 1

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.SplitHCursor
                                onPressed: function(mouse) { root._beginDrag(index, mouse.x) }
                                onPositionChanged: function(mouse) { root._doDrag(mouse.x) }
                                onReleased: root._endDrag()
                            }
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
                            required property int index
                            width: root._colWidth(index, root.width)
                            height: parent.height

                            Label {
                                anchors.fill: parent
                                anchors.leftMargin: 6
                                anchors.rightMargin: 6
                                text: root._formatCell(modelData, rowItem.modelData, rowItem.index)
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
                                obj[c.key] = root._formatCell(c, rowItem.modelData, rowItem.index)
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
