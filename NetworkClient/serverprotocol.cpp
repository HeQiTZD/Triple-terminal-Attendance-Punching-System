#include "serverprotocol.h"

#include <QUuid>

namespace ServerProtocol {

// ============================================================
// Envelope
// ============================================================

static const QStringList s_envelopeKeys = {
    QStringLiteral("type"),
    QStringLiteral("role"),
    QStringLiteral("from"),
    QStringLiteral("to"),
    QStringLiteral("msgId"),
    QStringLiteral("inReplyTo"),
    QStringLiteral("ts"),
    QStringLiteral("ack"),
    QStringLiteral("code"),
    QStringLiteral("msg"),
    QStringLiteral("data"),
};

QJsonObject Envelope::toJson() const
{
    QJsonObject obj;

    // Merge data fields first (payload at root)
    for (auto it = data.begin(); it != data.end(); ++it)
        obj[it.key()] = it.value();

    // Envelope keys overwrite any data field with same key
    obj[QStringLiteral("type")]  = type;
    obj[QStringLiteral("role")]  = role;
    obj[QStringLiteral("from")]  = from;
    obj[QStringLiteral("to")]    = to;
    obj[QStringLiteral("msgId")] = msgId;
    if (!inReplyTo.isEmpty())
        obj[QStringLiteral("inReplyTo")] = inReplyTo;
    obj[QStringLiteral("ts")] = ts;
    if (ack)
        obj[QStringLiteral("ack")] = true;
    if (code != 0)
        obj[QStringLiteral("code")] = code;
    if (!msg.isEmpty())
        obj[QStringLiteral("msg")] = msg;

    return obj;
}

Envelope Envelope::fromJson(const QJsonObject& obj)
{
    Envelope env;
    env.type      = obj.value(QStringLiteral("type")).toString();
    env.role      = obj.value(QStringLiteral("role")).toString(QStringLiteral("device"));
    env.from      = obj.value(QStringLiteral("from")).toString();
    env.to        = obj.value(QStringLiteral("to")).toString(QStringLiteral("server"));
    env.msgId     = obj.value(QStringLiteral("msgId")).toString();
    env.inReplyTo = obj.value(QStringLiteral("inReplyTo")).toString();
    env.ts        = static_cast<qint64>(obj.value(QStringLiteral("ts")).toDouble());
    env.ack       = obj.value(QStringLiteral("ack")).toBool();
    env.code      = obj.value(QStringLiteral("code")).toInt();
    env.msg       = obj.value(QStringLiteral("msg")).toString();

    // Extract non-envelope keys into data; also merge nested "data" if present
    if (obj.contains(QStringLiteral("data")) && obj.value(QStringLiteral("data")).isObject())
        env.data = obj.value(QStringLiteral("data")).toObject();

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (!s_envelopeKeys.contains(it.key())) {
            env.data[it.key()] = it.value();
        }
    }

    return env;
}

// ============================================================
// Core
// ============================================================

QString generateMsgId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

Envelope buildEnvelope(const QString& type,
                       const QString& from,
                       const QJsonObject& data,
                       const QString& inReplyTo,
                       bool ack)
{
    Envelope env;
    env.type      = type;
    env.from      = from;
    env.data      = data;
    env.inReplyTo = inReplyTo;
    env.ack       = ack;
    env.msgId     = generateMsgId();
    env.ts        = QDateTime::currentMSecsSinceEpoch();
    return env;
}

// ============================================================
// Message builders
// ============================================================

QJsonObject buildAuth(const QString& deviceId, const QString& deviceKey)
{
    QJsonObject data;
    data[QStringLiteral("deviceId")]  = deviceId;
    data[QStringLiteral("deviceKey")] = deviceKey;
    return buildEnvelope(kTypeAuth, deviceId, data).toJson();
}

QJsonObject buildHeartbeat(const QString& deviceId)
{
    return buildEnvelope(kTypeHeartbeat, deviceId).toJson();
}

QJsonObject buildSyncRequest(const QString& deviceId)
{
    return buildEnvelope(kTypeSyncRequest, deviceId).toJson();
}

QJsonObject buildAttendanceReport(const QString& employeeId,
                                  const QDateTime& checkTime,
                                  const QString& deviceId,
                                  const QString& status,
                                  bool awaitPhoto,
                                  const QString& clientMsgId)
{
    QJsonObject data;
    data[QStringLiteral("employeeId")] = employeeId;
    data[QStringLiteral("checkTime")]  = checkTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    data[QStringLiteral("deviceId")]   = deviceId;
    data[QStringLiteral("status")]     = status;
    if (awaitPhoto)
        data[QStringLiteral("awaitPhoto")] = true;

    Envelope env = buildEnvelope(kTypeAttendanceReport, deviceId, data);
    if (!clientMsgId.isEmpty())
        env.msgId = clientMsgId;  // 幂等: 重试保持相同 msgId
    return env.toJson();
}

QJsonObject buildAttendancePhotoHeader(const QString& deviceId,
                                       const QString& employeeId,
                                       int payloadLength)
{
    QJsonObject data;
    data[QStringLiteral("employeeId")]    = employeeId;
    data[QStringLiteral("payloadLength")] = payloadLength;
    data[QStringLiteral("contentType")]   = QStringLiteral("image/jpeg");
    data[QStringLiteral("payloadEncoding")] = QStringLiteral("raw");
    return buildEnvelope(kTypeAttendancePhotoHeader, deviceId, data).toJson();
}

QJsonObject buildDeviceStatusReport(const QString& deviceId,
                                    const QString& deviceName,
                                    const QString& ipAddress,
                                    const QString& fwVersion,
                                    const QJsonObject& extra)
{
    QJsonObject data = extra;
    data[QStringLiteral("deviceId")] = deviceId;
    if (!deviceName.isEmpty())
        data[QStringLiteral("deviceName")] = deviceName;
    if (!ipAddress.isEmpty())
        data[QStringLiteral("ipAddress")] = ipAddress;
    if (!fwVersion.isEmpty())
        data[QStringLiteral("fwVersion")] = fwVersion;
    return buildEnvelope(kTypeDeviceStatusReport, deviceId, data).toJson();
}

QJsonObject buildCommandAck(const QString& deviceId,
                            const QString& inReplyTo,
                            const QString& command,
                            bool success,
                            const QString& message)
{
    QJsonObject data;
    data[QStringLiteral("command")] = command;
    data[QStringLiteral("success")] = success;
    if (!message.isEmpty())
        data[QStringLiteral("message")] = message;
    return buildEnvelope(kTypeDeviceCommandAck, deviceId, data, inReplyTo, /*ack=*/true).toJson();
}

// ============================================================
// parseType
// ============================================================

MessageType parseType(const QJsonObject& msg)
{
    const QString t = msg.value(QStringLiteral("type")).toString();
    if (t == QLatin1StringView(kTypeAuthResponse))
        return MessageType::AuthResponse;
    if (t == QLatin1StringView(kTypeHeartbeatResponse))
        return MessageType::HeartbeatResponse;
    if (t == QLatin1StringView(kTypePersonSync))
        return MessageType::PersonSync;
    if (t == QLatin1StringView(kTypeFaceSyncBegin))
        return MessageType::FaceSyncBegin;
    if (t == QLatin1StringView(kTypeFaceSyncItemHeader))
        return MessageType::FaceSyncItemHeader;
    if (t == QLatin1StringView(kTypeFaceSyncEnd))
        return MessageType::FaceSyncEnd;
    if (t == QLatin1StringView(kTypeAttendanceReportResponse))
        return MessageType::AttendanceReportResponse;
    if (t == QLatin1StringView(kTypeDeviceStatusReportResponse))
        return MessageType::DeviceStatusReportResponse;
    if (t == QLatin1StringView(kTypeDeviceCommand))
        return MessageType::DeviceCommand;
    if (t == QLatin1StringView(kTypeTokenRefreshResponse))
        return MessageType::TokenRefreshResponse;
    if (t == QLatin1StringView(kTypeError))
        return MessageType::Error;
    return MessageType::Unknown;
}

// ============================================================
// Parsers
// ============================================================

QVector<PersonSyncItem> parsePersons(const QJsonObject& msg)
{
    QVector<PersonSyncItem> out;

    // Support both root-level "persons" and nested under "data"
    QJsonArray arr;
    if (msg.value(QStringLiteral("persons")).isArray()) {
        arr = msg.value(QStringLiteral("persons")).toArray();
    } else if (msg.value(QStringLiteral("data")).isObject()) {
        const QJsonObject d = msg.value(QStringLiteral("data")).toObject();
        if (d.value(QStringLiteral("persons")).isArray())
            arr = d.value(QStringLiteral("persons")).toArray();
    }

    out.reserve(arr.size());
    for (const auto& v : arr) {
        if (!v.isObject()) continue;
        const QJsonObject o = v.toObject();

        PersonSyncItem p;
        p.id         = o.value(QStringLiteral("id")).toInt();
        p.name       = o.value(QStringLiteral("name")).toString();
        p.employeeId = o.value(QStringLiteral("employeeId")).toString();
        p.department = o.value(QStringLiteral("department")).toString();
        p.position   = o.value(QStringLiteral("position")).toString();
        out.push_back(p);
    }
    return out;
}

// ============================================================
// PersonData
// ============================================================

QJsonObject PersonData::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("employeeId")]  = employeeId;
    obj[QStringLiteral("name")]        = name;
    obj[QStringLiteral("faceFeature")] = QString::fromUtf8(faceFeature.toBase64());
    obj[QStringLiteral("featureSize")] = featureSize;
    return obj;
}

PersonData PersonData::fromJson(const QJsonObject& obj)
{
    PersonData p;
    p.employeeId  = obj.value(QStringLiteral("employeeId")).toString();
    p.name        = obj.value(QStringLiteral("name")).toString();
    p.faceFeature = QByteArray::fromBase64(
        obj.value(QStringLiteral("faceFeature")).toString().toUtf8());
    p.featureSize = obj.value(QStringLiteral("featureSize")).toInt();
    return p;
}

// ============================================================
// AttendanceRecord
// ============================================================

QJsonObject AttendanceRecord::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("employeeId")] = employeeId;
    obj[QStringLiteral("checkTime")]  = checkTime;
    obj[QStringLiteral("status")]     = status;
    return obj;
}

AttendanceRecord AttendanceRecord::fromJson(const QJsonObject& obj)
{
    AttendanceRecord r;
    r.employeeId = obj.value(QStringLiteral("employeeId")).toString();
    r.checkTime  = obj.value(QStringLiteral("checkTime")).toString();
    r.status     = obj.value(QStringLiteral("status")).toString();
    return r;
}

// ============================================================
// Token refresh
// ============================================================

QJsonObject buildTokenRefreshRequest(const QString& refreshToken)
{
    QJsonObject data;
    data[QStringLiteral("refreshToken")] = refreshToken;
    return buildEnvelope(kTypeTokenRefresh, QString(), data).toJson();
}

} // namespace ServerProtocol
