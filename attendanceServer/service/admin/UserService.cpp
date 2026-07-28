#include "service/admin/UserService.hpp"

#include <memory>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/OperationLogRepository.hpp"
#include "db/RoleRepository.hpp"
#include "db/UserAccountRepository.hpp"
#include "net/Session.hpp"
#include "protocol/AppError.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
// AuditContext is defined in AdminCrudService.hpp; include it for the full
// struct definition (only forward-declared in UserService.hpp).
#include "service/AdminCrudService.hpp"
#include "service/IDbExecutor.hpp"
#include "util/PasswordHash.hpp"

namespace service {

namespace {

std::string opt_str(const nlohmann::json& data, const char* key) {
  if (!data.contains(key) || !data[key].is_string()) {
    return {};
  }
  return data[key].get<std::string>();
}

void send_db_error(const std::shared_ptr<net::ISession>& session,
                   std::exception_ptr ep, const std::string& msg_id,
                   const std::string& to) {
  try {
    std::rethrow_exception(ep);
  } catch (const mysqlx::Error& e) {
    const protocol::MappedError me = protocol::map_mysqlx_error(e);
    session->write_line(protocol::build_error(me.code, me.msg, msg_id, to));
  } catch (const protocol::AppError& e) {
    session->write_line(protocol::build_error(
        e.numeric_code(), e.what(), msg_id, to));
  } catch (const std::invalid_argument& e) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, e.what(), msg_id, to));
  } catch (const std::exception& ex) {
    session->write_line(protocol::build_error(protocol::kCodeDbError,
                                              ex.what(), msg_id, to));
  }
}

}  // namespace

// ---------------------------------------------------------------------------
// UserAccount CRUD
// ---------------------------------------------------------------------------

void admin_user_create(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit) {
  const std::string employee_id = opt_str(env.data, "employeeId");
  const std::string password = opt_str(env.data, "password");

  if (employee_id.empty() || password.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required fields: employeeId, password", env.msg_id,
        reply_to));
    return;
  }

  const std::string pw_err = util::validate_password_strength(password);
  if (!pw_err.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, pw_err, env.msg_id, reply_to));
    return;
  }

  db::UserAccountCreateInput input;
  input.employee_id = employee_id;
  input.password = util::hash_password(password);
  input.name = opt_str(env.data, "name");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [input, audit](mysqlx::Session& s) {
        db::UserAccountRepository::insertUser(s, input);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "user.create";
        log.target_type = "UserAccount";
        log.target_id = input.employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeUserCreateResponse, msg_id, to, 0, "ok", nullptr));
      });
}

void admin_user_query(const protocol::ParsedEnvelope& env,
                      const std::string& reply_to, IDbExecutor& db,
                      const std::shared_ptr<net::ISession>& session) {
  db::UserAccountQueryInput input;
  input.employee_id = opt_str(env.data, "employeeId");
  input.created_at = opt_str(env.data, "createdAt");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;

  auto results =
      std::make_shared<std::vector<db::UserAccountRecord>>();

  db.dispatch(
      [input, results](mysqlx::Session& s) {
        *results = db::UserAccountRepository::selectUser(s, input);
      },
      [session, msg_id, to, results](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        nlohmann::json data;
        data["records"] = nlohmann::json::array();
        for (const auto& r : *results) {
          data["records"].push_back({
              {"id", r.id},
              {"employeeId", r.employee_id},
              {"name", r.name},
              {"createdAt", r.created_at},
              {"lastLoginTime", r.last_login_time},
          });
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeUserQueryResponse, msg_id, to, 0, "ok", &data));
      });
}

void admin_user_update(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit) {
  const std::string employee_id = opt_str(env.data, "employeeId");
  if (employee_id.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: employeeId",
        env.msg_id, reply_to));
    return;
  }

  db::UserAccountUpdateInput input;
  input.name = opt_str(env.data, "name");
  const std::string raw_pw = opt_str(env.data, "password");
  if (!raw_pw.empty()) {
    const std::string pw_err = util::validate_password_strength(raw_pw);
    if (!pw_err.empty()) {
      session->write_line(protocol::build_error(
          protocol::kCodeBusinessValidation, pw_err, env.msg_id, reply_to));
      return;
    }
    input.password = util::hash_password(raw_pw);
  }

  if (input.name.empty() && input.password.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "at least one update field required", env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [employee_id, input, audit](mysqlx::Session& s) {
        db::UserAccountRepository::updateUser(s, employee_id, input);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "user.update";
        log.target_type = "UserAccount";
        log.target_id = employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeUserUpdateResponse, msg_id, to, 0, "ok", nullptr));
      });
}

void admin_user_delete(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit) {
  const std::string employee_id = opt_str(env.data, "employeeId");
  if (employee_id.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: employeeId",
        env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [employee_id, audit](mysqlx::Session& s) {
        s.sql("USE attendanceserver").execute();
        auto res = s.sql("SELECT ua.id FROM UserAccount ua WHERE ua.employee_id = ?")
                       .bind(employee_id)
                       .execute();
        auto row = res.fetchOne();
        if (row) {
          int uid = static_cast<int>(row[0].get<int64_t>());
          auto roles = db::RoleRepository::loadUserRoles(s, uid);
          for (const auto& r : roles) {
            if (r == "super_admin") {
              int count =
                  db::RoleRepository::countUsersWithRole(s, "super_admin");
              if (count <= 1) {
                throw std::runtime_error(
                    "cannot delete: last super_admin user");
              }
              break;
            }
          }
        }
        db::UserAccountRepository::deleteUser(s, employee_id);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "user.delete";
        log.target_type = "UserAccount";
        log.target_id = employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeUserDeleteResponse, msg_id, to, 0, "ok", nullptr));
      });
}

// ---------------------------------------------------------------------------
// User-Role management
// ---------------------------------------------------------------------------

void admin_user_role_assign(const protocol::ParsedEnvelope& env,
                            const std::string& reply_to, IDbExecutor& db,
                            const std::shared_ptr<net::ISession>& session,
                            const AuditContext& audit,
                            PermissionReloadCallback on_role_changed) {
  const int user_id = env.data.value("userId", 0);
  const std::string role_key = opt_str(env.data, "roleKey");
  if (user_id <= 0 || role_key.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required fields: userId, roleKey", env.msg_id, reply_to));
    return;
  }
  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [user_id, role_key, audit](mysqlx::Session& s) {
        db::RoleRepository::assignUserRole(s, user_id, role_key);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "user.role.assign";
        log.target_type = "UserRole";
        log.target_id = std::to_string(user_id);
        log.detail = "{\"roleKey\":\"" + role_key + "\"}";
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to, user_id, on_role_changed](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeUserRoleAssignResponse, msg_id, to, 0, "ok",
            nullptr));
        if (on_role_changed) on_role_changed(user_id);
      });
}

void admin_user_role_revoke(const protocol::ParsedEnvelope& env,
                            const std::string& reply_to, IDbExecutor& db,
                            const std::shared_ptr<net::ISession>& session,
                            int caller_user_id,
                            const AuditContext& audit,
                            PermissionReloadCallback on_role_changed) {
  const int user_id = env.data.value("userId", 0);
  const std::string role_key = opt_str(env.data, "roleKey");
  if (user_id <= 0 || role_key.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required fields: userId, roleKey", env.msg_id, reply_to));
    return;
  }
  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [user_id, role_key, caller_user_id, audit](mysqlx::Session& s) {
        if (role_key == "super_admin") {
          if (user_id == caller_user_id) {
            throw std::runtime_error(
                "cannot revoke own super_admin role");
          }
          int count = db::RoleRepository::countUsersWithRole(s, "super_admin");
          if (count <= 1) {
            throw std::runtime_error(
                "cannot revoke: at least one super_admin must remain");
          }
        }
        db::RoleRepository::revokeUserRole(s, user_id, role_key);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "user.role.revoke";
        log.target_type = "UserRole";
        log.target_id = std::to_string(user_id);
        log.detail = "{\"roleKey\":\"" + role_key + "\"}";
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to, user_id, on_role_changed](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeUserRoleRevokeResponse, msg_id, to, 0, "ok",
            nullptr));
        if (on_role_changed) on_role_changed(user_id);
      });
}

void admin_user_role_query(const protocol::ParsedEnvelope& env,
                           const std::string& reply_to, IDbExecutor& db,
                           const std::shared_ptr<net::ISession>& session) {
  const int user_id = env.data.value("userId", 0);
  if (user_id <= 0) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: userId",
        env.msg_id, reply_to));
    return;
  }
  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  auto roles = std::make_shared<std::vector<std::string>>();
  db.dispatch(
      [user_id, roles](mysqlx::Session& s) {
        *roles = db::RoleRepository::queryUserRoles(s, user_id);
      },
      [session, msg_id, to, roles](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        nlohmann::json data;
        data["roles"] = *roles;
        session->write_line(protocol::build_report_ack(
            protocol::kTypeUserRoleQueryResponse, msg_id, to, 0, "ok", &data));
      });
}

}  // namespace service
