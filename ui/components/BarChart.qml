import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: chart

    property var chartData: []
    property string title: ""
    property string xAxisLabel: ""
    property string yAxisLabel: ""
    property bool showGrid: true
    property bool showValues: true
    property bool horizontal: false
    property int animationDuration: 300

    implicitWidth: parent ? parent.width : 400
    implicitHeight: 300

    readonly property real chartPadding: 50
    readonly property real chartWidth: horizontal ? width - chartPadding * 2 - 40 : width - chartPadding * 2
    readonly property real chartHeight: horizontal ? height - chartPadding * 2 : height - chartPadding * 2 - 30

    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        radius: Theme.radiusMd

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

                    property var animationProgress: 0
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

                        if (!chart.chartData || chart.chartData.length === 0)
                            return

                        const padding = chart.chartPadding
                        const chartW = chart.chartWidth
                        const chartH = chart.chartHeight

                        if (chart.showGrid) {
                            ctx.strokeStyle = Theme.border
                            ctx.lineWidth = 1
                            ctx.setLineDash([2, 2])

                            if (!chart.horizontal) {
                                for (let i = 0; i <= 5; i++) {
                                    const y = padding + (chartH / 5) * i
                                    ctx.beginPath()
                                    ctx.moveTo(padding, y)
                                    ctx.lineTo(padding + chartW, y)
                                    ctx.stroke()
                                }
                            } else {
                                for (let i = 0; i <= 5; i++) {
                                    const x = padding + (chartW / 5) * i
                                    ctx.beginPath()
                                    ctx.moveTo(x, padding)
                                    ctx.lineTo(x, padding + chartH)
                                    ctx.stroke()
                                }
                            }

                            ctx.setLineDash([])
                        }

                        let maxValue = 0
                        for (let i = 0; i < chart.chartData.length; i++) {
                            const value = chart.chartData[i].value || 0
                            if (value > maxValue) maxValue = value
                        }

                        if (maxValue === 0) maxValue = 100

                        const barCount = chart.chartData.length
                        const barSpacing = chart.horizontal ? chartH / (barCount * 1.5) : chartW / (barCount * 1.5)
                        const barWidth = chart.horizontal ? chartH / (barCount * 1.5) * 0.6 : chartW / (barCount * 1.5) * 0.6

                        for (let i = 0; i < barCount; i++) {
                            const item = chart.chartData[i]
                            const value = (item.value || 0) * canvas.animationProgress
                            const color = item.color || Theme.chartColor(i)

                            if (!chart.horizontal) {
                                const barHeight = (value / maxValue) * chartH
                                const x = padding + i * barSpacing + barSpacing / 2
                                const y = padding + chartH - barHeight

                                ctx.fillStyle = color
                                ctx.beginPath()
                                ctx.roundRect(x, y, barWidth, barHeight, [2, 2, 0, 0])
                                ctx.fill()

                                if (chart.showValues && value > 0) {
                                    ctx.fillStyle = Theme.text
                                    ctx.font = `${Theme.fontSm}px ${Theme.fontFamily}`
                                    ctx.textAlign = "center"
                                    ctx.fillText(Math.round(value).toString(), x + barWidth / 2, y - 5)
                                }

                                ctx.fillStyle = Theme.textMuted
                                ctx.font = `${Theme.fontSm}px ${Theme.fontFamily}`
                                ctx.textAlign = "center"
                                const label = item.label || ""
                                ctx.fillText(label, x + barWidth / 2, padding + chartH + 15)
                            } else {
                                const barLength = (value / maxValue) * chartW
                                const y = padding + i * barSpacing + barSpacing / 2
                                const x = padding

                                ctx.fillStyle = color
                                ctx.beginPath()
                                ctx.roundRect(x, y, barLength, barWidth, [2, 2, 0, 0])
                                ctx.fill()

                                if (chart.showValues && value > 0) {
                                    ctx.fillStyle = Theme.text
                                    ctx.font = `${Theme.fontSm}px ${Theme.fontFamily}`
                                    ctx.textAlign = "left"
                                    ctx.fillText(Math.round(value).toString(), x + barLength + 5, y + barWidth / 2 + 4)
                                }

                                ctx.fillStyle = Theme.textMuted
                                ctx.font = `${Theme.fontSm}px ${Theme.fontFamily}`
                                ctx.textAlign = "right"
                                const label = item.label || ""
                                ctx.fillText(label, x - 5, y + barWidth / 2 + 4)
                            }
                        }

                        if (!chart.horizontal) {
                            ctx.fillStyle = Theme.textMuted
                            ctx.font = `${Theme.fontSm}px ${Theme.fontFamily}`
                            ctx.textAlign = "right"
                            for (let i = 0; i <= 5; i++) {
                                const value = maxValue - (maxValue / 5) * i
                                const y = padding + (chartH / 5) * i
                                ctx.fillText(Math.round(value).toString(), padding - 8, y + 4)
                            }
                        }
                    }

                    Component.onCompleted: {
                        animation.running = true
                    }

                    Connections {
                        target: chart
                        function onChartDataChanged() {
                            animationProgress = 0
                            animation.running = true
                            requestPaint()
                        }
                    }
                }

                MouseArea {
                    id: hoverArea
                    anchors.fill: parent
                    hoverEnabled: true

                    ToolTip {
                        id: tooltip
                        visible: parent.containsMouse && hoveredIndex >= 0 && hoveredIndex < chart.chartData.length
                        text: {
                            if (hoveredIndex < 0 || hoveredIndex >= chart.chartData.length)
                                return ""
                            const item = chart.chartData[hoveredIndex]
                            return `${item.label || ""}: ${item.value || 0}`
                        }
                    }

                    property int hoveredIndex: -1

                    onMouseXChanged: {
                        updateHoveredIndex(mouseX, mouseY)
                    }

                    onMouseYChanged: {
                        updateHoveredIndex(mouseX, mouseY)
                    }

                    onExited: {
                        hoveredIndex = -1
                    }

                    function updateHoveredIndex(mx, my) {
                        const padding = chart.chartPadding
                        const chartW = chart.chartWidth
                        const chartH = chart.chartHeight

                        if (!chart.horizontal) {
                            const barCount = chart.chartData.length
                            const barSpacing = chartW / (barCount * 1.5)
                            const barWidth = chartW / (barCount * 1.5) * 0.6

                            for (let i = 0; i < barCount; i++) {
                                const x = padding + i * barSpacing + barSpacing / 2
                                if (mx >= x && mx <= x + barWidth && my >= padding && my <= padding + chartH) {
                                    hoveredIndex = i
                                    return
                                }
                            }
                        } else {
                            const barCount = chart.chartData.length
                            const barSpacing = chartH / (barCount * 1.5)
                            const barWidth = chartH / (barCount * 1.5) * 0.6

                            for (let i = 0; i < barCount; i++) {
                                const y = padding + i * barSpacing + barSpacing / 2
                                if (my >= y && my <= y + barWidth && mx >= padding && mx <= padding + chartW) {
                                    hoveredIndex = i
                                    return
                                }
                            }
                        }

                        hoveredIndex = -1
                    }
                }
            }
        }
    }
}
