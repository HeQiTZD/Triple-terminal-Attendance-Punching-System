#pragma once

#include <cstdint>
#include <string_view>

namespace protocol {

// 通信协议 §5 常用 type（点分命名）。比较策略：区分大小写。
inline constexpr std::string_view kTypeAuth = "auth";
inline constexpr std::string_view kTypeAuthResponse = "auth_response";
inline constexpr std::string_view kTypeHeartbeat = "heartbeat";
inline constexpr std::string_view kTypeHeartbeatResponse = "heartbeat_response";
inline constexpr std::string_view kTypeError = "error";

inline constexpr std::string_view kTypeAttendanceReport = "attendance.report";
inline constexpr std::string_view kTypeDeviceStatusReport = "device.status.report";
inline constexpr std::string_view kTypeSyncRequest = "sync.request";
inline constexpr std::string_view kTypeSyncAck = "sync.ack";
inline constexpr std::string_view kTypePersonSync = "person.sync";
inline constexpr std::string_view kTypeFaceSyncBegin = "face.sync.begin";
inline constexpr std::string_view kTypeFaceSyncItemHeader =
    "face.sync.item.header";
inline constexpr std::string_view kTypeFaceSyncEnd = "face.sync.end";
inline constexpr std::string_view kTypeAttendancePhotoHeader =
    "attendance.photo.header";
inline constexpr std::string_view kTypeFaceRegisterResponse =
    "face.register.response";
inline constexpr std::string_view kTypeAttendancePush = "attendance.push";

// 设备状态推送事件
inline constexpr std::string_view kTypeDeviceStatusPush = "device.status.push";
inline constexpr std::string_view kTypeDeviceCommand = "device.command";
inline constexpr std::string_view kTypeDeviceCommandAck =
    "device.command.ack";
inline constexpr std::string_view kTypeConfigDeploy = "config.deploy";
inline constexpr std::string_view kTypeConfigDeployResponse =
    "config.deploy.response";
inline constexpr std::string_view kTypeSubscribe = "subscribe";
inline constexpr std::string_view kTypeUnsubscribe = "unsubscribe";
inline constexpr std::string_view kTypeSubscribeResponse =
    "subscribe.response";
inline constexpr std::string_view kTypeUnsubscribeResponse =
    "unsubscribe.response";
inline constexpr std::string_view kTypeAttendanceReportResponse =
    "attendance.report.response";
inline constexpr std::string_view kTypeDeviceStatusReportResponse =
    "device.status.report.response";

// 管理端 CRUD：人员管理
inline constexpr std::string_view kTypePersonCreate = "person.create";
inline constexpr std::string_view kTypePersonCreateResponse =
    "person.create.response";
inline constexpr std::string_view kTypePersonQuery = "person.query";
inline constexpr std::string_view kTypePersonQueryResponse =
    "person.query.response";
inline constexpr std::string_view kTypePersonUpdate = "person.update";
inline constexpr std::string_view kTypePersonUpdateResponse =
    "person.update.response";
inline constexpr std::string_view kTypePersonDelete = "person.delete";
inline constexpr std::string_view kTypePersonDeleteResponse =
    "person.delete.response";

// 管理端 CRUD：考勤记录管理
inline constexpr std::string_view kTypeAttendanceCreate = "attendance.create";
inline constexpr std::string_view kTypeAttendanceCreateResponse =
    "attendance.create.response";
inline constexpr std::string_view kTypeAttendanceQuery = "attendance.query";
inline constexpr std::string_view kTypeAttendanceQueryResponse =
    "attendance.query.response";
inline constexpr std::string_view kTypeAttendanceUpdate = "attendance.update";
inline constexpr std::string_view kTypeAttendanceUpdateResponse =
    "attendance.update.response";
inline constexpr std::string_view kTypeAttendanceDelete = "attendance.delete";
inline constexpr std::string_view kTypeAttendanceDeleteResponse =
    "attendance.delete.response";
inline constexpr std::string_view kTypeAttendanceArchiveQuery =
    "attendance.archive.query";
inline constexpr std::string_view kTypeAttendanceArchiveQueryResponse =
    "attendance.archive.query.response";
inline constexpr std::string_view kTypeAttendanceArchiveDelete =
    "attendance.archive.delete";
inline constexpr std::string_view kTypeAttendanceArchiveDeleteResponse =
    "attendance.archive.delete.response";

// 管理端 CRUD：设备管理
inline constexpr std::string_view kTypeDeviceCreate = "device.create";
inline constexpr std::string_view kTypeDeviceCreateResponse =
    "device.create.response";
inline constexpr std::string_view kTypeDeviceQuery = "device.query";
inline constexpr std::string_view kTypeDeviceQueryResponse =
    "device.query.response";
inline constexpr std::string_view kTypeDeviceUpdate = "device.update";
inline constexpr std::string_view kTypeDeviceUpdateResponse =
    "device.update.response";
inline constexpr std::string_view kTypeDeviceDelete = "device.delete";
inline constexpr std::string_view kTypeDeviceDeleteResponse =
    "device.delete.response";

// 管理端 CRUD：用户账号管理
inline constexpr std::string_view kTypeUserCreate = "user.create";
inline constexpr std::string_view kTypeUserCreateResponse =
    "user.create.response";
inline constexpr std::string_view kTypeUserQuery = "user.query";
inline constexpr std::string_view kTypeUserQueryResponse =
    "user.query.response";
inline constexpr std::string_view kTypeUserUpdate = "user.update";
inline constexpr std::string_view kTypeUserUpdateResponse =
    "user.update.response";
inline constexpr std::string_view kTypeUserDelete = "user.delete";
inline constexpr std::string_view kTypeUserDeleteResponse =
    "user.delete.response";

// 管理端 CRUD：人脸数据（查询/删除，注册已实现）
inline constexpr std::string_view kTypeFaceQuery = "face.query";
inline constexpr std::string_view kTypeFaceQueryResponse =
    "face.query.response";
inline constexpr std::string_view kTypeFaceDelete = "face.delete";
inline constexpr std::string_view kTypeFaceDeleteResponse =
    "face.delete.response";

// 管理端 RBAC：角色管理
inline constexpr std::string_view kTypeRoleCreate = "role.create";
inline constexpr std::string_view kTypeRoleCreateResponse =
    "role.create.response";
inline constexpr std::string_view kTypeRoleQuery = "role.query";
inline constexpr std::string_view kTypeRoleQueryResponse =
    "role.query.response";
inline constexpr std::string_view kTypeRoleUpdate = "role.update";
inline constexpr std::string_view kTypeRoleUpdateResponse =
    "role.update.response";
inline constexpr std::string_view kTypeRoleDelete = "role.delete";
inline constexpr std::string_view kTypeRoleDeleteResponse =
    "role.delete.response";

// 管理端 RBAC：用户角色分配
inline constexpr std::string_view kTypeUserRoleAssign = "user.role.assign";
inline constexpr std::string_view kTypeUserRoleAssignResponse =
    "user.role.assign.response";
inline constexpr std::string_view kTypeUserRoleRevoke = "user.role.revoke";
inline constexpr std::string_view kTypeUserRoleRevokeResponse =
    "user.role.revoke.response";
inline constexpr std::string_view kTypeUserRoleQuery = "user.role.query";
inline constexpr std::string_view kTypeUserRoleQueryResponse =
    "user.role.query.response";

// 管理端 RBAC：权限查询
inline constexpr std::string_view kTypePermissionQuery = "permission.query";
inline constexpr std::string_view kTypePermissionQueryResponse =
    "permission.query.response";
inline constexpr std::string_view kTypePermissionSelf = "permission.self";
inline constexpr std::string_view kTypePermissionSelfResponse =
    "permission.self.response";

// JWT 令牌相关
inline constexpr std::string_view kTypeTokenRefresh = "token.refresh";
inline constexpr std::string_view kTypeTokenRefreshResponse = "token.refresh.response";

inline constexpr std::string_view kRoleDevice = "device";
inline constexpr std::string_view kRoleAdmin = "admin";
inline constexpr std::string_view kRoleServer = "server";

// 通信协议 §8 节选
inline constexpr int kCodeParseError = 1001;
inline constexpr int kCodePayloadTooLarge = 1002;
inline constexpr int kCodeNotAuthenticated = 2001;
inline constexpr int kCodeAuthFailed = 2002;
inline constexpr int kCodeDuplicateSession = 2003;
inline constexpr int kCodeTokenInvalid = 2004;
inline constexpr int kCodeTokenExpired = 2005;
inline constexpr int kCodeRefreshTokenInvalid = 2006;
inline constexpr int kCodeInsufficientPermissions = 2007;
inline constexpr int kCodeForbidden = 3001;
inline constexpr int kCodeDeviceOffline = 5001;
inline constexpr int kCodeForwardTimeout = 5002;
inline constexpr int kCodeBusinessValidation = 4000;
inline constexpr int kCodeEmployeeNotFound = 4001;
inline constexpr int kCodeDuplicateKey = 6001;
inline constexpr int kCodeDbError = 6002;

inline constexpr std::string_view kTypeFaceRegister = "face.register";
}  // namespace protocol
