# 最近打卡记录显示栏 — 替换折线图

## 目标

将仪表盘底部的"近 7 天打卡趋势"折线图替换为"最近打卡记录"列表，按日期分组展示最近 N 条打卡记录。

## 变更范围

### 新建：`ui/components/RecentPunchList.qml`

- `records`: 打卡记录数组
- `maxItems`: 最大条数，默认 100
- ListView + section 按日期分组
- 分组头：今天→"今天"，昨天→"昨天"，其余→yyyy-MM-dd
- 紧凑行：HH:mm:ss | 姓名 | 工号 | 状态标签（颜色圆点）
- 状态颜色源自 Theme.attendancePieColor()

### 修改：`ui/pages/PageDashboard.qml`

- bottomRow 左侧 Card 标题改为"最近打卡记录"
- LineChart 替换为 RecentPunchList
- 新增 `recentPunchRecords` 属性和 `buildRecentPunchRecords()` 方法
- 删除 `weeklyTrendData`、`buildWeeklyTrendData()`、`hasTrendData`
- `trendRow` 可见性只依赖 `page.canReadAttendance`

### 修改：`CMakeLists.txt`

- 移除 `ui/components/LineChart.qml` 注册
- 新增 `ui/components/RecentPunchList.qml` 注册

### 删除：`ui/components/LineChart.qml`
