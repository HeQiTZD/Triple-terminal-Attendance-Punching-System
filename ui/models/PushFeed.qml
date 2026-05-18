pragma Singleton

import QtQuick

QtObject {
    id: root

    property ListModel model: ListModel {}
    property int todayCount: 0
    property string todayKey: ""

    signal pushRecorded()

    function _today() {
        return Qt.formatDateTime(new Date(), "yyyy-MM-dd")
    }

    function _ensureDay() {
        const d = _today()
        if (todayKey !== d) {
            todayKey = d
            todayCount = 0
        }
    }

    function recordPush(messageType, summary) {
        _ensureDay()
        todayCount++
        model.insert(0, {
            time: Qt.formatDateTime(new Date(), "HH:mm:ss"),
            type: String(messageType || ""),
            summary: String(summary || "")
        })
        if (model.count > 100)
            model.remove(100, model.count - 100)
        pushRecorded()
    }

    function clear() {
        model.clear()
        todayCount = 0
    }
}
