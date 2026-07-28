pragma Singleton

import QtQuick

QtObject {
    id: root

    // ---- 依赖 ----
    property var fileService: null   // 由 init(service) 注入 FileService 实例

    // ---- 数据 ----
    property ListModel entries: ListModel {}          // 全量解析后的条目
    property ListModel filteredEntries: ListModel {}  // 过滤后供 UI 显示

    // ---- 日期列表（供 ComboBox 使用）----
    property var availableDates: []   // string[]  如 ["2026-05-26", "2026-05-25"]
    property string currentDate: ""  // 当前选中日期

    // ---- 过滤条件 ----
    property string filterLevel: ""      // "INFO" / "ERROR" / "WARN" / "DEBUG" / ""
    property string filterType: ""       // "tcp" / "person" / ""  等
    property string filterKeyword: ""    // 关键词（模糊匹配 message）
    property string filterDateFrom: ""   // 起始日期 yyyy-MM-dd（含）
    property string filterDateTo: ""     // 结束日期 yyyy-MM-dd（含）

    // ---- 状态 ----
    property bool loading: false
    property int totalCount: 0
    property int filteredCount: 0
    property string lastError: ""

    // ---- 信号 ----
    signal logsLoaded()
    signal filterChanged()

    // ================================================================
    //  公共 API
    // ================================================================

    /// 初始化，注入 FileService 实例
    function init(service) {
        fileService = service
        refreshDateList()
    }

    /// 刷新可用日期列表
    function refreshDateList() {
        if (!fileService) return
        var dates = fileService.getLogFileDates()
        availableDates = dates
        if (dates.length > 0 && !currentDate) {
            currentDate = dates[0]
        }
    }

    /// 加载指定日期的日志；不传参则使用 currentDate
    function loadLogs(date) {
        if (!fileService) {
            lastError = "FileService 未初始化"
            return
        }
        var d = date || currentDate
        if (!d) {
            lastError = "未选择日期"
            return
        }
        currentDate = d
        loading = true
        lastError = ""

        var content = fileService.readLogFile(d)
        if (!content) {
            entries.clear()
            filteredEntries.clear()
            totalCount = 0
            filteredCount = 0
            loading = false
            lastError = "无法读取日志文件：" + d
            return
        }

        _parseContent(content)
        loading = false
        applyFilters()
        logsLoaded()
    }

    /// 解析日志行：[timestamp] [level] [type] message [| key=value ...]
    function parseLogLine(line) {
        if (!line || line.length === 0) return null

        // 去掉末尾空白
        var s = line.replace(/\s+$/, "")

        // 匹配 [timestamp] [level] [type] message
        var re = /^\[([^\]]+)\]\s*\[([^\]]+)\]\s*\[([^\]]+)\]\s*(.*)$/
        var m = re.exec(s)
        if (!m) return null

        var timestamp = m[1]
        var level     = m[2]
        var type      = m[3]
        var rest      = m[4]

        // 分离 message 和 details
        var message = rest
        var details = ""
        var pipeIdx = rest.indexOf(" | ")
        if (pipeIdx >= 0) {
            message = rest.substring(0, pipeIdx)
            details = rest.substring(pipeIdx + 3)
        }

        return {
            "timestamp": timestamp,
            "level":     level,
            "type":      type,
            "message":   message,
            "details":   details,
            "raw":       s
        }
    }

    /// 应用当前过滤条件，结果写入 filteredEntries
    function applyFilters() {
        filteredEntries.clear()
        for (var i = 0; i < entries.count; ++i) {
            var e = entries.get(i)
            if (_matchFilter(e)) {
                filteredEntries.append(e)
            }
        }
        filteredCount = filteredEntries.count
        filterChanged()
    }

    // ---- 过滤条件 setter ----

    function setFilterLevel(level) {
        if (filterLevel !== level) {
            filterLevel = level
            applyFilters()
        }
    }

    function setFilterType(type) {
        if (filterType !== type) {
            filterType = type
            applyFilters()
        }
    }

    function setFilterKeyword(keyword) {
        if (filterKeyword !== keyword) {
            filterKeyword = keyword
            applyFilters()
        }
    }

    function setFilterDateRange(from, to) {
        filterDateFrom = from || ""
        filterDateTo   = to   || ""
        applyFilters()
    }

    /// 重置所有过滤条件
    function clearFilters() {
        filterLevel    = ""
        filterType     = ""
        filterKeyword  = ""
        filterDateFrom = ""
        filterDateTo   = ""
        applyFilters()
    }

    /// 导出当前过滤后的日志到文件
    function exportLogs(filePath) {
        if (!fileService) {
            lastError = "FileService 未初始化"
            return false
        }
        var lines = []
        for (var i = 0; i < filteredEntries.count; ++i) {
            var e = filteredEntries.get(i)
            lines.push(e.raw)
        }
        var content = lines.join("\n")
        if (content.length > 0) content += "\n"
        return fileService.exportLogs(content, filePath)
    }

    // ================================================================
    //  内部辅助
    // ================================================================

    /// 批量解析原始文本为 entries ListModel
    function _parseContent(content) {
        entries.clear()
        var lines = content.split("\n")
        for (var i = 0; i < lines.length; ++i) {
            var obj = parseLogLine(lines[i])
            if (obj) {
                entries.append(obj)
            }
        }
        totalCount = entries.count
    }

    /// 判断单条是否匹配当前过滤条件
    function _matchFilter(e) {
        if (filterLevel.length > 0 && e.level !== filterLevel)
            return false
        if (filterType.length > 0 && e.type !== filterType)
            return false
        if (filterKeyword.length > 0 && e.message.indexOf(filterKeyword) < 0
            && e.details.indexOf(filterKeyword) < 0)
            return false
        // 日期范围过滤：从 timestamp ("yyyy-MM-dd HH:mm:ss") 中提取日期
        if (filterDateFrom.length > 0 || filterDateTo.length > 0) {
            var datePart = e.timestamp.substring(0, 10)
            if (filterDateFrom.length > 0 && datePart < filterDateFrom)
                return false
            if (filterDateTo.length > 0 && datePart > filterDateTo)
                return false
        }
        return true
    }
}
