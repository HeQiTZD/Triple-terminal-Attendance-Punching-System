#include "service/admin/AttendanceCrudService.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/AttendanceRecordRepository.hpp"
#include "db/OperationLogRepository.hpp"
#include "net/Session.hpp"
#include "protocol/AppError.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
// AuditContext is defined in AdminCrudService.hpp; include it for the full
// struct definition (only forward-declared in AttendanceCrudService.hpp).
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

std::optional<int> opt_int(const nlohmann::json& data, const char* key) {
  if (!data.contains(key)) {
    return std::nullopt;
  }
  const auto& v = data[key];
  if (v.is_number_integer()) {
    return v.get<int>();
  }
  return std::nullopt;
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
// AttendanceRecord CRUD
// ---------------------------------------------------------------------------

void admin_attendance_create(const protocol::ParsedEnvelope& env,
                             const std::string& reply_to, IDbExecutor& db,
                             const std::shared_ptr<net::ISession>& session,
                             const AuditContext& audit) {
  const std::string employee_id = opt_str(env.data, "employeeId");
  const std::string check_time = opt_str(env.data, "checkTime");
  const std::string status = opt_str(env.data, "status");

  if (employee_id.empty() || check_time.empty() || status.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required fields: employeeId, checkTime, status", env.msg_id,
        reply_to));
    return;
  }

  db::AttendanceRecordCreateInput input;
  input.employee_id = employee_id;
  input.check_time = check_time;
  input.status = status;
  input.device_id = opt_str(env.data, "deviceId");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [input, audit](mysqlx::Session& s) {
        db::AttendanceRecordRepository::insertRecord(s, input);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "attendance.create";
        log.target_type = "AttendanceRecord";
        log.target_id = input.employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeAttendanceCreateResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

void admin_attendance_query(const protocol::ParsedEnvelope& env,
                            const std::string& reply_to, IDbExecutor& db,
                            const std::shared_ptr<net::ISession>& session) {
  db::AttendanceRecordQueryInput input;
  input.id = opt_int(env.data, "id");
  input.employee_id = opt_str(env.data, "employeeId");
  input.check_time = opt_str(env.data, "checkTime");
  input.device_id = opt_str(env.data, "deviceId");
  input.status = opt_str(env.data, "status");
  input.received_time = opt_str(env.data, "receivedTime");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;

  auto results =
      std::make_shared<std::vector<db::AttendanceRecordRecord>>();

  db.dispatch(
      [input, results](mysqlx::Session& s) {
        *results = db::AttendanceRecordRepository::selectRecord(s, input);
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
              {"checkTime", r.check_time},
              {"deviceId", r.device_id},
              {"status", r.status},
              {"receivedTime", r.received_time},
          });
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeAttendanceQueryResponse, msg_id, to, 0, "ok",
            &data));
      });
}

void admin_attendance_update(const protocol::ParsedEnvelope& env,
                             const std::string& reply_to, IDbExecutor& db,
                             const std::shared_ptr<net::ISession>& session,
                             const AuditContext& audit) {
  const auto& d = env.data;
  if (!d.contains("locate") || !d["locate"].is_object() ||
      !d.contains("updates") || !d["updates"].is_object()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required fields: locate, updates", env.msg_id, reply_to));
    return;
  }

  const auto& loc = d["locate"];
  const auto& upd = d["updates"];

  db::AttendanceRecordLocateInput locate;
  locate.id = opt_int(loc, "id");
  locate.employee_id = opt_str(loc, "employeeId");
  locate.check_time = opt_str(loc, "checkTime");
  locate.device_id = opt_str(loc, "deviceId");
  locate.status = opt_str(loc, "status");
  locate.received_time = opt_str(loc, "receivedTime");

  bool has_locate = locate.id.has_value() || !locate.employee_id.empty() ||
                    !locate.check_time.empty() || !locate.device_id.empty() ||
                    !locate.status.empty() || !locate.received_time.empty();
  if (!has_locate) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "locate must have at least one condition", env.msg_id, reply_to));
    return;
  }

  db::AttendanceRecordUpdateInput updates;
  updates.check_time = opt_str(upd, "checkTime");
  updates.device_id = opt_str(upd, "deviceId");
  updates.status = opt_str(upd, "status");
  updates.received_time = opt_str(upd, "receivedTime");

  if (updates.check_time.empty() && updates.device_id.empty() &&
      updates.status.empty() && updates.received_time.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "updates must have at least one field", env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  std::string target_id = locate.employee_id;
  if (target_id.empty() && locate.id.has_value()) {
    target_id = std::to_string(*locate.id);
  }
  db.dispatch(
      [locate, updates, audit, target_id](mysqlx::Session& s) {
        db::AttendanceRecordRepository::updateRecord(s, locate, updates);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "attendance.update";
        log.target_type = "AttendanceRecord";
        log.target_id = target_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeAttendanceUpdateResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

void admin_attendance_delete(const protocol::ParsedEnvelope& env,
                             const std::string& reply_to, IDbExecutor& db,
                             const std::shared_ptr<net::ISession>& session,
                             const AuditContext& audit) {
  db::AttendanceRecordDeleteInput input;
  input.id = opt_int(env.data, "id");
  input.employee_id = opt_str(env.data, "employeeId");
  input.check_time = opt_str(env.data, "checkTime");
  input.device_id = opt_str(env.data, "deviceId");
  input.status = opt_str(env.data, "status");
  input.received_time = opt_str(env.data, "receivedTime");

  bool has_cond = input.id.has_value() || !input.employee_id.empty() ||
                  !input.check_time.empty() || !input.device_id.empty() ||
                  !input.status.empty() || !input.received_time.empty();
  if (!has_cond) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "at least one condition required to prevent full-table delete",
        env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  std::string target_id = input.employee_id;
  if (target_id.empty() && input.id.has_value()) {
    target_id = std::to_string(*input.id);
  }
  db.dispatch(
      [input, audit, target_id](mysqlx::Session& s) {
        db::AttendanceRecordRepository::deleteRecord(s, input);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "attendance.delete";
        log.target_type = "AttendanceRecord";
        log.target_id = target_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeAttendanceDeleteResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

void admin_attendance_archive_query(
    const protocol::ParsedEnvelope& env, const std::string& reply_to,
    IDbExecutor& db, const std::shared_ptr<net::ISession>& session) {
  db::AttendanceRecordArchiveQueryInput input;
  input.id = opt_int(env.data, "id");
  input.employee_id = opt_str(env.data, "employeeId");
  input.name = opt_str(env.data, "name");
  input.department = opt_str(env.data, "department");
  input.position = opt_str(env.data, "position");
  input.check_time = opt_str(env.data, "checkTime");
  input.device_id = opt_str(env.data, "deviceId");
  input.status = opt_str(env.data, "status");
  input.received_time = opt_str(env.data, "receivedTime");
  input.archived_at = opt_str(env.data, "archivedAt");
  input.archive_reason = opt_str(env.data, "archiveReason");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;

  auto results =
      std::make_shared<std::vector<db::AttendanceRecordArchiveRecord>>();

  db.dispatch(
      [input, results](mysqlx::Session& s) {
        *results = db::AttendanceRecordRepository::selectArchive(s, input);
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
              {"department", r.department},
              {"position", r.position},
              {"checkTime", r.check_time},
              {"deviceId", r.device_id},
              {"status", r.status},
              {"receivedTime", r.received_time},
              {"archivedAt", r.archived_at},
              {"archiveReason", r.archive_reason},
          });
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeAttendanceArchiveQueryResponse, msg_id, to, 0, "ok",
            &data));
      });
}

void admin_attendance_archive_delete(
    const protocol::ParsedEnvelope& env, const std::string& reply_to,
    IDbExecutor& db, const std::shared_ptr<net::ISession>& session,
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
        db::AttendanceRecordRepository::deleteArchive(s, employee_id);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "attendance.archive.delete";
        log.target_type = "AttendanceRecordArchive";
        log.target_id = employee_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeAttendanceArchiveDeleteResponse, msg_id, to, 0,
            "ok", nullptr));
      });
}

}  // namespace service
