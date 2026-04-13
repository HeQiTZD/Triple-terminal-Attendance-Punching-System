#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <QString>
namespace Protocol{
// message["type"]
static inline const QString kType = "type";
static inline const QString kAuth = "auth";
static inline const QString kAuthResponse = "auth_response";
static inline const QString kHeartbeat = "heartbeat";              // 统一拼写
static inline const QString kHeartbeatResponse = "heartbeat_response";
static inline const QString kAttendanceRecord = "attendance_record";
static inline const QString kDeviceStatus = "device_status";
static inline const QString kError = "error";
// common fields
static inline const QString kDeviceId = "deviceId";
static inline const QString kStatus = "status";
static inline const QString kMessage = "message";
// attendance_record payload (建议字段)
static inline const QString kPersonEmployeeId = "employeeId"; // 或 personId，二选一统一
static inline const QString kCheckTime = "checkTime";         // ISO8601 string
static inline const QString kRecordStatus = "recordStatus";   // 比如 "in"/"out"/"success"/"fail"
}

#endif // PROTOCOL_H
