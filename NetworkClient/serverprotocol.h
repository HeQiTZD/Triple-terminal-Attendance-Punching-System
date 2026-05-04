#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QVector>

namespace ServerProtocol {

// keys
inline constexpr const char* kType = "type";
inline constexpr const char* kDeviceId = "deviceId";
inline constexpr const char* kStatus = "status";
inline constexpr const char* kMessage = "message";

// attendance_record keys
inline constexpr const char* kEmployeeId = "employeeId";
inline constexpr const char* kCheckTime = "checkTime"; // ISO8601 string

// sync keys
inline constexpr const char* kPersons = "persons";

// types (must match AttendanceServer)
inline constexpr const char* tAuth = "auth";
inline constexpr const char* tAuthResponse = "auth_response";
inline constexpr const char* tHeartbeat = "heartbeat";
inline constexpr const char* tHeartbeatResponse = "heartbeat_response";
inline constexpr const char* tAttendanceRecord = "attendance_record";
inline constexpr const char* tDeviceStatus = "device_status";
inline constexpr const char* tSyncRequest = "sync_request";
inline constexpr const char* tPersonSync = "person_sync";
inline constexpr const char* tError = "error";

enum class MessageType {
    AuthResponse,
    HeartbeatResponse,
    PersonSync,
    Error,
    Unknown
};

MessageType parseType(const QJsonObject& msg);

QJsonObject buildAuth(const QString& deviceId);
QJsonObject buildHeartbeat();
QJsonObject buildSyncRequest(const QString& deviceId);

QJsonObject buildAttendanceRecord(const QString& employeeId,
                                  const QDateTime& checkTime,
                                  const QString& deviceId,
                                  const QString& status = "ok");

// `statusPayload` is optional extra fields like deviceName/ipAddress/etc.
QJsonObject buildDeviceStatus(const QString& deviceId,
                              const QString& status = "online",
                              const QJsonObject& statusPayload = {});

struct PersonSyncItem {
    int id = 0;
    QString name;
    QString employeeId;
    QString department;
    QString position;
};

QVector<PersonSyncItem> parsePersons(const QJsonObject& msg);

} // namespace ServerProtocol

