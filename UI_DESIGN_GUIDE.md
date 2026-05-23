# 企业考勤管理系统 - UI 设计系统使用指南

## 📋 项目概述

本设计系统为考勤记录管理应用提供了一整套专业的企业级 UI 组件和设计规范。

## ✅ 已完成的工作

### 1. 设计系统规范
- 📄 **DESIGN_SYSTEM.md** - 完整的设计系统文档
- 包含色彩系统、字体系统、间距系统、圆角系统等规范

### 2. 主题系统优化
- ✨ **Theme.qml** - 企业蓝色系配色方案
  - 主色: `#1D6BFF` (企业蓝)
  - 状态色: 成功/警告/错误/信息
  - 图表色: 8 种专业配色
  - 交互状态色: 悬停/选中/高亮

### 3. 图表组件库

#### 📈 趋势图表
1. **LineChart.qml** - 折线图组件
   - 支持平滑曲线和面积填充
   - 数据点标记和悬停提示
   - 网格线和轴标签
   - 动画效果

2. **BarChart.qml** - 柱状图组件
   - 支持垂直和水平模式
   - 数值标签显示
   - 颜色自动分配
   - 交互式悬停效果

3. **AttendanceStatusPieChart.qml** - 饼图组件（已优化）
   - 支持扇形突出效果
   - 中心统计信息
   - 图例显示
   - 百分比标签

#### 🎨 辅助组件
4. **ChartLegend.qml** - 图表图例组件
   - 自动颜色匹配
   - 水平/垂直布局
   - 悬停高亮效果

5. **ChartContainer.qml** - 图表容器组件
   - 统一的卡片包装
   - 可选标题和图例
   - 响应式布局

6. **StatCard.qml** - 统计卡片组件
   - 大数字显示
   - 趋势指示器（上升/下降）
   - 图标支持
   - 悬停交互效果

## 🚀 快速开始

### 1. 在项目中使用组件

#### 使用主题颜色
```qml
import AttendanceAdmin

Rectangle {
    color: Theme.primary
    border.color: Theme.border
}

Label {
    text: "标题"
    color: Theme.text
    font.pixelSize: Theme.fontLg
}
```

#### 使用折线图
```qml
LineChart {
    anchors.fill: parent
    data: [
        { label: "周一", value: 120 },
        { label: "周二", value: 150 },
        { label: "周三", value: 180 }
    ]
    showGrid: true
    showPoints: true
    showArea: true
}
```

#### 使用柱状图
```qml
BarChart {
    anchors.fill: parent
    data: [
        { label: "正常", value: 85 },
        { label: "迟到", value: 10 },
        { label: "缺勤", value: 5 }
    ]
    showValues: true
    horizontal: false
}
```

#### 使用饼图
```qml
AttendanceStatusPieChart {
    anchors.fill: parent
    slices: [
        { label: "正常", value: 85, color: Theme.pieNormal },
        { label: "迟到", value: 10, color: Theme.pieLate }
    ]
}
```

#### 使用统计卡片
```qml
StatCard {
    title: "今日打卡"
    value: "256"
    subtitle: "较昨日 +12%"
    trend: "12%"
    trendPositive: true
    icon: "✓"
    iconColor: Theme.success
}
```

#### 使用图表容器
```qml
ChartContainer {
    title: "考勤趋势"
    showHeader: true
    showLegend: true

    chart: LineChart {
        data: weeklyData
    }

    legendItems: [
        { label: "打卡次数", value: 256 }
    ]
}
```

## 📊 组件属性说明

### LineChart（折线图）
| 属性 | 类型 | 说明 |
|------|------|------|
| `data` | `var` | 数据数组 `[{label, value}]` |
| `title` | `string` | 图表标题 |
| `xAxisLabel` | `string` | X 轴标签 |
| `yAxisLabel` | `string` | Y 轴标签 |
| `showGrid` | `bool` | 显示网格线 |
| `showPoints` | `bool` | 显示数据点 |
| `showArea` | `bool` | 显示面积填充 |
| `animationDuration` | `int` | 动画时长(ms) |

### BarChart（柱状图）
| 属性 | 类型 | 说明 |
|------|------|------|
| `data` | `var` | 数据数组 `[{label, value, color?}]` |
| `title` | `string` | 图表标题 |
| `horizontal` | `bool` | 水平模式 |
| `showGrid` | `bool` | 显示网格线 |
| `showValues` | `bool` | 显示数值标签 |
| `animationDuration` | `int` | 动画时长(ms) |

### AttendanceStatusPieChart（饼图）
| 属性 | 类型 | 说明 |
|------|------|------|
| `slices` | `var` | 数据数组 `[{label, value, color}]` |
| `featuredIndex` | `int` | 当前高亮的扇区 |
| `totalCount` | `int` | 总数（只读） |

### StatCard（统计卡片）
| 属性 | 类型 | 说明 |
|------|------|------|
| `title` | `string` | 卡片标题 |
| `value` | `string` | 主数值 |
| `subtitle` | `string` | 副标题 |
| `trend` | `string` | 趋势值 |
| `trendPositive` | `bool` | 趋势方向 |
| `icon` | `string` | 图标文字 |
| `iconColor` | `color` | 图标颜色 |

## 🎨 设计原则

### 色彩使用
```qml
// 主色调
color: Theme.primary        // 企业蓝 #1D6BFF
color: Theme.primaryHover   // 悬停蓝 #3B82FF

// 状态色
color: Theme.success        // 绿色 - 正常/成功
color: Theme.warning        // 橙色 - 迟到/警告
color: Theme.danger         // 红色 - 缺勤/错误
color: Theme.info           // 蓝色 - 补签/信息

// 图表色
color: Theme.chartColor(0)  // 自动获取图表色
color: Theme.pieNormal      // 考勤正常色
color: Theme.pieLate        // 考勤迟到色
```

### 间距系统
```qml
// 使用标准间距
spacing: Theme.spacingXs    // 4px - 紧凑
spacing: Theme.spacingSm    // 6px - 小
spacing: Theme.spacingMd    // 10px - 中
spacing: Theme.spacingLg    // 16px - 大
spacing: Theme.spacingXl    // 24px - 超大
```

### 圆角系统
```qml
// 根据元素类型选择圆角
radius: Theme.radiusSm      // 4px - 按钮/输入框
radius: Theme.radiusMd      // 6px - 卡片
radius: Theme.radiusLg      // 10px - 模态框
```

## 📱 响应式设计

组件支持自适应布局：
- 自动填充可用空间
- 最小/最大尺寸限制
- 灵活的布局比例

## ♿ 可访问性

- 足够的颜色对比度（≥4.5:1）
- 键盘导航支持
- 屏幕阅读器友好
- 焦点状态清晰可见

## 🔧 自定义指南

### 创建自定义图表
```qml
// 在 LineChart 基础上添加自定义功能
LineChart {
    id: customChart

    // 覆盖默认样式
    property color lineColor: Theme.primary
    property int lineWidth: 3

    // 添加交互
    MouseArea {
        anchors.fill: parent
        onClicked: console.log("Chart clicked")
    }
}
```

### 创建自定义统计卡片
```qml
StatCard {
    id: customCard

    // 添加额外内容
    footer: Rectangle {
        color: Theme.surfaceAlt
        height: 30

        Label {
            anchors.centerIn: parent
            text: "更新时间: " + lastUpdate
        }
    }

    onClicked: {
        // 自定义点击行为
        openDetailDialog()
    }
}
```

## 📈 数据可视化最佳实践

### 1. 选择合适的图表类型
- **折线图**: 展示趋势变化（近 7 天打卡趋势）
- **柱状图**: 对比分析（部门打卡分布）
- **饼图**: 展示占比（考勤状态分布）

### 2. 数据格式化
```qml
// 数值格式化
function formatNumber(num) {
    if (num >= 1000) {
        return (num / 1000).toFixed(1) + 'k'
    }
    return num.toString()
}

// 百分比格式化
function formatPercent(value, total) {
    if (total === 0) return "0%"
    return ((value / total) * 100).toFixed(1) + "%"
}
```

### 3. 颜色编码
```qml
// 根据状态自动选择颜色
function getStatusColor(status) {
    switch(status) {
        case "normal": return Theme.success
        case "late": return Theme.warning
        case "absent": return Theme.danger
        default: return Theme.textMuted
    }
}
```

## 📝 注意事项

1. **性能优化**
   - 大量数据时使用虚拟化列表
   - 图表懒加载
   - 数据缓存

2. **兼容性**
   - 确保 Qt 版本支持 Canvas API
   - 测试不同 DPI 屏幕
   - 验证深色模式（如未来支持）

3. **维护性**
   - 使用 Theme 常量而非硬编码颜色
   - 组件解耦，便于单独更新
   - 文档化自定义组件

## 🎯 下一步建议

1. **添加更多图表类型**
   - 堆叠柱状图（展示多维度数据）
   - 漏斗图（展示流程转化）
   - 热力图（日历视图）

2. **增强交互**
   - 图表缩放和拖拽
   - 数据筛选联动
   - 导出功能（PNG/CSV）

3. **优化性能**
   - 图表组件性能测试
   - 大数据量优化
   - 动画性能调优

4. **完善文档**
   - 组件 API 文档
   - 示例代码库
   - 设计资源下载

## 📞 技术支持

如有问题或建议，请参考：
- DESIGN_SYSTEM.md - 设计规范
- 各组件的 QML 源码注释
- Qt Quick Controls 官方文档

---

**版本**: 1.0.0
**更新日期**: 2026-05-22
**设计风格**: 企业蓝色系 · 专业商务
