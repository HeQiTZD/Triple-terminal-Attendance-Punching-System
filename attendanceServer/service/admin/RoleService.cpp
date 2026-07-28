#include "service/admin/RoleService.hpp"

#include <memory>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/OperationLogRepository.hpp"
#include "db/RoleRepository.hpp"
#include "net/Session.hpp"
#include "protocol/AppError.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
// AuditContext is defined in AdminCrudService.hpp; include it for the full
// struct definition (only forward-declared in RoleService.hpp).
#include "service/AdminCrudService.hpp"
#include "service/IDbExecutor.hpp"

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

// ── Role CRUD ──

void admin_role_create(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit) {
  const std::string role_key = opt_str(env.data, "roleKey");
  const std::string role_name = opt_str(env.data, "roleName");
  if (role_key.empty() || role_name.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required fields: roleKey, roleName", env.msg_id, reply_to));
    return;
  }
  db::RoleCreateInput input;
  input.role_key = role_key;
  input.role_name = role_name;
  input.description = opt_str(env.data, "description");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [input, audit](mysqlx::Session& s) {
        db::RoleRepository::insertRole(s, input);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "role.create";
        log.target_type = "Role";
        log.target_id = input.role_key;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeRoleCreateResponse, msg_id, to, 0, "ok", nullptr));
      });
}

void admin_role_query(const protocol::ParsedEnvelope& env,
                      const std::string& reply_to, IDbExecutor& db,
                      const std::shared_ptr<net::ISession>& session) {
  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  auto results = std::make_shared<std::vector<db::RoleRecord>>();
  db.dispatch(
      [results](mysqlx::Session& s) {
        *results = db::RoleRepository::selectRoles(s);
      },
      [session, msg_id, to, results](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        nlohmann::json data;
        data["records"] = nlohmann::json::array();
        for (const auto& r : *results) {
          data["records"].push_back({
              {"id", r.id},
              {"roleKey", r.role_key},
              {"roleName", r.role_name},
              {"description", r.description},
              {"isSystem", r.is_system},
              {"createdAt", r.created_at},
              {"updatedAt", r.updated_at},
          });
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeRoleQueryResponse, msg_id, to, 0, "ok", &data));
      });
}

void admin_role_update(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit) {
  const std::string role_key = opt_str(env.data, "roleKey");
  if (role_key.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: roleKey",
        env.msg_id, reply_to));
    return;
  }
  db::RoleUpdateInput input;
  input.role_name = opt_str(env.data, "roleName");
  input.description = opt_str(env.data, "description");

  std::vector<std::string> perm_keys;
  bool update_perms = false;
  if (env.data.contains("permissions") && env.data["permissions"].is_array()) {
    update_perms = true;
    for (const auto& el : env.data["permissions"]) {
      if (el.is_string()) perm_keys.push_back(el.get<std::string>());
    }
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [role_key, input, update_perms, perm_keys, audit](mysqlx::Session& s) {
        nlohmann::json detail;
        if (update_perms) {
          auto old_perms = db::RoleRepository::getRolePermissionKeys(s, role_key);
          detail["permissionsBefore"] = old_perms;
          detail["permissionsAfter"] = perm_keys;
        }
        if (!input.role_name.empty() || !input.description.empty()) {
          db::RoleRepository::updateRole(s, role_key, input);
        }
        if (update_perms) {
          db::RoleRepository::setRolePermissions(s, role_key, perm_keys);
        }
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "role.update";
        log.target_type = "Role";
        log.target_id = role_key;
        log.detail = detail.empty() ? "" : detail.dump();
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeRoleUpdateResponse, msg_id, to, 0, "ok", nullptr));
      });
}

void admin_role_delete(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit) {
  const std::string role_key = opt_str(env.data, "roleKey");
  if (role_key.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: roleKey",
        env.msg_id, reply_to));
    return;
  }
  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [role_key, audit](mysqlx::Session& s) {
        db::RoleRepository::deleteRole(s, role_key);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "role.delete";
        log.target_type = "Role";
        log.target_id = role_key;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeRoleDeleteResponse, msg_id, to, 0, "ok", nullptr));
      });
}

// ── Permission queries ──

void admin_permission_query(const protocol::ParsedEnvelope& env,
                            const std::string& reply_to, IDbExecutor& db,
                            const std::shared_ptr<net::ISession>& session) {
  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  auto results = std::make_shared<std::vector<db::PermissionRecord>>();
  db.dispatch(
      [results](mysqlx::Session& s) {
        *results = db::RoleRepository::queryAllPermissions(s);
      },
      [session, msg_id, to, results](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        nlohmann::json data;
        data["records"] = nlohmann::json::array();
        for (const auto& r : *results) {
          data["records"].push_back({
              {"id", r.id},
              {"permKey", r.perm_key},
              {"permName", r.perm_name},
              {"resource", r.resource},
              {"description", r.description},
          });
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypePermissionQueryResponse, msg_id, to, 0, "ok",
            &data));
      });
}

void admin_permission_self(
    const protocol::ParsedEnvelope& env, const std::string& reply_to,
    const std::unordered_set<std::string>& caller_permissions,
    const std::shared_ptr<net::ISession>& session) {
  nlohmann::json data;
  data["permissions"] = nlohmann::json::array();
  for (const auto& p : caller_permissions) {
    data["permissions"].push_back(p);
  }
  session->write_line(protocol::build_report_ack(
      protocol::kTypePermissionSelfResponse, env.msg_id, reply_to, 0, "ok",
      &data));
}

}  // namespace service
