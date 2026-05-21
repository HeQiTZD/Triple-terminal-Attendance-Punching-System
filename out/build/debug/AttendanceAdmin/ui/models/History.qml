pragma Singleton

import QtQuick

QtObject {
    id: root

    property int maxEntries: 500

    // 每条字段：
    //   time      - 时间字符串
    //   direction - "OUT" / "IN" / "INVOKE"
    //   target    - 接口名 / 设备 ID
    //   payload   - 请求 JSON 文本（或参数摘要）
    //   result    - 结果摘要（"OK"/"FAIL: ..." 等）
    //   ok        - bool
    //   category  - "tcp" / "person" / "device" / "attendance" / "face" / "sync" / "export" / "analytics" / "other"
    property ListModel model: ListModel {}

    signal entryAdded(int index)
    signal replayRequested(var entry)

    function _ts() { return Qt.formatDateTime(new Date(), "HH:mm:ss.zzz") }

    function record(entry) {
        const e = {
            "time": _ts(),
            "direction": String(entry.direction || "INVOKE"),
            "target": String(entry.target || ""),
            "payload": typeof entry.payload === "string" ? entry.payload : JSON.stringify(entry.payload || "", null, 2),
            "result": String(entry.result || ""),
            "ok": !!entry.ok,
            "category": String(entry.category || "other")
        }
        model.append(e)
        if (model.count > maxEntries)
            model.remove(0, model.count - maxEntries)
        entryAdded(model.count - 1)
    }

    function clear() { model.clear() }

    function get(index) {
        if (index < 0 || index >= model.count) return null
        return model.get(index)
    }

    function exportJsonText() {
        let arr = []
        for (let i = 0; i < model.count; ++i) {
            const e = model.get(i)
            arr.push({
                time: e.time,
                direction: e.direction,
                target: e.target,
                payload: e.payload,
                result: e.result,
                ok: e.ok,
                category: e.category
            })
        }
        return JSON.stringify(arr, null, 2)
    }

    function requestReplay(index) {
        const e = get(index)
        if (e) replayRequested(e)
    }
}
