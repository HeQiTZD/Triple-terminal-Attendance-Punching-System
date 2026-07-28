#include "service/admin/PersonService.hpp"

#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/OperationLogRepository.hpp"
#include "db/PersonRepository.hpp"
#include "net/Session.hpp"
#include "protocol/AppError.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
// AuditContext is defined in AdminCrudService.hpp; include it for the full
// struct definition (only forward-declared in PersonService.hpp).
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

// ---------------------------------------------------------------------------
// Person CRUD
// ---------------------------------------------------------------------------

void admin_person_create(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit) {
  const std::string name = opt_str(env.data, "name");
  const std::string employee_id = opt_str(env.data, "employeeId");
  const std::string department = opt_str(env.data, "department");
  const std::string position = opt_str(env.data, "position");

  if (name.empty() || employee_id.empty() || department.empty() ||
      position.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required fields: name, employeeId, department, position",
        env.msg_id, reply_to));
    return;
  }

  db::PersonCreateInput input;
  input.name = name;
  input.employee_id = employee_id;
  input.department = department;
  input.position = position;

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [input, audit](mysqlx::Session& s) {
        db::PersonRepository::insert(s, input);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "person.create";
        log.target_type = "Person";
        log.target_id = input.employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypePersonCreateResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

void admin_person_query(const protocol::ParsedEnvelope& env,
                        const std::string& reply_to, IDbExecutor& db,
                        const std::shared_ptr<net::ISession>& session) {
  db::PersonQueryInput input;
  input.name = opt_str(env.data, "name");
  input.employee_id = opt_str(env.data, "employeeId");
  input.department = opt_str(env.data, "department");
  input.position = opt_str(env.data, "position");
  input.created_at = opt_str(env.data, "createdAt");
  input.updated_at = opt_str(env.data, "updatedAt");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;

  auto results =
      std::make_shared<std::vector<db::PersonRecord>>();

  db.dispatch(
      [input, results](mysqlx::Session& s) {
        *results = db::PersonRepository::selectPerson(s, input);
      },
      [session, msg_id, to, results](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        nlohmann::json data;
        data["records"] = nlohmann::json::array();
        for (const auto& r : *results) {
          data["records"].push_back({
              {"id", r.id},
              {"name", r.name},
              {"employeeId", r.employee_id},
              {"department", r.department},
              {"position", r.position},
              {"createdAt", r.created_at},
              {"updatedAt", r.updated_at},
          });
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypePersonQueryResponse, msg_id, to, 0, "ok", &data));
      });
}

void admin_person_update(const protocol::ParsedEnvelope& env,
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

  db::PersonUpdateInput input;
  input.name = opt_str(env.data, "name");
  input.department = opt_str(env.data, "department");
  input.position = opt_str(env.data, "position");

  if (input.name.empty() && input.department.empty() &&
      input.position.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "at least one update field required", env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [employee_id, input, audit](mysqlx::Session& s) {
        db::PersonRepository::updatePerson(s, employee_id, input);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "person.update";
        log.target_type = "Person";
        log.target_id = employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypePersonUpdateResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

void admin_person_delete(const protocol::ParsedEnvelope& env,
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
        db::deletePerson(s, employee_id);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "person.delete";
        log.target_type = "Person";
        log.target_id = employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypePersonDeleteResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

}  // namespace service
