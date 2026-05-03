pragma Singleton

import QtQuick
import QtCore

/*
  全局配置预设单例：通过 QtCore.Settings 持久化。
  - QtObject 无可用 default property 承载裸的 Settings {} 子结点。
  - 使用 property Settings + 内嵌 id（sett），alias 指向 id 而非外层 property 名。
*/
QtObject {
    id: root

    readonly property Settings _backing: Settings {
        id: sett
        category: "AttendanceServer"
        property string dbHost: "localhost"
        property string dbName: "textAttendance"
        property string dbUser: "root"
        property string dbPassword: "root"
        property int tcpPort: 8080
        property string defaultDeviceId: "dev001"
        property string faceAppId: ""
        property string faceSdkKey: ""
        property string defaultEmployeeId: "EMP001"
        property string defaultStartTime: "2026-04-01 00:00:00"
        property string defaultEndTime: "2026-04-30 23:59:59"
        property string lastExportPath: ""
        property string lastImagePath: ""
        property string lastSavePath: ""
    }

    property alias dbHost:     sett.dbHost
    property alias dbName:     sett.dbName
    property alias dbUser:     sett.dbUser
    property alias dbPassword: sett.dbPassword

    property alias tcpPort:         sett.tcpPort
    property alias defaultDeviceId: sett.defaultDeviceId

    property alias faceAppId:  sett.faceAppId
    property alias faceSdkKey: sett.faceSdkKey

    property alias defaultEmployeeId: sett.defaultEmployeeId
    property alias defaultStartTime:  sett.defaultStartTime
    property alias defaultEndTime:    sett.defaultEndTime
    property alias lastExportPath:    sett.lastExportPath
    property alias lastImagePath:     sett.lastImagePath
    property alias lastSavePath:      sett.lastSavePath

    signal changed()

    function reset() {
        sett.dbHost = "localhost"
        sett.dbName = "textAttendance"
        sett.dbUser = "root"
        sett.dbPassword = "root"
        sett.tcpPort = 8080
        sett.defaultDeviceId = "dev001"
        sett.faceAppId = ""
        sett.faceSdkKey = ""
        sett.defaultEmployeeId = "EMP001"
        sett.defaultStartTime = "2026-04-01 00:00:00"
        sett.defaultEndTime = "2026-04-30 23:59:59"
        sett.lastExportPath = ""
        sett.lastImagePath = ""
        sett.lastSavePath = ""
        changed()
    }

    function notifyChanged() { changed() }
}
