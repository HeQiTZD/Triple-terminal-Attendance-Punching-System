import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts

import AttendanceAdmin

Item {
    id: root

    property var slices: []
    property var sliceMeta: []
    property int featuredIndex: -1
    property int totalCount: 0

    readonly property real labelMinPercent: 5
    readonly property real chartStartAngle: 0
    readonly property real basePieSize: 0.58
    readonly property real highlightedPieSize: 0.64
    readonly property real pieHoleSize: 0.34

    Timer {
        id: hoverClearTimer
        interval: 80
        repeat: false
        onTriggered: {
            root.featuredIndex = -1
            root.clearHighlightSlice()
        }
    }

    Rectangle {
        id: chartPanel
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: legendRow.top
        color: "transparent"

        ChartView {
            id: chart
            anchors.fill: parent
            antialiasing: true
            backgroundColor: "transparent"
            legend.visible: false
            animationOptions: ChartView.NoAnimation
            margins.top: 16
            margins.bottom: 16
            margins.left: 24
            margins.right: 24

            PieSeries {
                id: innerBand
                size: 0.5
                holeSize: 0.38
            }

            PieSeries {
                id: pieSeries
                size: root.basePieSize
                holeSize: root.pieHoleSize
                startAngle: root.chartStartAngle

                onHovered: function(slice, state) {
                    if (state) {
                        hoverClearTimer.stop()
                        let idx = -1
                        for (let j = 0; j < pieSeries.count; ++j) {
                            if (pieSeries.at(j) === slice) {
                                idx = j
                                break
                            }
                        }
                        root.featuredIndex = idx
                        root.showHighlightSlice(idx)
                    } else {
                        hoverClearTimer.restart()
                    }
                }
            }

            PieSeries {
                id: highlightSeries
                size: root.highlightedPieSize
                holeSize: root.pieHoleSize
                visible: root.featuredIndex >= 0

                onHovered: function(slice, state) {
                    if (state)
                        hoverClearTimer.stop()
                    else
                        hoverClearTimer.restart()
                }
            }
        }

        Item {
            id: hubOverlay
            anchors.centerIn: chart
            width: hubContent.implicitWidth
            height: hubContent.implicitHeight
            z: 1
            enabled: false

            ColumnLayout {
                id: hubContent
                anchors.centerIn: parent
                spacing: 4

                ColumnLayout {
                    visible: root.featuredIndex >= 0
                    spacing: 2
                    Layout.alignment: Qt.AlignHCenter

                    Text {
                        text: root.featuredIndex >= 0 && root.featuredIndex < root.sliceMeta.length
                              ? root.sliceMeta[root.featuredIndex].label : ""
                        color: root.featuredIndex >= 0 && root.featuredIndex < root.sliceMeta.length
                               ? root.sliceMeta[root.featuredIndex].color : Theme.text
                        font.pixelSize: Theme.fontLg
                        font.family: Theme.fontFamily
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }

                    RowLayout {
                        spacing: Theme.spacingXs
                        Layout.alignment: Qt.AlignHCenter

                        Text {
                            text: root.featuredIndex >= 0 && root.featuredIndex < root.sliceMeta.length
                                  ? String(root.sliceMeta[root.featuredIndex].count) : ""
                            color: Theme.text
                            font.pixelSize: Theme.fontXl
                            font.family: Theme.fontFamily
                            font.bold: true
                        }
                        Text {
                            text: qsTr("人")
                            color: Theme.textMuted
                            font.pixelSize: Theme.fontMd
                            font.family: Theme.fontFamily
                        }
                    }
                }

                Text {
                    visible: root.featuredIndex < 0 && root.totalCount > 0
                    text: qsTr("共 %1 人").arg(root.totalCount)
                    color: Theme.textSubtle
                    font.pixelSize: Theme.fontMd
                    font.family: Theme.fontFamily
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                }
            }
        }
    }

    RowLayout {
        id: legendRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 26
        spacing: Theme.spacingMd
        visible: root.slices && root.slices.length > 0

        Item {
            Layout.fillWidth: true
        }

        Repeater {
            model: root.slices

            delegate: RowLayout {
                id: legendDelegate
                required property var modelData

                spacing: Theme.spacingXs

                Rectangle {
                    width: 10
                    height: 10
                    radius: 5
                    color: legendDelegate.modelData.color
                    Layout.alignment: Qt.AlignVCenter
                }

                Text {
                    text: legendDelegate.modelData.label || legendDelegate.modelData.status || ""
                    color: Theme.textMuted
                    font.pixelSize: Theme.fontSm
                    font.family: Theme.fontFamily
                    elide: Text.ElideRight
                    Layout.maximumWidth: 56
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }
    }

    function withAlpha(color, alpha) {
        const c = Qt.color(color)
        return Qt.rgba(c.r, c.g, c.b, alpha)
    }

    function formatPercent(count, total) {
        if (total <= 0)
            return "0%"
        const pct = Math.round(count * 1000 / total) / 10
        return (pct % 1 === 0 ? String(Math.round(pct)) : pct.toFixed(1)) + "%"
    }

    function percentValue(count, total) {
        if (total <= 0)
            return 0
        return count * 100 / total
    }

    function clearHighlightSlice() {
        highlightSeries.clear()
    }

    function showHighlightSlice(index) {
        highlightSeries.clear()
        if (index < 0 || index >= root.sliceMeta.length || root.totalCount <= 0)
            return

        const meta = root.sliceMeta[index]
        highlightSeries.startAngle = root.chartStartAngle + meta.startAngle
        highlightSeries.endAngle = root.chartStartAngle + meta.endAngle

        const slice = highlightSeries.append("", meta.count)
        slice.color = meta.color
        slice.borderWidth = 0
        slice.labelVisible = false
    }

    function rebuild() {
        if (chart.width <= 0 || chart.height <= 0)
            return

        innerBand.clear()
        pieSeries.clear()
        highlightSeries.clear()
        root.sliceMeta = []
        root.featuredIndex = -1
        root.totalCount = 0

        if (!root.slices || root.slices.length === 0)
            return

        let total = 0
        for (let i = 0; i < root.slices.length; ++i) {
            const item = root.slices[i]
            const count = item.value !== undefined ? item.value : item.count
            total += count
        }
        root.totalCount = total

        const meta = []
        let cumulative = 0
        for (let i = 0; i < root.slices.length; ++i) {
            const item = root.slices[i]
            const count = item.value !== undefined ? item.value : item.count
            const labelText = item.label || item.status || ""
            const sliceColor = item.color
            const pctLabel = formatPercent(count, total)
            const pct = percentValue(count, total)
            const startAngle = total > 0 ? cumulative * 360 / total : 0
            const endAngle = total > 0 ? (cumulative + count) * 360 / total : 0

            const innerSlice = innerBand.append("", count)
            innerSlice.color = withAlpha(sliceColor, 0.35)
            innerSlice.borderWidth = 0
            innerSlice.labelVisible = false

            const slice = pieSeries.append("", count)
            slice.color = sliceColor
            slice.label = labelText + " " + pctLabel
            slice.labelVisible = count > 0 && pct >= root.labelMinPercent
            slice.labelPosition = PieSlice.LabelOutside
            slice.labelArmLengthFactor = 0.16
            slice.labelColor = sliceColor
            slice.borderWidth = 0
            slice.exploded = false

            meta.push({
                label: labelText,
                count: count,
                color: sliceColor,
                startAngle: startAngle,
                endAngle: endAngle
            })
            cumulative += count
        }
        root.sliceMeta = meta
    }

    function scheduleRebuild() {
        Qt.callLater(function() {
            if (chart.width <= 0 || chart.height <= 0)
                return
            rebuild()
        })
    }

    onSlicesChanged: scheduleRebuild()

    Connections {
        target: chart
        function onWidthChanged() { root.scheduleRebuild() }
        function onHeightChanged() { root.scheduleRebuild() }
    }

    Connections {
        target: chartPanel
        function onWidthChanged() { root.scheduleRebuild() }
        function onHeightChanged() { root.scheduleRebuild() }
    }

    Component.onCompleted: scheduleRebuild()
}
