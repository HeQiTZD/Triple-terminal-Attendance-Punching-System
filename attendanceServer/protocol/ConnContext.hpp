#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/asio/steady_timer.hpp>

#include "db/AttendanceRecordRepository.hpp"

namespace protocol {

enum class EndpointRole { Unknown, Device, Admin };

enum class PendingBinaryKind { None, AttendancePhoto };

// 同步下发与失败计数：异步回调通过 shared_ptr 延长寿命，避免 ConnContext 销毁后触达已释放对象。
struct SyncPushGate {
  std::mutex mu;
  bool push_running = false;
  int fail_count = 0;
};

struct PendingAttendance {
  std::string msg_id;
  std::string reply_to;
  db::AttendanceRecordCreateInput row;
};

struct ConnContext {
  EndpointRole endpoint_role = EndpointRole::Unknown;
  bool authenticated = false;
  std::string device_id;
  std::string bound_from;

  // RBAC: 管理端鉴权后从数据库加载
  int user_id = 0;
  std::string employee_id;
  std::string username;
  std::vector<std::string> roles;
  std::unordered_set<std::string> permissions;

  // JWT 令牌相关
  std::string access_token;
  std::string refresh_token;
  int64_t token_expires_at = 0;
  std::string jti;

  int heartbeat_sec = 30;
  std::shared_ptr<boost::asio::steady_timer> heartbeat_timer;
  PendingBinaryKind pending_binary = PendingBinaryKind::None;
  std::uint32_t expected_binary_length = 0;
  std::optional<PendingAttendance> pending_attendance;

  std::shared_ptr<SyncPushGate> sync_gate = std::make_shared<SyncPushGate>();
};

}  // namespace protocol
