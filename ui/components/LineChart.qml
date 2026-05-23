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
    property bool showPoints: true
    property bool showArea: false
    property int animationDuration: 300

    implicitWidth: parent ? parent.width : 400
    implicitHeight: 300

    readonly property real chartPadding: 50
    readonly property real chartWidth: width - chartPadding * 2
    readonly property real chartHeight: height - chartPadding * 2 - 30

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

                            for (let i = 0; i <= 5; i++) {
                                const y = padding + (chartH / 5) * i
                                ctx.beginPath()
                                ctx.moveTo(padding, y)
                                ctx.lineTo(padding + chartW, y)
                                ctx.stroke()
                            }

                            ctx.setLineDash([])
                        }

                        let maxValue = 0
                        let minValue = 0
                        for (let i = 0; i < chart.chartData.length; i++) {
                            const value = chart.chartData[i].value || 0
                            if (value > maxValue) maxValue = value
                            if (value < minValue) minValue = value
                        }

                        if (maxValue === minValue) {
                            maxValue = minValue + 100
                            minValue = 0
                        }

                        const range = maxValue - minValue
                        const pointSpacing = chartW / (chart.chartData.length - 1 || 1)

                        if (chart.showArea) {
                            ctx.fillStyle = Theme.primarySubtle
                            ctx.globalAlpha = 0.3 * canvas.animationProgress
                            ctx.beginPath()
                            ctx.moveTo(padding, padding + chartH)

                            for (let i = 0; i < chart.chartData.length; i++) {
                                const x = padding + i * pointSpacing
                                const value = chart.chartData[i].value || 0
                                const y = padding + chartH - ((value - minValue) / range) * chartH * canvas.animationProgress
                                ctx.lineTo(x, y)
                            }

                            ctx.lineTo(padding + (chart.chartData.length - 1) * pointSpacing, padding + chartH)
                            ctx.closePath()
                            ctx.fill()
                            ctx.globalAlpha = 1.0
                        }

                        ctx.strokeStyle = Theme.primary
                        ctx.lineWidth = 2
                        ctx.lineCap = "round"
                        ctx.lineJoin = "round"
                        ctx.beginPath()

                        for (let i = 0; i < chart.chartData.length; i++) {
                            const x = padding + i * pointSpacing
                            const value = chart.chartData[i].value || 0
                            const y = padding + chartH - ((value - minValue) / range) * chartH * canvas.animationProgress

                            if (i === 0) {
                                ctx.moveTo(x, y)
                            } else {
                                ctx.lineTo(x, y)
                            }
                        }

                        ctx.stroke()

                        if (chart.showPoints) {
                            for (let i = 0; i < chart.chartData.length; i++) {
                                const x = padding + i * pointSpacing
                                const value = chart.chartData[i].value || 0
                                const y = padding + chartH - ((value - minValue) / range) * chartH * canvas.animationProgress

                                ctx.fillStyle = Theme.surface
                                ctx.beginPath()
                                ctx.arc(x, y, 4, 0, Math.PI * 2)
                                ctx.fill()

                                ctx.strokeStyle = Theme.primary
                                ctx.lineWidth = 2
                                ctx.beginPath()
                                ctx.arc(x, y, 4, 0, Math.PI * 2)
                                ctx.stroke()
                            }
                        }

                        ctx.fillStyle = Theme.textMuted
                        ctx.font = `${Theme.fontSm}px ${Theme.fontFamily}`
                        ctx.textAlign = "center"

                        for (let i = 0; i < chart.chartData.length; i++) {
                            const x = padding + i * pointSpacing
                            const label = chart.chartData[i].label || ""
                            ctx.fillText(label, x, padding + chartH + 20)
                        }

                        ctx.textAlign = "right"
                        for (let i = 0; i <= 5; i++) {
                            const value = maxValue - (range / 5) * i
                            const y = padding + (chartH / 5) * i
                            ctx.fillText(Math.round(value).toString(), padding - 8, y + 4)
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

                    property int hoveredIndex: -1

                    ToolTip {
                        id: tooltip
                        visible: hoverArea.hoveredIndex >= 0 && hoverArea.hoveredIndex < chart.chartData.length
                        text: {
                            if (hoverArea.hoveredIndex < 0 || hoverArea.hoveredIndex >= chart.chartData.length)
                                return ""
                            const item = chart.chartData[hoverArea.hoveredIndex]
                            return `${item.label || ""}: ${item.value || 0}`
                        }
                    }

                    onMouseXChanged: {
                        const padding = chart.chartPadding
                        const chartW = chart.chartWidth
                        const pointSpacing = chartW / (chart.chartData.length - 1 || 1)

                        hoveredIndex = Math.round((mouseX - padding) / pointSpacing)

                        if (hoveredIndex < 0 || hoveredIndex >= chart.chartData.length)
                            hoveredIndex = -1
                    }

                    onMouseYChanged: {
                        const padding = chart.chartPadding
                        const chartH = chart.chartHeight

                        if (hoveredIndex >= 0 && (mouseY < padding || mouseY > padding + chartH))
                            hoveredIndex = -1
                    }

                    onExited: {
                        hoveredIndex = -1
                    }
                }
            }
        }
    }
}
