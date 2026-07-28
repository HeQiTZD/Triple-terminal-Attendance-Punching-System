#include "protocol/ResponseBuilders.hpp"

#include "protocol/ProtocolTypes.hpp"

#include <cassert>
#include <chrono>
#include <nlohmann/json.hpp>

namespace protocol {

namespace {

std::int64_t utc_ms_now() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
      .count();
}

std::string dump_compact_line(const nlohmann::json& j) {
  const std::string s = j.dump();
  assert(s.find('\n') == std::string::npos &&
         s.find('\r') == std::string::npos);
  return s;
}

}  // namespace

std::string build_auth_response(std::string_view in_reply_to,
                                std::string_view to,
                                std::string_view mock_session_token,
                                int heartbeat_sec,
                                const std::vector<std::string>& roles,
                                const std::unordered_set<std::string>& permissions) {
  const std::int64_t ts = utc_ms_now();

  nlohmann::json perm_arr = nlohmann::json::array();
  for (const auto& p : permissions) {
    perm_arr.push_back(p);
  }

  nlohmann::json role_arr = nlohmann::json::array();
  for (const auto& r : roles) {
    role_arr.push_back(r);
  }

  nlohmann::json j = {
      {"type", std::string(kTypeAuthResponse)},
      {"role", std::string(kRoleServer)},
      {"from", std::string(kRoleServer)},
      {"to", to},
      {"inReplyTo", in_reply_to},
      {"ts", ts},
      {"code", 0},
      {"msg", "ok"},
      {"data",
       {{"sessionToken", mock_session_token},
        {"serverTime", ts},
        {"heartbeatSec", heartbeat_sec},
        {"roles", role_arr},
        {"permissions", perm_arr}}}};
  return dump_compact_line(j);
}

std::string build_auth_response(std::string_view in_reply_to,
                                std::string_view to,
                                std::string_view mock_session_token,
                                int heartbeat_sec,
                                const nlohmann::json& custom_data) {
  const std::int64_t ts = utc_ms_now();

  nlohmann::json data = custom_data;
  // 确保包含基本字段
  if (!data.contains("sessionToken")) {
    data["sessionToken"] = mock_session_token;
  }
  if (!data.contains("serverTime")) {
    data["serverTime"] = ts;
  }
  if (!data.contains("heartbeatSec")) {
    data["heartbeatSec"] = heartbeat_sec;
  }

  nlohmann::json j = {
      {"type", std::string(kTypeAuthResponse)},
      {"role", std::string(kRoleServer)},
      {"from", std::string(kRoleServer)},
      {"to", to},
      {"inReplyTo", in_reply_to},
      {"ts", ts},
      {"code", 0},
      {"msg", "ok"},
      {"data", data}};
  return dump_compact_line(j);
}

std::string build_heartbeat_response(std::string_view in_reply_to,
                                     std::string_view to) {
  const std::int64_t ts = utc_ms_now();
  nlohmann::json j = {{"type", std::string(kTypeHeartbeatResponse)},
                       {"role", std::string(kRoleServer)},
                       {"from", std::string(kRoleServer)},
                       {"to", to},
                       {"inReplyTo", in_reply_to},
                       {"ts", ts},
                       {"code", 0},
                       {"msg", "ok"}};
  return dump_compact_line(j);
}

std::string build_error(int code, std::string_view msg,
                        std::string_view in_reply_to,
                        std::string_view to) {
  const std::int64_t ts = utc_ms_now();
  nlohmann::json j = {{"type", std::string(kTypeError)},
                       {"role", std::string(kRoleServer)},
                       {"from", std::string(kRoleServer)},
                       {"to", to},
                       {"inReplyTo", in_reply_to},
                       {"ts", ts},
                       {"code", code},
                       {"msg", msg}};
  return dump_compact_line(j);
}

std::string build_report_ack(std::string_view response_type,
                             std::string_view in_reply_to, std::string_view to,
                             int code, std::string_view msg,
                             const nlohmann::json* optional_data) {
  const std::int64_t ts = utc_ms_now();
  nlohmann::json j = {{"type", std::string(response_type)},
                       {"role", std::string(kRoleServer)},
                       {"from", std::string(kRoleServer)},
                       {"to", to},
                       {"inReplyTo", in_reply_to},
                       {"ts", ts},
                       {"code", code},
                       {"msg", msg}};
  if (optional_data != nullptr && !optional_data->is_null()) {
    j["data"] = *optional_data;
  } else {
    j["data"] = nlohmann::json::object();
  }
  return dump_compact_line(j);
}

}  // namespace protocol
