#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVector>

namespace ServerProtocol {

// ============================================================
// Envelope — 完整消息信封 (通信协议 §5)
// ============================================================
struct Envelope {
    QString type;       // 点分消息类型
    QString role = QStringLiteral("device");
    QString from;       // deviceId
    QString to = QStringLiteral("server");
    QString msgId;
    QString inReplyTo;
    qint64  ts   = 0;
    bool    ack  = false;
    int     code = 0;
    QString msg;
    QJsonObject data;   // payload — merged at root in wire format

    QJsonObject toJson() const;
    static Envelope fromJson(const QJsonObject& obj);
};

// ============================================================
// Message type constants — aligned with ProtocolTypes.hpp
// ============================================================

// Core
inline constexpr const char* kTypeAuth              = "auth";
inline constexpr const char* kTypeAuthResponse      = "auth_response";
inline constexpr const char* kTypeHeartbeat         = "heartbeat";
inline constexpr const char* kTypeHeartbeatResponse = "heartbeat_response";
inline constexpr const char* kTypeError             = "error";

// Attendance
inline constexpr const char* kTypeAttendanceReport         = "attendance.report";
inline constexpr const char* kTypeAttendanceReportResponse = "attendance.report.response";
inline constexpr const char* kTypeAttendancePhotoHeader    = "attendance.photo.header";

// Device
inline constexpr const char* kTypeDeviceStatusReport         = "device.status.report";
inline constexpr const char* kTypeDeviceStatusReportResponse = "device.status.report.response";
inline constexpr const char* kTypeDeviceCommand              = "device.command";
inline constexpr const char* kTypeDeviceCommandAck           = "device.command.ack";

// Sync
inline constexpr const char* kTypeSyncRequest       = "sync.request";
inline constexpr const char* kTypeSyncAck           = "sync.ack";
inline constexpr const char* kTypePersonSync        = "person.sync";
inline constexpr const char* kTypeFaceSyncBegin     = "face.sync.begin";
inline constexpr const char* kTypeFaceSyncItemHeader = "face.sync.item.header";
inline constexpr const char* kTypeFaceSyncEnd       = "face.sync.end";

// ============================================================
// Roles
// ============================================================
inline constexpr const char* kRoleDevice = "device";
inline constexpr const char* kRoleServer = "server";
inline constexpr const char* kRoleAdmin  = "admin";

// ============================================================
// Error codes (通信协议 §8)
// ============================================================
inline constexpr int kCodeOk                = 0;
inline constexpr int kCodeParseError        = 1001;
inline constexpr int kCodePayloadTooLarge   = 1002;
inline constexpr int kCodeNotAuthenticated  = 2001;
inline constexpr int kCodeAuthFailed        = 2002;
inline constexpr int kCodeDuplicateSession  = 2003;
inline constexpr int kCodeForbidden         = 3001;
inline constexpr int kCodeEmployeeNotFound  = 4001;
inline constexpr int kCodeDeviceOffline     = 5001;
inline constexpr int kCodeForwardTimeout    = 5002;
inline constexpr int kCodeDuplicateKey      = 6001;
inline constexpr int kCodeDbError           = 6002;

// ============================================================
// Envelope / payload key constants
// ============================================================
inline constexpr const char* kType       = "type";
inline constexpr const char* kDeviceId   = "deviceId";
inline constexpr const char* kDeviceKey  = "deviceKey";
inline constexpr const char* kStatus     = "status";
inline constexpr const char* kMessage    = "message";
inline constexpr const char* kEmployeeId = "employeeId";
inline constexpr const char* kCheckTime  = "checkTime";
inline constexpr const char* kPersons    = "persons";

// ============================================================
// Message type enum for routing received messages
// ============================================================
enum class MessageType {
    AuthResponse,
    HeartbeatResponse,
    PersonSync,
    FaceSyncBegin,
    FaceSyncItemHeader,
    FaceSyncEnd,
    AttendanceReportResponse,
    DeviceStatusReportResponse,
    DeviceCommand,
    Error,
    Unknown
};

// ============================================================
// Core functions
// ============================================================

MessageType parseType(const QJsonObject& msg);

/// Unified envelope builder — auto-fills msgId (UUID) and ts.
Envelope buildEnvelope(const QString& type,
                       const QString& from,
                       const QJsonObject& data = {},
                       const QString& inReplyTo = {},
                       bool ack = false);

// ============================================================
// Message builders — return ready-to-send QJsonObject
// ============================================================

QJsonObject buildAuth(const QString& deviceId, const QString& deviceKey);
QJsonObject buildHeartbeat(const QString& deviceId = {});
QJsonObject buildSyncRequest(const QString& deviceId);

QJsonObject buildAttendanceReport(const QString& employeeId,
                                  const QDateTime& checkTime,
                                  const QString& deviceId,
                                  const QString& status = QStringLiteral("ok"));

/// deviceName / ipAddress / fwVersion map to standard payload keys;
/// extra is merged on top for custom fields.
QJsonObject buildDeviceStatusReport(const QString& deviceId,
                                    const QString& deviceName = {},
                                    const QString& ipAddress = {},
                                    const QString& fwVersion = {},
                                    const QJsonObject& extra = {});

QJsonObject buildCommandAck(const QString& deviceId,
                            const QString& inReplyTo,
                            const QString& command,
                            bool success,
                            const QString& message = {});

// ============================================================
// Data structs
// ============================================================

struct PersonSyncItem {
    int     id         = 0;
    QString name;
    QString employeeId;
    QString department;
    QString position;
};

/// Person data for local storage (supersedes Protocol::PersonData).
struct PersonData {
    QString    employeeId;
    QString    name;
    QByteArray faceFeature;
    int        featureSize = 0;

    QJsonObject toJson() const;
    static PersonData fromJson(const QJsonObject& obj);
};

/// Attendance record for local storage (supersedes Protocol::AttendanceRecord).
struct AttendanceRecord {
    QString employeeId;
    QString checkTime;
    QString status;

    QJsonObject toJson() const;
    static AttendanceRecord fromJson(const QJsonObject& obj);
};

// ============================================================
// Parsers
// ============================================================

QVector<PersonSyncItem> parsePersons(const QJsonObject& msg);

// ============================================================
// Utility
// ============================================================

QString generateMsgId();

} // namespace ServerProtocol
