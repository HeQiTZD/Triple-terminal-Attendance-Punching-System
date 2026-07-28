#include "protocol/Envelope.hpp"

namespace protocol {

namespace {

std::string opt_string(const nlohmann::json& j, const char* key) {
  if (!j.contains(key) || !j[key].is_string()) {
    return {};
  }
  return j[key].get<std::string>();
}

void merge_compat_top_level(nlohmann::json& data,
                            const nlohmann::json& root,
                            const char* key) {
  if (data.contains(key)) {
    return;
  }
  if (root.contains(key)) {
    data[key] = root[key];
  }
}

}  // namespace

std::optional<ParsedEnvelope> try_parse_envelope(const nlohmann::json& root) {
  if (!root.is_object() || !root.contains("type") ||
      !root["type"].is_string()) {
    return std::nullopt;
  }

  ParsedEnvelope out;
  out.type = root["type"].get<std::string>();

  out.role = opt_string(root, "role");
  out.from = opt_string(root, "from");
  out.to = opt_string(root, "to");
  out.msg_id = opt_string(root, "msgId");
  out.in_reply_to = opt_string(root, "inReplyTo");
  if (root.contains("ts") && root["ts"].is_number_integer()) {
    out.ts = root["ts"].get<std::int64_t>();
  }
  if (root.contains("code") && root["code"].is_number_integer()) {
    out.code = static_cast<int>(root["code"].get<std::int64_t>());
  }
  out.msg = opt_string(root, "msg");

  nlohmann::json data = nlohmann::json::object();
  if (root.contains("data") && root["data"].is_object()) {
    data = root["data"];
  }

  // 通信协议 §3：兼容顶层 deviceId/status/message → 沉入 data（缺则补齐）。
  merge_compat_top_level(data, root, "deviceId");
  merge_compat_top_level(data, root, "deviceKey");
  merge_compat_top_level(data, root, "status");
  merge_compat_top_level(data, root, "message");

  out.data = std::move(data);
  return out;
}

}  // namespace protocol
