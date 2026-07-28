#include "service/DeviceService.hpp"

#include <mysqlx/xdevapi.h>

#include "db/DeviceRepository.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "net/Session.hpp"
#include "service/DbExecutor.hpp"

namespace service {

namespace {

std::string json_string_any_of(const nlohmann::json& data,
                               std::initializer_list<const char*> keys) {
  for (const char* k : keys) {
    if (data.contains(k) && data[k].is_string()) {
      const auto s = data[k].get<std::string>();
      if (!s.empty()) {
        return s;
      }
    }
  }
  return {};
}

}  // namespace

void device_handle_status_report(const protocol::ParsedEnvelope& env,
                                 protocol::ConnContext& ctx,
                                 const std::string& reply_to,
                                 DbExecutor& db,
                                 const std::shared_ptr<net::ISession>& session) {
  if (ctx.endpoint_role != protocol::EndpointRole::Device) {
    session->write_line(protocol::build_error(
        protocol::kCodeForbidden, "device role required", env.msg_id, reply_to));
    return;
  }

  const std::string device_id = ctx.device_id;
  if (device_id.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "missing bound device_id",
        env.msg_id, reply_to));
    return;
  }

  db::DeviceUpdateInput upd;
  upd.device_name =
      json_string_any_of(env.data, {"device_name", "deviceName"});
  upd.ip_address = json_string_any_of(env.data, {"ip_address", "ipAddress"});
  upd.last_online_now = true;
  // 不显式更新 status：在线语义由 upsertDeviceOnline / markDeviceOffline 维护。

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  const std::string dev = device_id;

  db.dispatch(
      [dev, upd](mysqlx::Session& s) {
        // last_online_now 单独亦为有效更新项。
        db::DeviceRepository::updateDevice(s, dev, upd);
      },
      [session, msg_id, to](std::exception_ptr ep) {
        if (!session) {
          return;
        }
        if (ep) {
          try {
            std::rethrow_exception(ep);
          } catch (const mysqlx::Error& e) {
            const protocol::MappedError me = protocol::map_mysqlx_error(e);
            session->write_line(protocol::build_error(me.code, me.msg, msg_id,
                                                      to));
          } catch (const std::exception& ex) {
            session->write_line(protocol::build_error(
                protocol::kCodeBusinessValidation, ex.what(), msg_id, to));
          }
          return;
        }
        session->write_line(protocol::build_report_ack(
            protocol::kTypeDeviceStatusReportResponse, msg_id, to, 0, "ok",
            nullptr));
      });
}

}  // namespace service
