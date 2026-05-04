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
static inline const QString kPersonEmployeeId = "employeeId";
static inline const QString kCheckTime = "checkTime";         // ISO8601 string
static inline const QString kRecordStatus = "recordStatus";   // 比如 "in"/"out"/"success"/"fail"

static inline const QString kSyncRequest = "sync_request";
static inline const QString kPersonSync = "person_sync";
static inline const QString kConfigSync = "config_sync";
// Face data full sync (JSON header + binary frame)
static inline const QString kFaceSyncBegin = "face_sync_begin";
static inline const QString kFaceSyncItemHeader = "face_sync_item_header";
static inline const QString kFaceSyncEnd = "face_sync_end";
static inline const QString kSyncAck = "sync_ack";

static inline const QString kPayload = "payload";
static inline const QString kPersons = "persons";
static inline const QString kEmployeeId = "employeeId";
static inline const QString kFeatureSize = "featureSize";
static inline const QString kPayloadLength = "payloadLength";
}

#endif // PROTOCOL_H
