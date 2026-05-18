pragma Singleton

import QtQuick
import QtCore

QtObject {
    id: root

    readonly property Settings _backing: Settings {
        id: sett
        category: "AttendanceAdmin"
        property string serverHost: "127.0.0.1"
        property int serverPort: 9527
        property string defaultDeviceId: "DEV001"
        property string defaultEmployeeId: "EMP001"
        property string defaultStartTime: "2026-04-01 00:00:00"
        property string defaultEndTime: "2026-04-30 23:59:59"
    }

    property alias serverHost:         sett.serverHost
    property alias serverPort:         sett.serverPort
    property alias defaultDeviceId:    sett.defaultDeviceId
    property alias defaultEmployeeId:  sett.defaultEmployeeId
    property alias defaultStartTime:   sett.defaultStartTime
    property alias defaultEndTime:     sett.defaultEndTime

    signal changed()

    function reset() {
        sett.serverHost = "127.0.0.1"
        sett.serverPort = 9527
        sett.defaultDeviceId = "DEV001"
        sett.defaultEmployeeId = "EMP001"
        sett.defaultStartTime = "2026-04-01 00:00:00"
        sett.defaultEndTime = "2026-04-30 23:59:59"
        changed()
    }

    function notifyChanged() { changed() }
}
