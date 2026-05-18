pragma Singleton

import QtQuick

QtObject {
    id: root

    function messageForCode(code, serverMsg) {
        const msg = String(serverMsg || "").trim()
        switch (code) {
        case 0: return msg.length ? msg : qsTr("操作成功")
        case 1001: return qsTr("协议解析错误") + (msg ? ("：" + msg) : "")
        case 1002: return qsTr("消息体过大") + (msg ? ("：" + msg) : "")
        case 2001: return qsTr("未认证，请重新登录")
        case 2002: return qsTr("认证失败") + (msg ? ("：" + msg) : "")
        case 2003: return qsTr("会话冲突，请重新登录")
        case 3001: return qsTr("权限不足，请联系管理员") + (msg ? ("：" + msg) : "")
        case 4000: return qsTr("业务校验失败") + (msg ? ("：" + msg) : "")
        case 4001: return qsTr("员工不存在") + (msg ? ("：" + msg) : "")
        case 5001: return qsTr("设备离线") + (msg ? ("：" + msg) : "")
        case 5002: return qsTr("转发超时") + (msg ? ("：" + msg) : "")
        case 6001: return qsTr("数据重复") + (msg ? ("：" + msg) : "")
        case 6002: return qsTr("数据库错误") + (msg ? ("：" + msg) : "")
        case -1: return msg.length ? msg : qsTr("请求超时或网络异常")
        default:
            if (msg.length)
                return qsTr("错误 %1：%2").arg(code).arg(msg)
            return qsTr("错误码 %1").arg(code)
        }
    }

    function connectionStateLabel(state) {
        switch (state) {
        case 0: return qsTr("offline")
        case 1: return qsTr("connecting")
        case 2: return qsTr("connected")
        case 3: return qsTr("authenticated")
        default: return qsTr("unknown")
        }
    }
}
