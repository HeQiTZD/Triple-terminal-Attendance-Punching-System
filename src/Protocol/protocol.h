#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <QString>

namespace Protocol {

// ── message envelope fields ──
inline const QString kType       = QStringLiteral("type");
inline const QString kRole       = QStringLiteral("role");
inline const QString kFrom       = QStringLiteral("from");
inline const QString kTo         = QStringLiteral("to");
inline const QString kMsgId      = QStringLiteral("msgId");
inline const QString kInReplyTo  = QStringLiteral("inReplyTo");
inline const QString kTs         = QStringLiteral("ts");
inline const QString kCode       = QStringLiteral("code");
inline const QString kMsg        = QStringLiteral("msg");
inline const QString kData       = QStringLiteral("data");

// ── roles ──
inline const QString kRoleAdmin  = QStringLiteral("admin");
inline const QString kRoleServer = QStringLiteral("server");

// ── auth ──
inline const QString kAuth            = QStringLiteral("auth");
inline const QString kAuthResponse    = QStringLiteral("auth_response");
inline const QString kUsername        = QStringLiteral("username");
inline const QString kPassword        = QStringLiteral("password");
inline const QString kSessionToken    = QStringLiteral("sessionToken");
inline const QString kServerTime      = QStringLiteral("serverTime");
inline const QString kHeartbeatSec    = QStringLiteral("heartbeatSec");
inline const QString kRoles           = QStringLiteral("roles");
inline const QString kPermissions     = QStringLiteral("permissions");

// ── heartbeat ──
inline const QString kHeartbeat          = QStringLiteral("heartbeat");
inline const QString kHeartbeatResponse  = QStringLiteral("heartbeat_response");

// ── person ──
inline const QString kPersonCreate  = QStringLiteral("person.create");
inline const QString kPersonQuery   = QStringLiteral("person.query");
inline const QString kPersonUpdate  = QStringLiteral("person.update");
inline const QString kPersonDelete  = QStringLiteral("person.delete");

// ── attendance ──
inline const QString kAttendanceCreate  = QStringLiteral("attendance.create");
inline const QString kAttendanceQuery   = QStringLiteral("attendance.query");
inline const QString kAttendanceUpdate  = QStringLiteral("attendance.update");
inline const QString kAttendanceDelete  = QStringLiteral("attendance.delete");

// ── attendance archive ──
inline const QString kAttendanceArchiveQuery  = QStringLiteral("attendance.archive.query");
inline const QString kAttendanceArchiveDelete = QStringLiteral("attendance.archive.delete");

// ── device ──
inline const QString kDeviceCreate   = QStringLiteral("device.create");
inline const QString kDeviceQuery    = QStringLiteral("device.query");
inline const QString kDeviceUpdate   = QStringLiteral("device.update");
inline const QString kDeviceDelete   = QStringLiteral("device.delete");
inline const QString kDeviceCommand  = QStringLiteral("device.command");
inline const QString kDeviceStatus   = QStringLiteral("device.status");

// ── user ──
inline const QString kUserCreate  = QStringLiteral("user.create");
inline const QString kUserQuery   = QStringLiteral("user.query");
inline const QString kUserUpdate  = QStringLiteral("user.update");
inline const QString kUserDelete  = QStringLiteral("user.delete");

// ── face ──
inline const QString kFaceQuery             = QStringLiteral("face.query");
inline const QString kFaceDelete            = QStringLiteral("face.delete");
inline const QString kFaceRegisterHeader    = QStringLiteral("face.register.header");

// ── RBAC role ──
inline const QString kRoleCreate  = QStringLiteral("role.create");
inline const QString kRoleQuery   = QStringLiteral("role.query");
inline const QString kRoleUpdate  = QStringLiteral("role.update");
inline const QString kRoleDelete  = QStringLiteral("role.delete");

// ── RBAC user-role ──
inline const QString kUserRoleAssign = QStringLiteral("user.role.assign");
inline const QString kUserRoleRevoke = QStringLiteral("user.role.revoke");
inline const QString kUserRoleQuery  = QStringLiteral("user.role.query");

// ── permission ──
inline const QString kPermissionQuery = QStringLiteral("permission.query");
inline const QString kPermissionSelf  = QStringLiteral("permission.self");

// ── subscribe ──
inline const QString kSubscribe    = QStringLiteral("subscribe");
inline const QString kUnsubscribe  = QStringLiteral("unsubscribe");

// ── error ──
inline const QString kError = QStringLiteral("error");

// ── common data fields ──
inline const QString kDeviceId      = QStringLiteral("deviceId");
inline const QString kDeviceName    = QStringLiteral("deviceName");
inline const QString kIpAddress     = QStringLiteral("ipAddress");
inline const QString kLastOnline    = QStringLiteral("lastOnline");
inline const QString kStatus        = QStringLiteral("status");
inline const QString kMessage       = QStringLiteral("message");

inline const QString kEmployeeId    = QStringLiteral("employeeId");
inline const QString kName          = QStringLiteral("name");
inline const QString kDepartment    = QStringLiteral("department");
inline const QString kPosition      = QStringLiteral("position");
inline const QString kCheckTime     = QStringLiteral("checkTime");
inline const QString kReceivedTime  = QStringLiteral("receivedTime");
inline const QString kRecords       = QStringLiteral("records");

inline const QString kCreatedAt     = QStringLiteral("createdAt");
inline const QString kUpdatedAt     = QStringLiteral("updatedAt");

inline const QString kRoleKey       = QStringLiteral("roleKey");
inline const QString kRoleName      = QStringLiteral("roleName");
inline const QString kDescription   = QStringLiteral("description");
inline const QString kIsSystem      = QStringLiteral("isSystem");

inline const QString kPermKey       = QStringLiteral("permKey");
inline const QString kPermName      = QStringLiteral("permName");
inline const QString kResource      = QStringLiteral("resource");

inline const QString kUserId        = QStringLiteral("userId");
inline const QString kCommand       = QStringLiteral("command");
inline const QString kParams        = QStringLiteral("params");
inline const QString kTopics        = QStringLiteral("topics");
inline const QString kFound         = QStringLiteral("found");
inline const QString kFeatureSize   = QStringLiteral("featureSize");
inline const QString kPayloadLength = QStringLiteral("payloadLength");

// ── binary frame fields (sync) ──
inline const QString kPayload       = QStringLiteral("payload");
inline const QString kPersons       = QStringLiteral("persons");
inline const QString kFaceSyncBegin      = QStringLiteral("face_sync_begin");
inline const QString kFaceSyncItemHeader = QStringLiteral("face_sync_item_header");
inline const QString kFaceSyncEnd        = QStringLiteral("face_sync_end");
inline const QString kSyncAck       = QStringLiteral("sync_ack");
inline const QString kSyncRequest   = QStringLiteral("sync_request");
inline const QString kPersonSync    = QStringLiteral("person_sync");
inline const QString kConfigSync    = QStringLiteral("config_sync");

// ── attendance push ──
inline const QString kAttendancePush = QStringLiteral("attendance.push");

// ── response suffix helper ──
inline QString responseType(const QString &requestType) {
    return requestType + QStringLiteral(".response");
}

// ── error codes ──
namespace ErrorCode {
    constexpr int kSuccess               = 0;
    constexpr int kParseError            = 1001;
    constexpr int kPayloadTooLarge       = 1002;
    constexpr int kNotAuthenticated      = 2001;
    constexpr int kAuthFailed            = 2002;
    constexpr int kDuplicateSession      = 2003;
    constexpr int kForbidden             = 3001;
    constexpr int kBusinessValidation    = 4000;
    constexpr int kEmployeeNotFound      = 4001;
    constexpr int kDeviceOffline         = 5001;
    constexpr int kForwardTimeout        = 5002;
    constexpr int kDuplicateKey          = 6001;
    constexpr int kDbError               = 6002;
}

} // namespace Protocol

#endif // PROTOCOL_H
