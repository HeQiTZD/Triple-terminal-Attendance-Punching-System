# UI Design Document Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement all P1–P3 changes from docs/UI_INTERFACE_DESIGN.md §12: fix color contrast, replace emoji icons, add font fallback, unify charts, fix Card padding, add shadow tokens, add LineChart, and reserve dark mode tokens.

**Architecture:** Theme.qml is the single source of truth for all design tokens. Other files consume Theme properties only — no hardcoded colors/values. Changes are split into 8 independent task groups: 3 are single-token Theme.qml edits, 1 is icon replacement, 1 is Card padding fix, 1 is new LineChart component, 1 is CMakeLists registration, and 1 is dark mode token preparation.

**Tech Stack:** Qt 6.8 QML, Qt Quick Controls (Fusion style), QtCharts, Canvas 2D

---

### Task 1: Fix `textMuted` contrast and `hover` color in Theme.qml

**Files:**
- Modify: `ui/theme/Theme.qml:23,42`

- [ ] **Step 1: Update textMuted for WCAG AA 4.5:1**

```qml
// Line 23 — change:
readonly property color textMuted:    "#4B5563"
// to:
readonly property color textMuted:    "#3D4551"
```

- [ ] **Step 2: Update hover for distinction from surfaceAlt**

```qml
// Line 42 — change:
readonly property color hover:        "#DCE3EE"
// to:
readonly property color hover:        "#CBD5E1"
```

- [ ] **Step 3: Commit**

```bash
git add ui/theme/Theme.qml
git commit -m "fix(theme): increase textMuted contrast to 4.5:1, deepen hover for row visibility"
```

---

### Task 2: Fix `warning` color contrast and add font fallback chain

**Files:**
- Modify: `ui/theme/Theme.qml:32,72`

- [ ] **Step 1: Fix warning color for WCAG AA on white**

```qml
// Line 32 — change:
readonly property color warning:      "#B45309"
// to:
readonly property color warning:      "#9A3E04"
```

- [ ] **Step 2: Add cross-platform font fallback**

```qml
// Line 72 — change:
readonly property string fontFamily: "Microsoft YaHei UI"
// to:
readonly property string fontFamily: "'Segoe UI', 'Microsoft YaHei UI', 'PingFang SC', sans-serif"
```

- [ ] **Step 3: Commit**

```bash
git add ui/theme/Theme.qml
git commit -m "fix(theme): fix warning color contrast, add cross-platform font fallback"
```

---

### Task 3: Prepare dark mode color tokens in Theme.qml

**Files:**
- Modify: `ui/theme/Theme.qml` (append dark palette section)

- [ ] **Step 1: Add dark mode color properties at end of QtObject**

Add after `chartColor()` function, before the closing `}`:

```qml
    // ===== 暗色模式色板（远期启用） =====
    readonly property color darkBg:           "#0F172A"
    readonly property color darkSurface:      "#1E293B"
    readonly property color darkSurfaceAlt:   "#273449"
    readonly property color darkSideBar:      "#162032"
    readonly property color darkBorder:       "#334155"
    readonly property color darkBorderStrong: "#475569"

    readonly property color darkText:         "#F1F5F9"
    readonly property color darkTextMuted:    "#94A3B8"
    readonly property color darkTextSubtle:   "#64748B"

    readonly property color darkHover:        "#334155"
    readonly property color darkSelected:     "#1E3A5F"

    readonly property color darkShadowLight:  Qt.rgba(0, 0, 0, 0.30)
    readonly property color darkShadowMedium: Qt.rgba(0, 0, 0, 0.45)
    readonly property color darkShadowStrong: Qt.rgba(0, 0, 0, 0.60)
```

- [ ] **Step 2: Add dark chart colors array**

```qml
    readonly property var darkChartColors: [
        "#3B82F6",
        "#22C55E",
        "#F97316",
        "#A78BFA",
        "#EF4444",
        "#06B6D4",
        "#84CC16",
        "#F59E0B"
    ]
```

- [ ] **Step 3: Commit**

```bash
git add ui/theme/Theme.qml
git commit -m "feat(theme): add dark mode color tokens for future dark theme support"
```

---

### Task 4: Replace emoji navigation icons with Unicode symbols

**Files:**
- Modify: `ui/models/PermissionCatalog.qml:127-141`

- [ ] **Step 1: Replace emoji icons in allNavItems**

```qml
// Lines 127-141 — replace all emoji with cross-platform Unicode symbols:
readonly property var allNavItems: [
    { group: qsTr("总览") },
    { key: "dashboard", label: qsTr("仪表盘"), icon: "◆" },      // ◆ black diamond

    { group: qsTr("数据管理") },
    { key: "person", label: qsTr("人员管理"), icon: "☺" },        // ☺ smile (face substitute)
    { key: "device", label: qsTr("设备管理"), icon: "⬡" },        // ⬡ hexagon
    { key: "configDeploy", label: qsTr("配置下发"), icon: "⇧" },  // ⇧ upload arrow
    { key: "attendance", label: qsTr("考勤记录"), icon: "☰" },    // ☰ list/hamburger
    { key: "face", label: qsTr("人脸库"), icon: "◎" },            // ◎ double circle

    { group: qsTr("账号与权限") },
    { key: "user", label: qsTr("用户账号"), icon: "⚿" },         // ⚿ key
    { key: "rbac", label: qsTr("用户权限"), icon: "⛨" }          // ⛨ shield
]
```

- [ ] **Step 2: Commit**

```bash
git add ui/models/PermissionCatalog.qml
git commit -m "fix(ui): replace emoji nav icons with cross-platform Unicode symbols"
```

---

### Task 5: Fix Card double-padding and implement shadow tokens

**Files:**
- Modify: `ui/components/Card.qml:14`
- Modify: `ui/pages/PageDashboard.qml:312-313` (RecentPunchList anchors)

- [ ] **Step 1: Add shadow support to Card.qml**

Add a new property and layer effect to Card.qml. Add after `property bool stretchContent: false` on line 25:

```qml
    property int elevation: 0  // 0=none, 1=light, 2=medium, 3=strong

    layer.enabled: root.elevation > 0
    layer.effect: null  // Qt Quick has no built-in shadow; use DropShadow for Qt 6.7+
```

For Qt 6.7+ (if available), replace the `layer.effect: null` with a proper implementation. Since the project uses Qt 6.8, we can use `MultiEffect`:

Add import at top of Card.qml:
```qml
import QtQuick.Effects
```

Then replace the `layer` block with:
```qml
    layer.enabled: root.elevation > 0
    layer.effect: MultiEffect {
        shadowEnabled: root.elevation > 0
        shadowBlur: root.elevation <= 1 ? 0.2 :
                    root.elevation === 2 ? 0.4 : 0.6
        shadowVerticalOffset: root.elevation <= 1 ? 1 :
                              root.elevation === 2 ? 3 : 6
        shadowColor: Theme.shadowLight
    }
```

If Qt Quick Effects module is not available, skip the layer.effect and just document that shadow is controlled via `elevation` property for future use.

- [ ] **Step 2: Fix RecentPunchList double-padding in PageDashboard.qml**

```qml
// Lines 312-314 — remove extra margins since Card already applies padding:
RecentPunchList {
    anchors.fill: parent
    // Remove: anchors.margins: Theme.spacingMd
}
```

- [ ] **Step 3: Check all other pages for double-padding pattern**

Grep for `anchors.margins:` inside `stretchContent: true` Card blocks. If found, remove the extra margins.

For pages that use Card with `stretchContent: true` and children adding their own margins:
- `PageAttendance.qml` — check DataTable inside stretchContent Card
- `PagePerson.qml` — check DataTable inside stretchContent Card  
- `PageDevice.qml` — check DataTable inside stretchContent Card

These are correct because Card.padding already provides the spacing.

- [ ] **Step 4: Commit**

```bash
git add ui/components/Card.qml ui/pages/PageDashboard.qml
git commit -m "fix(ui): add Card elevation property, remove double-padding on RecentPunchList"
```

---

### Task 6: Create LineChart.qml component

**Files:**
- Create: `ui/components/LineChart.qml`

- [ ] **Step 1: Create LineChart.qml with Canvas 2D rendering**

Write `ui/components/LineChart.qml`:

```qml
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

                        // Area fill (if enabled)
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
```

- [ ] **Step 2: Commit**

```bash
git add ui/components/LineChart.qml
git commit -m "feat(ui): add LineChart component with Canvas 2D rendering"
```

---

### Task 7: Register LineChart.qml in CMakeLists.txt

**Files:**
- Modify: `CMakeLists.txt` (add LineChart.qml to QML_FILES)

- [ ] **Step 1: Add LineChart.qml to QML_FILES list**

Insert after the BarChart.qml line in CMakeLists.txt:
```
        ui/components/BarChart.qml
        ui/components/LineChart.qml
        ui/components/ChartLegend.qml
```

- [ ] **Step 2: Commit**

```bash
git add CMakeLists.txt
git commit -m "build: register LineChart.qml in CMakeLists"
```

---

### Task 8: Build verification

**Files:**
- None (read-only verification)

- [ ] **Step 1: Clean build test**

```bash
cd E:/project/AttendanceServer && cmake --build out/build/debug --target appAttendanceAdmin 2>&1 | tail -30
```

Expected: Build succeeds with zero errors. If `QtQuick.Effects` import fails in Card.qml, remove the `import QtQuick.Effects` line and the `layer.effect: MultiEffect { ... }` block, leaving only `property int elevation: 0` and `layer.enabled: root.elevation > 0`.

- [ ] **Step 2: Verify QML module loads without warnings**

Launch the app and check stderr for QML warnings. There should be zero new warnings related to the changed files.

---

## Self-Review

**1. Spec coverage — docs/UI_INTERFACE_DESIGN.md §12 checklist:**

| # | Change | Task | Status |
|---|--------|------|--------|
| P1 | textMuted contrast `#4B5563` → `#3D4551` | Task 1 | ✅ |
| P1 | Font fallback chain | Task 2 | ✅ |
| P1 | Emoji → Unicode icons | Task 4 | ✅ |
| P2 | hover color `#DCE3EE` → `#CBD5E1` | Task 1 | ✅ |
| P2 | Warning color fix | Task 2 | ✅ |
| P2 | Add LineChart component | Task 6+7 | ✅ |
| P2 | Fix Card double-padding | Task 5 | ✅ |
| P3 | Shadow tokens implementation | Task 5 | ✅ |
| P3 | Dark mode tokens | Task 3 | ✅ |

**2. Placeholder scan:** No TBD, TODO, or placeholders found.

**3. Type consistency:** Theme.qml uses `readonly property color` / `readonly property string` consistently. LineChart property names match BarChart conventions (`chartData`, `showGrid`, `animationDuration`). Card.qml `elevation` is `int` matching the shadow level concept.

---

## Execution Handoff

**Plan complete and saved to `docs/superpowers/plans/2026-05-23-ui-design-implementation.md`. Two execution options:**

**1. Subagent-Driven (recommended)** - I dispatch a fresh subagent per task, review between tasks, fast iteration

**2. Inline Execution** - Execute tasks in this session using executing-plans, batch execution with checkpoints

**Which approach?**
