#include "service/AttendanceService.hpp"

#include <mysqlx/xdevapi.h>

#include <nlohmann/json.hpp>

#include "db/AttendanceRecordRepository.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "net/Session.hpp"
#include "util/DebugLog.hpp"
#include <chrono>

#include "service/DbExecutor.hpp"
#include "service/EventSubscriptionRegistry.hpp"

namespace service {

namespace {

bool json_bool_default_false(const nlohmann::json& data, const char* key) {
  if (!data.contains(key)) {
    return false;
  }
  const auto& v = data[key];
  if (v.is_boolean()) {
    return v.get<bool>();
  }
  return false;
}

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

std::string attendance_status_from_data(const nlohmann::json& data) {
  std::string s =
      json_string_any_of(data, {"checkStatus", "attendanceStatus", "punchStatus"});
  if (!s.empty()) {
    return s;
  }
  return json_string_any_of(data, {"status"});
}

}  // namespace

void attendance_handle_report(const protocol::ParsedEnvelope& env,
                              protocol::ConnContext& ctx,
                              const std::string& reply_to,
                              DbExecutor& db,
                              const std::shared_ptr<net::ISession>& session,
                              EventSubscriptionRegistry* push_registry) {
  INFO_ATTENDANCE("考勤上报请求开始: msgId=" << env.msg_id);
  INFO_ATTENDANCE("请求详情: deviceId=" << ctx.device_id
                  << " employeeId=" << env.data.value("employeeId", env.data.value("employee_id", ""))
                  << " checkTime=" << env.data.value("checkTime", env.data.value("check_time", ""))
                  << " status=" << env.data.value("status", "")
                  << " awaitPhoto=" << (json_bool_default_false(env.data, "awaitPhoto") ? "true" : "false"));

  if (ctx.endpoint_role != protocol::EndpointRole::Device) {
    WARN_ATTENDANCE("考勤上报失败: 角色验证不通过"
                    << "\n  - 请求角色: " << (ctx.endpoint_role == protocol::EndpointRole::Admin ? "Admin" : "Unknown")
                    << "\n  - 要求角色: Device"
                    << "\n  - msgId: " << env.msg_id
                    << "\n  - deviceId: " << ctx.device_id);
    session->write_line(protocol::build_error(
        protocol::kCodeForbidden, "device role required", env.msg_id, reply_to));
    return;
  }
  DBG_ATTENDANCE("角色验证通过: role=Device");

  const bool await_photo = json_bool_default_false(env.data, "awaitPhoto");

  db::AttendanceRecordCreateInput input;
  input.employee_id =
      json_string_any_of(env.data, {"employee_id", "employeeId"});
  input.check_time =
      json_string_any_of(env.data, {"check_time", "checkTime"});
  input.status = attendance_status_from_data(env.data);

  input.device_id = ctx.device_id;
  const std::string data_device =
      json_string_any_of(env.data, {"deviceId", "device_id"});
  if (!data_device.empty() && !ctx.device_id.empty() &&
      data_device != ctx.device_id) {
    WARN_ATTENDANCE("考勤上报失败: deviceId不匹配"
                    << "\n  - 连接设备: " << ctx.device_id
                    << "\n  - 请求设备: " << data_device
                    << "\n  - msgId: " << env.msg_id
                    << "\n  - employeeId: " << input.employee_id);
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "deviceId mismatch", env.msg_id,
        reply_to));
    return;
  }
  DBG_ATTENDANCE("deviceId验证通过: ctx=" << ctx.device_id << " data=" << data_device);
  if (input.device_id.empty()) {
    input.device_id = data_device;
  }

  if (input.employee_id.empty() || input.check_time.empty() ||
      input.status.empty()) {
    WARN_ATTENDANCE("考勤上报失败: 缺少必填字段"
                    << "\n  - employeeId: " << (input.employee_id.empty() ? "✗ (空)" : "✓ (" + input.employee_id + ")")
                    << "\n  - checkTime: " << (input.check_time.empty() ? "✗ (空)" : "✓ (" + input.check_time + ")")
                    << "\n  - status: " << (input.status.empty() ? "✗ (空)" : "✓ (" + input.status + ")")
                    << "\n  - msgId: " << env.msg_id
                    << "\n  - deviceId: " << input.device_id);
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation,
        "missing employee_id, check_time, or attendance status", env.msg_id,
        reply_to));
    return;
  }
  DBG_ATTENDANCE("字段验证通过: all required fields present");

  if (await_photo) {
    INFO_ATTENDANCE("进入照片等待模式: employeeId=" << input.employee_id << " pending_msgId=" << env.msg_id);
    protocol::PendingAttendance pend;
    pend.msg_id = env.msg_id;
    pend.reply_to = reply_to;
    pend.row = std::move(input);
    ctx.pending_attendance = std::move(pend);
    return;
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;
  INFO_DB("数据库任务已派发: operation=insertAttendanceRecord employeeId=" << input.employee_id);
  db.dispatch(
      [input](mysqlx::Session& s) {
        db::AttendanceRecordRepository::insertRecord(s, input);
      },
      [session, msg_id, to, env, push_registry, input](std::exception_ptr ep) {
        if (!session) {
          return;
        }
        if (ep) {
          try {
            std::rethrow_exception(ep);
          } catch (const mysqlx::Error& e) {
            const protocol::MappedError me = protocol::map_mysqlx_error(e);
            ERR_DB("SQL执行失败: 数据库错误"
                   << "\n  - 错误类型: mysqlx::Error"
                   << "\n  - 错误码: " << me.code
                   << "\n  - 错误信息: " << me.msg
                   << "\n  - employeeId: " << input.employee_id
                   << "\n  - 原始错误: " << e.what());
            WARN_PROTO("考勤错误响应已发送: type=error code=" << me.code << " msg=\"" << me.msg << "\" msgId=" << msg_id << " to=" << to);
            session->write_line(protocol::build_error(me.code, me.msg, msg_id,
                                                        to));
          } catch (const std::exception& ex) {
            ERR_DB("SQL执行失败: 未知异常"
                   << "\n  - 异常类型: " << typeid(ex).name()
                   << "\n  - 异常信息: " << ex.what()
                   << "\n  - employeeId: " << input.employee_id);
            WARN_PROTO("考勤错误响应已发送: type=error code=4000 msg=\"" << ex.what() << "\" msgId=" << msg_id << " to=" << to);
            session->write_line(protocol::build_error(
                protocol::kCodeBusinessValidation, ex.what(), msg_id, to));
          }
          return;
        }
        INFO_DB("考勤记录保存成功: employeeId=" << input.employee_id << " hasPhoto=false");
        INFO_PROTO("考勤响应已发送: type=attendance.report.response code=0 msgId=" << msg_id << " to=" << to);
        session->write_line(protocol::build_report_ack(
            protocol::kTypeAttendanceReportResponse, msg_id, to, 0, "ok",
            nullptr));
        if (push_registry) {
          INFO_EVENT("考勤事件推送: topic=attendance msgId=push-" << msg_id);
          DBG_EVENT("推送详情: employeeId=" << input.employee_id
                    << " checkTime=" << input.check_time
                    << " status=" << input.status
                    << " hasPhoto=false");
          nlohmann::json root;
          root["type"] = std::string(protocol::kTypeAttendancePush);
          root["role"] = std::string(protocol::kRoleServer);
          root["from"] = std::string(protocol::kRoleServer);
          root["msgId"] = std::string("push-") + msg_id;
          root["ts"] = env.ts;
          root["data"] = env.data;
          push_registry->publish("attendance", root.dump());
        }
      });
}

void attendance_complete_with_photo(protocol::PendingAttendance pending,
                                    const std::vector<char>& photo_bytes,
                                    DbExecutor& db,
                                    const std::shared_ptr<net::ISession>& session,
                                    EventSubscriptionRegistry* push_registry) {
  pending.row.photo.assign(photo_bytes.begin(), photo_bytes.end());

  const std::string msg_id = pending.msg_id;
  const std::string to = pending.reply_to;
  db::AttendanceRecordCreateInput input = std::move(pending.row);

  INFO_ATTENDANCE("照片数据已合并: employeeId=" << input.employee_id << " photoSize=" << photo_bytes.size() << " bytes");

  nlohmann::json push_data = nlohmann::json::object();
  push_data["employee_id"] = input.employee_id;
  push_data["check_time"] = input.check_time;
  push_data["status"] = input.status;
  push_data["device_id"] = input.device_id;

  INFO_DB("数据库任务已派发: operation=insertAttendanceRecordWithPhoto employeeId=" << input.employee_id << " photoSize=" << input.photo.size());
  db.dispatch(
      [input](mysqlx::Session& s) {
        db::AttendanceRecordRepository::insertRecord(s, input);
      },
      [session, msg_id, to, push_data, push_registry, input](std::exception_ptr ep) {
        if (!session) {
          return;
        }
        if (ep) {
          try {
            std::rethrow_exception(ep);
          } catch (const mysqlx::Error& e) {
            const protocol::MappedError me = protocol::map_mysqlx_error(e);
            ERR_DB("SQL执行失败: 数据库错误"
                   << "\n  - 错误类型: mysqlx::Error"
                   << "\n  - 错误码: " << me.code
                   << "\n  - 错误信息: " << me.msg
                   << "\n  - employeeId: " << input.employee_id
                   << "\n  - 原始错误: " << e.what());
            WARN_PROTO("考勤错误响应已发送: type=error code=" << me.code << " msg=\"" << me.msg << "\" msgId=" << msg_id << " to=" << to);
            session->write_line(protocol::build_error(me.code, me.msg, msg_id,
                                                        to));
          } catch (const std::exception& ex) {
            ERR_DB("SQL执行失败: 未知异常"
                   << "\n  - 异常类型: " << typeid(ex).name()
                   << "\n  - 异常信息: " << ex.what()
                   << "\n  - employeeId: " << input.employee_id);
            WARN_PROTO("考勤错误响应已发送: type=error code=4000 msg=\"" << ex.what() << "\" msgId=" << msg_id << " to=" << to);
            session->write_line(protocol::build_error(
                protocol::kCodeBusinessValidation, ex.what(), msg_id, to));
          }
          return;
        }
        INFO_DB("考勤记录保存成功: employeeId=" << input.employee_id << " hasPhoto=true");
        INFO_PROTO("考勤响应已发送: type=attendance.report.response code=0 msgId=" << msg_id << " to=" << to);
        session->write_line(protocol::build_report_ack(
            protocol::kTypeAttendanceReportResponse, msg_id, to, 0, "ok",
            nullptr));
        if (push_registry) {
          INFO_EVENT("考勤事件推送: topic=attendance msgId=push-" << msg_id);
          DBG_EVENT("推送详情: employeeId=" << input.employee_id
                    << " checkTime=" << input.check_time
                    << " status=" << input.status
                    << " hasPhoto=true");
          nlohmann::json root;
          root["type"] = std::string(protocol::kTypeAttendancePush);
          root["role"] = std::string(protocol::kRoleServer);
          root["from"] = std::string(protocol::kRoleServer);
          root["msgId"] = std::string("push-") + msg_id;
          root["ts"] = static_cast<std::int64_t>(
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count());
          root["data"] = push_data;
          push_registry->publish("attendance", root.dump());
        }
      });
}

}  // namespace service
