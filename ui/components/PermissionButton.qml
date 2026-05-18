import QtQuick
import QtQuick.Controls

import AttendanceAdmin

Button {
    id: control

    property var sessionManager
    property string requiredPermission: ""
    property string requiredRole: ""
    property bool hideWhenDenied: true
    property var deniedDialog: null

    readonly property bool allowed: PermissionCatalog.isAllowed(
        sessionManager, requiredPermission, requiredRole)

    visible: hideWhenDenied ? allowed : true
    enabled: allowed

    function guardedClick(handler) {
        if (!allowed) {
            const hint = requiredRole.length ? ("role:" + requiredRole) : requiredPermission
            if (deniedDialog)
                deniedDialog.openFor(hint)
            else
                Logger.warn(qsTr("权限不足：") + hint)
            return
        }
        if (handler)
            handler()
    }
}
