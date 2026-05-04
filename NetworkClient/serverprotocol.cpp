#include "serverprotocol.h"

namespace ServerProtocol {

static QString typeString(const QJsonObject& msg) {
    return msg.value(kType).toString();
}

MessageType parseType(const QJsonObject& msg) {
    const QString t = typeString(msg);
    if (t == tAuthResponse) return MessageType::AuthResponse;
    if (t == tHeartbeatResponse) return MessageType::HeartbeatResponse;
    if (t == tPersonSync) return MessageType::PersonSync;
    if (t == tError) return MessageType::Error;
    return MessageType::Unknown;
}

QJsonObject buildAuth(const QString& deviceId) {
    QJsonObject msg;
    msg[kType] = tAuth;
    msg[kDeviceId] = deviceId;
    return msg;
}

QJsonObject buildHeartbeat() {
    QJsonObject msg;
    msg[kType] = tHeartbeat;
    return msg;
}

QJsonObject buildSyncRequest(const QString& deviceId) {
    QJsonObject msg;
    msg[kType] = tSyncRequest;
    msg[kDeviceId] = deviceId;
    return msg;
}

QJsonObject buildAttendanceRecord(const QString& employeeId,
                                  const QDateTime& checkTime,
                                  const QString& deviceId,
                                  const QString& status) {
    QJsonObject msg;
    msg[kType] = tAttendanceRecord;
    msg[kEmployeeId] = employeeId;
    msg[kCheckTime] = checkTime.toString(Qt::ISODate);
    msg[kDeviceId] = deviceId;
    msg[kStatus] = status;
    return msg;
}

QJsonObject buildDeviceStatus(const QString& deviceId,
                              const QString& status,
                              const QJsonObject& statusPayload) {
    QJsonObject msg = statusPayload;
    msg[kType] = tDeviceStatus;
    msg[kDeviceId] = deviceId;
    msg[kStatus] = status;
    return msg;
}

QVector<PersonSyncItem> parsePersons(const QJsonObject& msg) {
    QVector<PersonSyncItem> out;
    const auto personsVal = msg.value(kPersons);
    if (!personsVal.isArray()) return out;

    const QJsonArray arr = personsVal.toArray();
    out.reserve(arr.size());
    for (const auto& v : arr) {
        if (!v.isObject()) continue;
        const QJsonObject o = v.toObject();

        PersonSyncItem p;
        p.id = o.value("id").toInt();
        p.name = o.value("name").toString();
        p.employeeId = o.value("employeeId").toString();
        p.department = o.value("department").toString();
        p.position = o.value("position").toString();
        out.push_back(p);
    }
    return out;
}

} // namespace ServerProtocol

