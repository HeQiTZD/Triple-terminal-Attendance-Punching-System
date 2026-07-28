#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace protocol {

// 通信协议 §3 Envelope + 顶层兼容字段（deviceId/status/message）合并入 data。
struct ParsedEnvelope {
  std::string type;
  std::string role;
  std::string from;
  std::string to;
  std::string msg_id;
  std::string in_reply_to;
  std::int64_t ts = 0;
  int code = 0;
  std::string msg;
  nlohmann::json data;  // object：业务字段；已合并顶层兼容键
};

// root 必须为 object 且含字符串字段 type。
std::optional<ParsedEnvelope> try_parse_envelope(const nlohmann::json& root);

}  // namespace protocol
