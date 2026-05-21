pragma Singleton

import QtQuick

QtObject {
    id: root

    // 日志条目最大保留数（避免内存膨胀）
    property int maxEntries: 2000

    property ListModel model: ListModel {}

    // 最近一条文本，便于在状态栏展示
    property string lastMessage: ""
    property string lastError: ""
    property string lastInfo: ""

    signal logged(int level, string message)

    // 级别枚举：保持与 ListModel 中字段含义一致
    readonly property int levelDebug: 0
    readonly property int levelInfo:  1
    readonly property int levelWarn:  2
    readonly property int levelError: 3
    readonly property int levelOk:    4

    function _ts() {
        return Qt.formatDateTime(new Date(), "HH:mm:ss")
    }

    function _push(level, message) {
        if (!message)
            return
        const ts = _ts()
        model.append({
            "time": ts,
            "level": level,
            "message": String(message)
        })
        if (model.count > maxEntries)
            model.remove(0, model.count - maxEntries)

        lastMessage = message
        if (level === levelError)
            lastError = message
        else if (level === levelInfo || level === levelOk)
            lastInfo = message

        console.log("[" + ts + "][" + _levelTag(level) + "] " + message)
        logged(level, message)
    }

    function _levelTag(level) {
        switch (level) {
        case levelDebug: return "DBG"
        case levelInfo:  return "INFO"
        case levelWarn:  return "WARN"
        case levelError: return "ERR"
        case levelOk:    return "OK"
        }
        return "?"
    }

    function debug(msg) { _push(levelDebug, msg) }
    function info(msg)  { _push(levelInfo,  msg) }
    function warn(msg)  { _push(levelWarn,  msg) }
    function error(msg) { _push(levelError, msg) }
    function ok(msg)    { _push(levelOk,    msg) }

    // 通用：根据布尔结果记录 PASS/FAIL
    function logResult(name, success, detail) {
        const tail = detail ? (" | " + detail) : ""
        if (success)
            ok("操作=" + name + " | 结果=成功" + tail)
        else
            error("操作=" + name + " | 结果=失败" + tail)
    }

    function clear() {
        model.clear()
        lastMessage = ""
        lastError = ""
        lastInfo = ""
    }

    // 导出为可读文本
    function toText(filterLevel) {
        let lines = []
        for (let i = 0; i < model.count; ++i) {
            const e = model.get(i)
            if (filterLevel !== undefined && filterLevel !== null && e.level !== filterLevel)
                continue
            lines.push("[" + e.time + "][" + _levelTag(e.level) + "] " + e.message)
        }
        return lines.join("\n")
    }
}
