#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <nlohmann/json_fwd.hpp>

namespace protocol {

// 紧凑单行 JSON（不含 \\n）；失败时返回空串（阶段一应仅在内存充足时出现）。
std::string build_auth_response(std::string_view in_reply_to,
                                std::string_view to,
                                std::string_view mock_session_token,
                                int heartbeat_sec,
                                const std::vector<std::string>& roles = {},
                                const std::unordered_set<std::string>& permissions = {});

// 支持自定义 data 对象的重载版本（用于 JWT 认证）
std::string build_auth_response(std::string_view in_reply_to,
                                std::string_view to,
                                std::string_view mock_session_token,
                                int heartbeat_sec,
                                const nlohmann::json& custom_data);

std::string build_heartbeat_response(std::string_view in_reply_to,
                                     std::string_view to);

std::string build_error(int code, std::string_view msg,
                        std::string_view in_reply_to, std::string_view to);

// 业务响应：与 auth_response 同为根级 code/msg/inReplyTo（通信协议 §3）。
std::string build_report_ack(std::string_view response_type,
                             std::string_view in_reply_to, std::string_view to,
                             int code, std::string_view msg,
                             const nlohmann::json* optional_data);

}  // namespace protocol
