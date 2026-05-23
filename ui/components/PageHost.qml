import QtQuick
import QtQuick.Controls

import AttendanceAdmin

/*
  按 pageKey 懒加载业务页；无权限时显示 PageAccessPlaceholder。
*/
Item {
    id: host

    property string pageKey: "dashboard"
    required property var sessionManager
    property var personServer
    property var deviceServer
    property var configDeployServer
    property var attendanceService
    property var faceServer
    property var userServer
    property var rbacServer
    property var eventService
    property var deniedDialog: null

    signal serviceResult(string apiType, int code, string msg, string category)

    readonly property bool pageAllowed: {
        if (sessionManager) {
            const _p = sessionManager.permissions
            const _r = sessionManager.roles
            void _p
            void _r
        }
        return PermissionCatalog.canAccessNav(host.pageKey, sessionManager)
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        active: host.pageAllowed
        asynchronous: true
        sourceComponent: {
            switch (host.pageKey) {
            case "dashboard":  return compDashboard
            case "person":     return compPerson
            case "device":     return compDevice
            case "configDeploy": return compConfigDeploy
            case "attendance": return compAttendance
            case "face":       return compFace
            case "user":       return compUser
            case "rbac":       return compRbac
            default:           return null
            }
        }
    }

    PageAccessPlaceholder {
        anchors.fill: parent
        visible: !host.pageAllowed
        pageKey: host.pageKey
        sessionManager: host.sessionManager
    }

    Component {
        id: compDashboard
        PageDashboard {
            sessionManager: host.sessionManager
            personServer: host.personServer
            deviceServer: host.deviceServer
            attendanceService: host.attendanceService
        }
    }

    Component {
        id: compPerson
        PagePerson {
            personServer: host.personServer
            sessionManager: host.sessionManager
            deniedDialog: host.deniedDialog
            onServiceResult: (apiType, code, msg) =>
                host.serviceResult(apiType, code, msg, "person")
        }
    }

    Component {
        id: compDevice
        PageDevice {
            deviceServer: host.deviceServer
            sessionManager: host.sessionManager
            deniedDialog: host.deniedDialog
            onServiceResult: (apiType, code, msg) =>
                host.serviceResult(apiType, code, msg, "device")
        }
    }

    Component {
        id: compConfigDeploy
        PageConfigDeploy {
            configDeployServer: host.configDeployServer
            deviceServer: host.deviceServer
            sessionManager: host.sessionManager
            deniedDialog: host.deniedDialog
            onServiceResult: (apiType, code, msg) =>
                host.serviceResult(apiType, code, msg, "config")
        }
    }

    Component {
        id: compAttendance
        PageAttendance {
            attendanceService: host.attendanceService
            sessionManager: host.sessionManager
            deniedDialog: host.deniedDialog
            onServiceResult: (apiType, code, msg) =>
                host.serviceResult(apiType, code, msg, "attendance")
        }
    }

    Component {
        id: compFace
        PageFace {
            faceServer: host.faceServer
            sessionManager: host.sessionManager
            deniedDialog: host.deniedDialog
            onServiceResult: (apiType, code, msg) =>
                host.serviceResult(apiType, code, msg, "face")
        }
    }

    Component {
        id: compUser
        PageUser {
            userServer: host.userServer
            sessionManager: host.sessionManager
            deniedDialog: host.deniedDialog
            onServiceResult: (apiType, code, msg) =>
                host.serviceResult(apiType, code, msg, "user")
        }
    }

    Component {
        id: compRbac
        PageRbac {
            rbacServer: host.rbacServer
            userServer: host.userServer
            sessionManager: host.sessionManager
            deniedDialog: host.deniedDialog
            onServiceResult: (apiType, code, msg) =>
                host.serviceResult(apiType, code, msg, "rbac")
        }
    }

}
