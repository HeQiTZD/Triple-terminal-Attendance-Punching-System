#include "service/admin/DeviceCrudService.hpp"

#include <memory>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/DeviceRepository.hpp"
#include "db/OperationLogRepository.hpp"
#include "net/Session.hpp"
#include "protocol/AppError.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
// AuditContext is defined in AdminCrudService.hpp; include it for the full
// struct definition (only forward-declared in DeviceCrudService.hpp).
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
// Device CRUD
// ---------------------------------------------------------------------------

void admin_device_create(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit) {
  const std::string device_id = opt_str(env.data, "deviceId");
  const std::string device_name = opt_str(env.data, "deviceName");
  const std::string device_key = opt_str(env.data, "deviceKey");

  if (device_id.empty() || device_name.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required fields: deviceId, deviceName", env.msg_id,
        reply_to));
    return;
  }

  if (device_key.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing required field: deviceKey", env.msg_id, reply_to));
    return;
  }

  db::DeviceCreateInput input;
  input.device_id = device_id;
  input.device_name = device_name;
  input.ip_address = opt_str(env.data, "ipAddress");
  input.last_online = opt_str(env.data, "lastOnline");
  input.status = opt_str(env.data, "status");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [input, device_key, audit](mysqlx::Session& s) {
        db::DeviceRepository::insertDevice(s, input);
        // 存储设备密钥（明文）
        s.sql("UPDATE Device SET key_hash = ? WHERE device_id = ?")
            .bind(device_key, input.device_id)
            .execute();
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "device.create";
        log.target_type = "Device";
        log.target_id = input.device_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeDeviceCreateResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

void admin_device_query(const protocol::ParsedEnvelope& env,
                        const std::string& reply_to, IDbExecutor& db,
                        const std::shared_ptr<net::ISession>& session) {
  db::DeviceQueryInput input;
  input.device_id = opt_str(env.data, "deviceId");
  input.device_name = opt_str(env.data, "deviceName");
  input.ip_address = opt_str(env.data, "ipAddress");
  input.last_online = opt_str(env.data, "lastOnline");

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;

  auto results = std::make_shared<std::vector<db::DeviceRecord>>();

  db.dispatch(
      [input, results](mysqlx::Session& s) {
        *results = db::DeviceRepository::selectDevice(s, input);
      },
      [session, msg_id, to, results](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        nlohmann::json data;
        data["records"] = nlohmann::json::array();
        for (const auto& r : *results) {
          data["records"].push_back({
              {"id", r.id},
              {"deviceId", r.device_id},
              {"deviceName", r.device_name},
              {"ipAddress", r.ip_address},
              {"lastOnline", r.last_online},
              {"status", r.status},
          });
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeDeviceQueryResponse, msg_id, to, 0, "ok", &data));
      });
}

void admin_device_update(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit) {
  const std::string device_id = opt_str(env.data, "deviceId");
  if (device_id.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: deviceId",
        env.msg_id, reply_to));
    return;
  }

  db::DeviceUpdateInput input;
  input.device_name = opt_str(env.data, "deviceName");
  input.ip_address = opt_str(env.data, "ipAddress");
  input.last_online = opt_str(env.data, "lastOnline");
  input.status = opt_str(env.data, "status");

  if (input.device_name.empty() && input.ip_address.empty() &&
      input.last_online.empty() && input.status.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "at least one update field required", env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [device_id, input, audit](mysqlx::Session& s) {
        db::DeviceRepository::updateDevice(s, device_id, input);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "device.update";
        log.target_type = "Device";
        log.target_id = device_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeDeviceUpdateResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

void admin_device_delete(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit) {
  const std::string device_id = opt_str(env.data, "deviceId");
  if (device_id.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing required field: deviceId",
        env.msg_id, reply_to));
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  db.dispatch(
      [device_id, audit](mysqlx::Session& s) {
        db::DeviceRepository::deleteDevice(s, device_id);
        db::OperationLogEntry log;
        log.user_id = audit.user_id;
        log.employee_id = audit.employee_id;
        log.action = "device.delete";
        log.target_type = "Device";
        log.target_id = device_id;
        log.ip_address = audit.ip_address;
        log.result = "success";
        db::OperationLogRepository::insertLog(s, log);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) return;
        if (ep) { send_db_error(session, ep, msg_id, to); return; }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeDeviceDeleteResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

}  // namespace service
