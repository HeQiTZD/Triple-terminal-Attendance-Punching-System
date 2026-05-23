import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: chart

    property var chartData: []       // [{ label, value }]
    property string title: ""
    property string xAxisLabel: ""
    property string yAxisLabel: ""
    property bool showGrid: true
    property bool showPoints: true
    property bool showArea: false
    property color lineColor: Theme.primary
    property color areaColor: Qt.rgba(0.11, 0.42, 1.0, 0.12)
    property int animationDuration: 300

    implicitWidth: parent ? parent.width : 400
    implicitHeight: 300

    readonly property real chartPadding: 50
    readonly property real chartWidth: width - chartPadding * 2
    readonly property real chartHeight: height - chartPadding * 2 - 30

    Rectangle {
        anchors.fill: parent
        color: "transparent"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingMd
            spacing: Theme.spacingSm

            Label {
                text: chart.title
                font.pixelSize: Theme.fontLg
                font.bold: true
                font.family: Theme.fontFamily
                color: Theme.text
                Layout.alignment: Qt.AlignHCenter
                visible: chart.title.length > 0
            }

            Item {
                id: chartArea
                Layout.fillWidth: true
                Layout.fillHeight: true

                Canvas {
                    id: canvas
                    anchors.fill: parent
                    antialiasing: true

                    property real animationProgress: 0
                    NumberAnimation {
                        id: animation
                        target: canvas
                        property: "animationProgress"
                        from: 0
                        to: 1
                        duration: chart.animationDuration
                        easing.type: Easing.OutQuad
                    }

                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)

                        if (!chart.chartData || chart.chartData.length < 2)
                            return

                        const padding = chart.chartPadding
                        const chartW = chart.chartWidth
                        const chartH = chart.chartHeight

                        let maxValue = 0
                        let minValue = Infinity
                        for (let i = 0; i < chart.chartData.length; i++) {
                            const v = chart.chartData[i].value || 0
                            if (v > maxValue) maxValue = v
                            if (v < minValue) minValue = v
                        }
                        if (maxValue === minValue) maxValue = minValue + 1
                        const range = maxValue - minValue

                        // Grid
                        if (chart.showGrid) {
                            ctx.strokeStyle = Theme.border
                            ctx.lineWidth = 1
                            ctx.setLineDash([2, 2])
                            for (let i = 0; i <= 5; i++) {
                                const y = padding + (chartH / 5) * i
                                ctx.beginPath()
                                ctx.moveTo(padding, y)
                                ctx.lineTo(padding + chartW, y)
                                ctx.stroke()
                            }
                            ctx.setLineDash([])
                        }

                        // Y-axis labels
                        ctx.fillStyle = Theme.textMuted
                        ctx.font = `${Theme.fontSm}px ${Theme.fontFamily}`
                        ctx.textAlign = "right"
                        for (let i = 0; i <= 5; i++) {
                            const value = maxValue - (range / 5) * i
                            const y = padding + (chartH / 5) * i
                            ctx.fillText(Math.round(value).toString(), padding - 8, y + 4)
                        }

                        // X-axis labels
                        const pointCount = chart.chartData.length
                        const xStep = pointCount > 1 ? chartW / (pointCount - 1) : chartW
                        ctx.textAlign = "center"
                        for (let i = 0; i < pointCount; i++) {
                            const x = padding + xStep * i
                            const label = chart.chartData[i].label || ""
                            ctx.fillText(label, x, padding + chartH + 15)
                        }

                        // Area fill
                        if (chart.showArea) {
                            ctx.fillStyle = chart.areaColor
                            ctx.beginPath()
                            const x0 = padding
                            const y0 = padding + chartH - ((chart.chartData[0].value - minValue) / range * chartH) * canvas.animationProgress
                            ctx.moveTo(x0, padding + chartH)
                            ctx.lineTo(x0, y0)
                            for (let i = 1; i < pointCount; i++) {
                                const x = padding + xStep * i
                                const y = padding + chartH - ((chart.chartData[i].value - minValue) / range * chartH) * canvas.animationProgress
                                ctx.lineTo(x, y)
                            }
                            const xEnd = padding + xStep * (pointCount - 1)
                            ctx.lineTo(xEnd, padding + chartH)
                            ctx.closePath()
                            ctx.fill()
                        }

                        // Line
                        ctx.strokeStyle = chart.lineColor
                        ctx.lineWidth = 2
                        ctx.lineJoin = "round"
                        ctx.lineCap = "round"
                        ctx.beginPath()
                        for (let i = 0; i < pointCount; i++) {
                            const x = padding + xStep * i
                            const y = padding + chartH - ((chart.chartData[i].value - minValue) / range * chartH) * canvas.animationProgress
                            if (i === 0) ctx.moveTo(x, y)
                            else ctx.lineTo(x, y)
                        }
                        ctx.stroke()

                        // Data points
                        if (chart.showPoints) {
                            for (let i = 0; i < pointCount; i++) {
                                const x = padding + xStep * i
                                const y = padding + chartH - ((chart.chartData[i].value - minValue) / range * chartH) * canvas.animationProgress
                                ctx.fillStyle = Theme.surface
                                ctx.strokeStyle = chart.lineColor
                                ctx.lineWidth = 2
                                ctx.beginPath()
                                ctx.arc(x, y, 4, 0, 2 * Math.PI)
                                ctx.fill()
                                ctx.stroke()
                            }
                        }
                    }

                    Component.onCompleted: {
                        animation.running = true
                    }

                    Connections {
                        target: chart
                        function onChartDataChanged() {
                            canvas.animationProgress = 0
                            animation.running = true
                            canvas.requestPaint()
                        }
                    }
                }

                MouseArea {
                    id: hoverArea
                    anchors.fill: parent
                    hoverEnabled: true
                    property int hoveredIndex: -1

                    ToolTip {
                        visible: hoverArea.containsMouse && hoverArea.hoveredIndex >= 0
                                 && hoverArea.hoveredIndex < chart.chartData.length
                        text: {
                            const idx = hoverArea.hoveredIndex
                            if (idx < 0 || idx >= chart.chartData.length) return ""
                            const item = chart.chartData[idx]
                            return `${item.label || ""}: ${item.value || 0}`
                        }
                    }

                    onPositionChanged: function(mouse) {
                        const padding = chart.chartPadding
                        const pointCount = chart.chartData.length
                        const xStep = pointCount > 1 ? chart.chartWidth / (pointCount - 1) : chart.chartWidth
                        let closest = -1
                        let minDist = 12
                        for (let i = 0; i < pointCount; i++) {
                            const px = padding + xStep * i
                            const dist = Math.abs(mouse.x - px)
                            if (dist < minDist) {
                                minDist = dist
                                closest = i
                            }
                        }
                        hoveredIndex = closest
                    }

                    onExited: { hoveredIndex = -1 }
                }
            }
        }
    }
}
