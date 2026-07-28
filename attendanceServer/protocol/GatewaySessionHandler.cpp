#include "protocol/GatewaySessionHandler.hpp"

#include <cstdio>
#include <cstring>

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "util/DebugLog.hpp"

#include <boost/asio/error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>
#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/DeviceRepository.hpp"
#include "db/OperationLogRepository.hpp"
#include "db/RoleRepository.hpp"
#include "db/UserAccountRepository.hpp"
#include "net/Framing.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/LineCodec.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "protocol/ProtocolUtils.hpp"
#include "protocol/handlers/HeartbeatHandler.hpp"
#include "protocol/handlers/DeviceStatusReportHandler.hpp"
#include "protocol/handlers/AttendanceReportHandler.hpp"
#include "protocol/handlers/SyncHandler.hpp"
#include "protocol/handlers/CrudHandlerFactory.hpp"
#include "protocol/handlers/FaceHandler.hpp"
#include "protocol/handlers/RoleHandler.hpp"
#include "protocol/handlers/DeviceCommandHandler.hpp"
#include "protocol/handlers/ConfigDeployHandler.hpp"
#include "protocol/handlers/SubscribeHandler.hpp"
#include "protocol/handlers/AttendancePhotoHandler.hpp"
#include "service/AdminCrudService.hpp"
#include "service/AttendanceService.hpp"
#include "service/DeviceConfigService.hpp"
#include "service/DeviceService.hpp"
#include "service/SyncService.hpp"
#include "util/JwtUtil.hpp"

#include <cctype>

namespace protocol {

namespace {

void log_parse_failure(std::string_view raw_line) {
  std::cerr << "protocol: json_parse_failed len=" << raw_line.size()
            << " preview=\"";
  constexpr std::size_t kMax = 120;
  std::size_t n = 0;
  for (unsigned char c : raw_line) {
    if (n >= kMax) {
      break;
    }
    if (c == '\n' || c == '\r') {
      std::cerr << '\\' << (c == '\n' ? 'n' : 'r');
    } else if (std::isprint(c) != 0) {
      std::cerr << static_cast<char>(c);
    } else {
      char hex[5];
      std::snprintf(hex, sizeof(hex), "\\x%02x", c);
      std::cerr << hex;
    }
    ++n;
  }
  std::cerr << "\"\n";
}

EndpointRole parse_role(std::string_view r) {
  if (r == kRoleDevice) {
    return EndpointRole::Device;
  }
  if (r == kRoleAdmin) {
    return EndpointRole::Admin;
  }
  return EndpointRole::Unknown;
}

std::string effective_client_route(const ParsedEnvelope& env) {
  if (!env.from.empty()) {
    return env.from;
  }
  if (env.data.contains("deviceId") && env.data["deviceId"].is_string()) {
    return env.data["deviceId"].get<std::string>();
  }
  return {};
}

// read_payload_length 已移至 protocol/ProtocolUtils.hpp

std::string opt_data_string(const nlohmann::json& data, const char* key) {
  if (!data.contains(key) || !data[key].is_string()) {
    return {};
  }
  return data[key].get<std::string>();
}

// remote_ip_string, has_permission, has_role 已移至 protocol/ProtocolUtils.hpp

}  // namespace

GatewaySessionHandler::GatewaySessionHandler(
    service::DeviceRegistry& registry, service::DbExecutor& db_executor,
    GatewayConfig cfg, service::AdminRegistry& admin_registry,
    service::EventSubscriptionRegistry& subscriptions)
    : registry_(registry),
      db_(db_executor),
      admin_registry_(admin_registry),
      subscriptions_(subscriptions),
      cfg_(std::move(cfg)) {
  // 初始化 JWT 配置
  util::JwtConfig jwtConfig;
  jwtConfig.secret = cfg_.jwt_secret.empty() ? "default-jwt-secret-change-in-production-32bytes" : cfg_.jwt_secret;
  jwtConfig.access_token_ttl = cfg_.access_token_ttl;
  jwtConfig.refresh_token_ttl = cfg_.refresh_token_ttl;
  jwtConfig.clock_skew = cfg_.clock_skew;

  tokenManager_ = std::make_unique<service::TokenManager>(jwtConfig, db_);

  // 注册消息处理器
  register_handlers();
}

void GatewaySessionHandler::register_handlers() {
  using namespace handlers;

  // 设备端消息
  handler_registry_.register_handler(std::string(kTypeHeartbeat),
      [this](net::ISession& session, const ParsedEnvelope& env,
             ConnContext& ctx, const std::string& route_to) {
        arm_heartbeat(session, ctx);
        handle_heartbeat(session, env, ctx, route_to);
      });

  handler_registry_.register_handler(std::string(kTypeDeviceStatusReport),
      [this](net::ISession& session, const ParsedEnvelope& env,
             ConnContext& ctx, const std::string& route_to) {
        handle_device_status_report(session, env, ctx, route_to, db_);
      });

  handler_registry_.register_handler(std::string(kTypeAttendanceReport),
      [this](net::ISession& session, const ParsedEnvelope& env,
             ConnContext& ctx, const std::string& route_to) {
        handle_attendance_report(session, env, ctx, route_to, db_, &subscriptions_);
      });

  handler_registry_.register_handler(std::string(kTypeSyncRequest),
      [this](net::ISession& session, const ParsedEnvelope& env,
             ConnContext& ctx, const std::string& route_to) {
        handle_sync_request(session, env, ctx, route_to, db_);
      });

  handler_registry_.register_handler(std::string(kTypeSyncAck),
      [this](net::ISession& session, const ParsedEnvelope& env,
             ConnContext& ctx, const std::string& route_to) {
        handle_sync_ack(session, env, ctx, route_to);
      });

  // 设备命令和配置
  register_device_command_handlers(handler_registry_, router_.get());
  register_config_deploy_handler(handler_registry_, db_, router_.get());

  // 事件订阅
  register_subscribe_handlers(handler_registry_, subscriptions_);

  // 考勤照片
  register_attendance_photo_handler(handler_registry_);

  // Admin CRUD 处理器
  register_person_handlers(handler_registry_, db_);
  register_attendance_handlers(handler_registry_, db_);
  register_device_handlers(handler_registry_, db_);
  register_user_handlers(handler_registry_, db_);
  register_face_handlers(handler_registry_, db_);
  register_role_handlers(handler_registry_, db_,
      [this](int user_id) {
        reload_permissions(std::to_string(user_id));
      });
}

void GatewaySessionHandler::reload_permissions(
    const std::string& operator_id) {
  admin_registry_.with_session(
      operator_id, [this, operator_id](net::ISession& sess) {
        db_.dispatch(
            [this, operator_id,
             wp = sess.weak_from_this()](mysqlx::Session& s) {
              int uid = 0;
              {
                auto sp = wp.lock();
                if (!sp) return;
                std::lock_guard<std::mutex> lk(mu_);
                auto it = connections_.find(sp.get());
                if (it == connections_.end()) return;
                uid = it->second.user_id;
              }
              if (uid <= 0) return;
              auto roles = db::RoleRepository::loadUserRoles(s, uid);
              auto perms = db::RoleRepository::loadUserPermissions(s, uid);
              auto sp = wp.lock();
              if (!sp) return;
              std::lock_guard<std::mutex> lk(mu_);
              auto it = connections_.find(sp.get());
              if (it == connections_.end()) return;
              it->second.roles = std::move(roles);
              it->second.permissions = std::move(perms);
            },
            [](std::exception_ptr ep) {
              if (ep) {
                try {
                  std::rethrow_exception(ep);
                } catch (const std::exception& ex) {
                  std::cerr << "reload_permissions failed: " << ex.what()
                            << '\n';
                }
              }
            });
      });
}

void GatewaySessionHandler::cancel_heartbeat(ConnContext& ctx) const {
  if (ctx.heartbeat_timer) {
    ctx.heartbeat_timer->cancel();
  }
}

void GatewaySessionHandler::arm_heartbeat(net::ISession& session,
                                          ConnContext& ctx) {
  // 仅在锁内创建 timer（如未初始化），timer 操作本身延迟到锁外执行，
  // 避免 DB 线程持 mu_ 时直接操作 timer 引发线程模型问题。
  if (!ctx.heartbeat_timer) {
    ctx.heartbeat_timer =
        std::make_shared<boost::asio::steady_timer>(session.strand());
  }

  const int mult = std::max(1, cfg_.heartbeat_grace_multiplier);
  const auto duration = std::chrono::seconds(ctx.heartbeat_sec * mult);
  auto wp = session.weak_from_this();
  auto timer = ctx.heartbeat_timer;

  // 将实际的 cancel / expires_after / async_wait 派发到 session 的 strand 上执行：
  // - 若当前已在该 strand（io_context 线程），dispatch 直接内联执行；
  // - 若从 DB 线程调用，dispatch 将操作投递到 strand，不持锁等待。
  boost::asio::dispatch(session.strand(),
                        [timer, duration, wp]() mutable {
    // session 可能在 dispatch 投递后、执行前已关闭，提前退出避免重新 arm
    if (!wp.lock()) {
      return;
    }
    timer->cancel();
    timer->expires_after(duration);
    timer->async_wait([wp, timer](const boost::system::error_code& ec) {
      if (ec == boost::asio::error::operation_aborted) {
        return;
      }
      if (ec) {
        return;
      }
      auto sp = wp.lock();
      if (!sp) {
        return;
      }
      sp->request_close("heartbeat_timeout");
    });
  });
}

bool GatewaySessionHandler::device_key_matches(const std::string& device_id,
                                               const std::string& key) const {
  auto it = cfg_.device_keys.find(device_id);
  const std::string& expected =
      it != cfg_.device_keys.end() ? it->second : cfg_.default_device_key;
  return key == expected;
}

void GatewaySessionHandler::erase_connection(net::ISession* s) {
  ConnContext ctx;
  bool removed = false;
  {
    std::lock_guard<std::mutex> lk(mu_);
    auto it = connections_.find(s);
    if (it != connections_.end()) {
      ctx = std::move(it->second);
      connections_.erase(it);
      removed = true;
    }
  }
  if (!removed) {
    return;
  }
  INFO_SESSION("连接清理: role=" << static_cast<int>(ctx.endpoint_role)
               << " device_id=" << ctx.device_id
               << " bound_from=" << ctx.bound_from
               << " authenticated=" << ctx.authenticated);
  cancel_heartbeat(ctx);
  subscriptions_.unsubscribe_all(s);
  if (router_) {
    router_->cancel_pending_for_session(s);
  }
  if (ctx.authenticated && ctx.endpoint_role == EndpointRole::Admin &&
      !ctx.bound_from.empty()) {
    admin_registry_.unregister_session(ctx.bound_from, s);
  }
  if (ctx.authenticated && ctx.endpoint_role == EndpointRole::Device &&
      !ctx.device_id.empty()) {
    registry_.unregister(ctx.device_id, s);
    const std::string device_id = ctx.device_id;
    db_.dispatch(
        [device_id](mysqlx::Session& session) {
          db::DeviceRepository::markDeviceOffline(session, device_id);
        },
        [](std::exception_ptr ep) {
          if (!ep) {
            return;
          }
          try {
            std::rethrow_exception(ep);
          } catch (const std::exception& ex) {
            std::cerr << "db: markDeviceOffline failed: " << ex.what() << '\n';
          }
        });

    // 推送设备离线事件给管理端
    {
      nlohmann::json push_msg = {
          {"type", std::string(kTypeDeviceStatusPush)},
          {"role", std::string(kRoleServer)},
          {"from", std::string(kRoleServer)},
          {"ts", std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count()},
          {"code", 0},
          {"msg", "ok"},
          {"data", {{"deviceId", device_id}, {"status", "offline"}}}};
      subscriptions_.publish("device", push_msg.dump());
    }
  }
}

void GatewaySessionHandler::on_line(net::ISession& session, std::string line) {
  // 1. JSON 解析
  std::optional<nlohmann::json> jopt = parse_json_line(line);
  if (!jopt) {
    log_parse_failure(line);
    return;
  }

  // 2. 信封解析
  std::optional<ParsedEnvelope> envopt = try_parse_envelope(*jopt);
  if (!envopt) {
    std::cerr << "protocol: envelope_missing_type\n";
    return;
  }

  ParsedEnvelope env = std::move(*envopt);
  const std::string& canon = env.type;
  const std::string route_to = effective_client_route(env);

  DBG_PROTO("收到消息: type=" << canon << " from=" << env.from
            << " msgId=" << env.msg_id << " to=" << route_to);

  // 3. 认证消息特殊处理（无需锁）
  if (canon == std::string(kTypeAuth)) {
    const EndpointRole declared = parse_role(env.role);
    if (declared == EndpointRole::Admin) {
      handleAdminAuth(session, env);
    } else if (declared == EndpointRole::Device) {
      handleDeviceAuth(session, env);
    } else {
      session.write_line(build_error(kCodeAuthFailed, "invalid auth role",
                                     env.msg_id, route_to));
    }
    return;
  }

  if (canon == std::string("token.refresh")) {
    handleTokenRefresh(session, env);
    return;
  }

  // 4. 认证检查
  std::unique_lock<std::mutex> lk(mu_);
  ConnContext& ctx = connections_[&session];

  if (!ctx.authenticated) {
    lk.unlock();
    session.write_line(build_error(kCodeNotAuthenticated, "not authenticated",
                                   env.msg_id, route_to));
    return;
  }

  if (!verifyTokenInEnvelope(env, ctx)) {
    lk.unlock();
    session.write_line(build_error(kCodeTokenInvalid, "invalid or expired token",
                                   env.msg_id, route_to));
    return;
  }

  // 5. 待处理考勤照片检查
  if (ctx.pending_attendance &&
      canon != std::string(kTypeHeartbeat) &&
      canon != std::string(kTypeAttendancePhotoHeader)) {
    ctx.pending_attendance.reset();
    lk.unlock();
    session.write_line(build_error(kCodeBusinessValidation,
                                   "pending attendance photo aborted",
                                   env.msg_id, route_to));
    return;
  }

  // 6. 查表分发
  const auto* handler = handler_registry_.find(canon);
  if (handler) {
    lk.unlock();
    (*handler)(session, env, ctx, route_to);
    return;
  }

  // 7. 未知消息类型
  lk.unlock();
  WARN_PROTO("未知消息类型: type=" << canon << " msgId=" << env.msg_id
             << " from=" << env.from);
  session.write_line(build_error(kCodeBusinessValidation, "unknown message type",
                                 env.msg_id, route_to));
}

void GatewaySessionHandler::on_binary(net::ISession& session,
                                      std::vector<char> payload) {
  PendingBinaryKind kind = PendingBinaryKind::None;
  std::uint32_t expected = 0;
  std::optional<PendingAttendance> attendance_after_photo;
  {
    std::lock_guard<std::mutex> lk(mu_);
    auto it = connections_.find(&session);
    if (it != connections_.end()) {
      kind = it->second.pending_binary;
      expected = it->second.expected_binary_length;
      it->second.pending_binary = PendingBinaryKind::None;
      it->second.expected_binary_length = 0;
      if (kind == PendingBinaryKind::AttendancePhoto) {
        if (it->second.pending_attendance) {
          attendance_after_photo =
              std::move(*it->second.pending_attendance);
          it->second.pending_attendance.reset();
        }
      }
    }
  }

  DBG_PROTO("二进制消息到达: kind=" << static_cast<int>(kind)
            << " bytes=" << payload.size() << " expected=" << expected);

  if (kind == PendingBinaryKind::AttendancePhoto) {
    if (!attendance_after_photo) {
      WARN_NET("照片接收失败: 无待处理考勤记录"
               << "\n  - 当前状态: attendance_after_photo=null"
               << "\n  - 接收字节: " << payload.size());
      session.write_line(build_error(kCodeBusinessValidation,
                                     "attendance photo without pending row",
                                     "", ""));
      return;
    }
    INFO_NET("考勤照片数据接收完成: " << payload.size() << " bytes");
    auto self = session.shared_from_this();
    service::attendance_complete_with_photo(
        std::move(*attendance_after_photo), payload, db_, self,
        &subscriptions_);
    return;
  }

  std::cerr << "protocol binary: kind=" << static_cast<int>(kind)
            << " bytes=" << payload.size() << " expected=" << expected << '\n';
}

void GatewaySessionHandler::on_error(net::ISession& session,
                                     const char* reason) {
  const std::string remote = remote_ip_string(session);
  ConnContext snap;
  {
    std::lock_guard<std::mutex> lk(mu_);
    auto it = connections_.find(&session);
    if (it != connections_.end()) {
      snap = it->second;
    }
  }
  std::cerr << "net session error: reason=" << (reason ? reason : "(null)")
            << " remote=" << remote;
  if (!snap.bound_from.empty()) {
    std::cerr << " bound_from=" << snap.bound_from;
  }
  if (!snap.device_id.empty()) {
    std::cerr << " device_id=" << snap.device_id;
  }
  std::cerr << " role=" << static_cast<int>(snap.endpoint_role);
  if (reason && std::strcmp(reason, "line_too_long") == 0) {
    std::cerr << " max_json_line_bytes=" << net::kMaxJsonLineBytes
              << " proto_code=" << protocol::kCodePayloadTooLarge;
  } else if (reason &&
             (std::strcmp(reason, "binary_expect_too_large") == 0 ||
              std::strcmp(reason, "binary_payload_too_large") == 0)) {
    std::cerr << " max_binary_bytes=" << net::kMaxBinaryPayloadBytes
              << " proto_code=" << protocol::kCodePayloadTooLarge;
  } else if (reason &&
             std::strcmp(reason, "recv_buffer_overflow") == 0) {
    std::cerr << " max_recv_buffer_bytes=" << net::kMaxRecvBufferBytes
              << " proto_code=" << protocol::kCodePayloadTooLarge;
  }
  std::cerr << '\n';
  erase_connection(&session);
}

void GatewaySessionHandler::on_close(net::ISession& session) {
  DBG_SESSION("会话关闭回调触发");
  erase_connection(&session);
}

void GatewaySessionHandler::on_listen_started(boost::asio::io_context& ioc) {
  router_ = std::make_unique<service::MessageRouter>(ioc, registry_);
}

// ============================================================================
// JWT 认证相关方法
// ============================================================================

void GatewaySessionHandler::handleDeviceAuth(net::ISession& session, const ParsedEnvelope& env) {
  const std::string device_id = opt_data_string(env.data, "deviceId");
  const std::string device_key = opt_data_string(env.data, "deviceKey");
  const std::string route_to = device_id;

  INFO_AUTH("设备认证请求: deviceId=" << device_id
            << " ip=" << remote_ip_string(session));

  if (device_id.empty()) {
    WARN_AUTH("设备认证失败: missing deviceId");
    session.write_line(build_error(kCodeAuthFailed, "missing deviceId", env.msg_id, route_to));
    return;
  }

  // 获取设备 IP 地址
  const std::string ip_address = remote_ip_string(session);

  // 检查设备是否存在于数据库
  bool device_exists = false;
  try {
    auto promise = std::make_shared<std::promise<bool>>();
    auto future = promise->get_future();
    db_.dispatch(
        [promise, device_id](mysqlx::Session& s) {
          std::string status = db::DeviceRepository::getDeviceStatus(s, device_id);
          promise->set_value(!status.empty());
        },
        [promise](std::exception_ptr ep) {
          if (ep) {
            try { promise->set_exception(ep); } catch (...) {}
          }
        });
    device_exists = future.get();
  } catch (const std::exception& ex) {
    std::cerr << "check device exists failed: " << ex.what() << '\n';
    session.write_line(build_error(kCodeDbError, "database error", env.msg_id, route_to));
    return;
  }

  // 设备不存在 → 拒绝连接
  if (!device_exists) {
    INFO_AUTH("设备未注册: deviceId=" << device_id);
    session.write_line(build_error(kCodeAuthFailed, "device not registered", env.msg_id, route_to));
    return;
  }

  // 使用 TokenManager 进行认证
  auto result = tokenManager_->authenticate_device(device_id, device_key);

  if (!result.success) {
    WARN_AUTH("设备认证失败: deviceId=" << device_id
              << " error=" << result.error_message);
    session.write_line(build_error(kCodeAuthFailed, result.error_message, env.msg_id, route_to));
    return;
  }

  // 检查重复连接策略
  if (cfg_.duplicate_policy == service::DuplicateDevicePolicy::RejectNew &&
      registry_.has_live_session(device_id)) {
    WARN_AUTH("设备重复连接被拒绝: deviceId=" << device_id);
    session.write_line(build_error(kCodeDuplicateSession, "device already connected", env.msg_id, device_id));
    return;
  }

  auto reg = registry_.register_or_replace(device_id, session.shared_from_this(), cfg_.duplicate_policy);
  if (reg.status == service::DeviceRegisterStatus::RejectedDuplicate) {
    WARN_AUTH("设备重复连接被拒绝: deviceId=" << device_id);
    session.write_line(build_error(kCodeDuplicateSession, "device already connected", env.msg_id, device_id));
    return;
  }

  // 更新连接上下文
  std::string reply;
  {
    std::lock_guard<std::mutex> lk(mu_);
    ConnContext& ctx = connections_[&session];
    ctx.endpoint_role = EndpointRole::Device;
    ctx.authenticated = true;
    ctx.device_id = device_id;
    ctx.bound_from = device_id;
    ctx.heartbeat_sec = cfg_.heartbeat_sec;

    // 存储令牌信息
    ctx.access_token = result.tokens.access_token;
    ctx.refresh_token = result.tokens.refresh_token;
    ctx.token_expires_at = util::JwtUtil::current_timestamp() + result.tokens.expires_in;

    arm_heartbeat(session, ctx);

    // 构建包含令牌的响应
    nlohmann::json data;
    data["accessToken"] = result.tokens.access_token;
    data["refreshToken"] = result.tokens.refresh_token;
    data["tokenType"] = "Bearer";
    data["expiresIn"] = result.tokens.expires_in;
    data["heartbeatSec"] = cfg_.heartbeat_sec;
    data["serverTime"] = util::JwtUtil::current_timestamp() * 1000;
    data["roles"] = nlohmann::json::array();
    data["permissions"] = nlohmann::json::array({"attendance.report", "sync.request", "device.status.report"});

    reply = build_auth_response(env.msg_id, device_id, "device-session-" + device_id,
                                 cfg_.heartbeat_sec, data);
  }

  session.write_line(std::move(reply));
  INFO_AUTH("设备认证成功: deviceId=" << device_id << " ip=" << ip_address);

  // 更新数据库中的设备状态为 online
  db_.dispatch(
      [device_id, ip_address](mysqlx::Session& s) {
        db::DeviceRepository::upsertDeviceOnline(s, device_id, ip_address);
      },
      [device_id](std::exception_ptr ep) {
        if (ep) {
          try {
            std::rethrow_exception(ep);
          } catch (const std::exception& ex) {
            std::cerr << "db: upsertDeviceOnline failed deviceId="
                      << device_id << " error=" << ex.what() << '\n';
          }
        }
      });

  // 推送设备上线事件给管理端
  {
    nlohmann::json push_msg = {
        {"type", std::string(kTypeDeviceStatusPush)},
        {"role", std::string(kRoleServer)},
        {"from", std::string(kRoleServer)},
        {"ts", std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count()},
        {"code", 0},
        {"msg", "ok"},
        {"data", {{"deviceId", device_id}, {"status", "online"}, {"ipAddress", ip_address}}}};
    subscriptions_.publish("device", push_msg.dump());
  }

  if (router_) {
    service::push_pending_config_for_device(device_id, db_, *router_);
  }
}

void GatewaySessionHandler::handleAdminAuth(net::ISession& session, const ParsedEnvelope& env) {
  const std::string username = opt_data_string(env.data, "username");
  const std::string password = opt_data_string(env.data, "password");
  const std::string route_to = env.from.empty() ? username : env.from;

  INFO_AUTH("管理端认证请求: username=" << username
            << " ip=" << remote_ip_string(session));

  if (username.empty() || password.empty()) {
    WARN_AUTH("管理端认证失败: missing credentials");
    session.write_line(build_error(kCodeAuthFailed, "missing username or password", env.msg_id, route_to));
    return;
  }

  // 使用 TokenManager 进行认证
  auto result = tokenManager_->authenticate_admin(username, password);

  if (!result.success) {
    WARN_AUTH("管理端认证失败: username=" << username
              << " error=" << result.error_message);
    session.write_line(build_error(kCodeAuthFailed, result.error_message, env.msg_id, route_to));

    // 记录失败日志
    const std::string login_ip = remote_ip_string(session);
    db_.dispatch(
        [username, login_ip](mysqlx::Session& s) {
          db::OperationLogEntry log;
          log.action = "auth.login";
          log.target_type = "UserAccount";
          log.target_id = username;
          log.ip_address = login_ip;
          log.result = "denied";
          db::OperationLogRepository::insertLog(s, log);
        },
        [](std::exception_ptr) {});
    return;
  }

  // 更新连接上下文
  std::string reply;
  {
    std::lock_guard<std::mutex> lk(mu_);
    ConnContext& ctx = connections_[&session];
    ctx.endpoint_role = EndpointRole::Admin;
    ctx.authenticated = true;
    ctx.bound_from = route_to;
    ctx.user_id = result.user_id;
    ctx.employee_id = result.employee_id;
    ctx.username = username;
    ctx.roles = std::move(result.roles);
    ctx.permissions = std::unordered_set<std::string>(result.permissions.begin(), result.permissions.end());
    ctx.heartbeat_sec = cfg_.heartbeat_sec;

    // 存储令牌信息
    ctx.access_token = result.tokens.access_token;
    ctx.refresh_token = result.tokens.refresh_token;
    ctx.token_expires_at = util::JwtUtil::current_timestamp() + result.tokens.expires_in;

    arm_heartbeat(session, ctx);

    // 构建包含令牌的响应
    nlohmann::json data;
    data["accessToken"] = result.tokens.access_token;
    data["refreshToken"] = result.tokens.refresh_token;
    data["tokenType"] = "Bearer";
    data["expiresIn"] = result.tokens.expires_in;
    data["heartbeatSec"] = cfg_.heartbeat_sec;
    data["serverTime"] = util::JwtUtil::current_timestamp() * 1000;
    data["roles"] = ctx.roles;
    data["permissions"] = result.permissions;

    reply = build_auth_response(env.msg_id, route_to, "admin-db-" + route_to,
                                 cfg_.heartbeat_sec, data);
  }

  admin_registry_.register_session(route_to, session.shared_from_this());
  session.write_line(std::move(reply));
  INFO_AUTH("管理端认证成功: username=" << username << " userId=" << result.user_id
            << " employeeId=" << result.employee_id << " roles=" << result.roles.size());

  // 记录成功日志
  const std::string login_ip = remote_ip_string(session);
  const int logged_uid = result.user_id;
  const std::string logged_eid = result.employee_id;
  db_.dispatch(
      [logged_uid, logged_eid, login_ip](mysqlx::Session& s) {
        db::OperationLogEntry log;
        log.user_id = logged_uid;
        log.employee_id = logged_eid;
        log.action = "auth.login";
        log.target_type = "UserAccount";
        log.target_id = logged_eid;
        log.ip_address = login_ip;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [](std::exception_ptr) {});
}

void GatewaySessionHandler::handleTokenRefresh(net::ISession& session, const ParsedEnvelope& env) {
  const std::string refresh_token = opt_data_string(env.data, "refreshToken");
  const std::string route_to = env.from;

  DBG_AUTH("令牌刷新请求: from=" << route_to);

  if (refresh_token.empty()) {
    WARN_AUTH("令牌刷新失败: missing refresh token");
    session.write_line(build_error(kCodeRefreshTokenInvalid, "missing refresh token", env.msg_id, route_to));
    return;
  }

  // 刷新令牌
  auto result = tokenManager_->refresh_tokens(refresh_token);

  if (!result) {
    WARN_AUTH("令牌刷新失败: invalid or expired token from=" << route_to);
    session.write_line(build_error(kCodeRefreshTokenInvalid, "invalid or expired refresh token", env.msg_id, route_to));
    return;
  }

  DBG_AUTH("令牌刷新成功: from=" << route_to);

  // 更新连接上下文
  {
    std::lock_guard<std::mutex> lk(mu_);
    ConnContext& ctx = connections_[&session];
    ctx.access_token = result->access_token;
    ctx.refresh_token = result->refresh_token;
    ctx.token_expires_at = util::JwtUtil::current_timestamp() + result->expires_in;
  }

  // 构建响应
  nlohmann::json data;
  data["accessToken"] = result->access_token;
  data["refreshToken"] = result->refresh_token;
  data["tokenType"] = "Bearer";
  data["expiresIn"] = result->expires_in;

  nlohmann::json response;
  response["type"] = "token.refresh.response";
  response["role"] = "server";
  response["from"] = "server";
  response["to"] = route_to;
  response["inReplyTo"] = env.msg_id;
  response["ts"] = util::JwtUtil::current_timestamp() * 1000;
  response["code"] = 0;
  response["msg"] = "ok";
  response["data"] = data;

  session.write_line(response.dump());
}

bool GatewaySessionHandler::verifyTokenInEnvelope(const ParsedEnvelope& env, ConnContext& ctx) {
  // 从信封中获取令牌
  std::string token;

  // 检查 accessToken 字段（管理端发送的格式）
  if (env.data.contains("accessToken") && env.data["accessToken"].is_string()) {
    token = env.data["accessToken"].get<std::string>();
  }

  // 检查 token 字段
  if (token.empty() && env.data.contains("token") && env.data["token"].is_string()) {
    token = env.data["token"].get<std::string>();
  }

  // 如果没有令牌，检查是否在 headers 中
  if (token.empty() && env.data.contains("headers") && env.data["headers"].is_object()) {
    auto headers = env.data["headers"];
    if (headers.contains("Authorization") && headers["Authorization"].is_string()) {
      std::string auth = headers["Authorization"].get<std::string>();
      if (auth.substr(0, 7) == "Bearer ") {
        token = auth.substr(7);
      }
    }
  }

  // 如果仍然没有令牌，使用现有的 accessToken
  if (token.empty()) {
    token = ctx.access_token;
  }

  // 如果还是没有令牌，返回失败
  if (token.empty()) {
    return false;
  }

  // 验证令牌
  auto payload = tokenManager_->verify_access_token(token);

  if (!payload) {
    return false;
  }

  // 更新上下文中的权限信息
  if (!payload->permissions.empty()) {
    ctx.permissions = std::unordered_set<std::string>(payload->permissions.begin(), payload->permissions.end());
  }

  return true;
}

}  // namespace protocol
