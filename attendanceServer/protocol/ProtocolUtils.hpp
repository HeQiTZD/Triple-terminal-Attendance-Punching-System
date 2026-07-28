#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <boost/system/error_code.hpp>
#include <nlohmann/json.hpp>

#include "net/Session.hpp"
#include "protocol/ConnContext.hpp"

namespace protocol {

// 检查连接上下文是否有指定权限
inline bool has_permission(const ConnContext& ctx, std::string_view perm) {
  return ctx.permissions.count(std::string(perm)) > 0;
}

// 检查连接上下文是否有指定角色
inline bool has_role(const ConnContext& ctx, std::string_view role) {
  for (const auto& r : ctx.roles) {
    if (r == role) return true;
  }
  return false;
}

// 获取远程 IP 地址字符串
inline std::string remote_ip_string(net::ISession& session) {
  boost::system::error_code ec;
  auto ep = session.socket().remote_endpoint(ec);
  if (ec) {
    return {};
  }
  return ep.address().to_string();
}

// 从 JSON data 中读取 payloadLength 字段
inline std::optional<std::uint64_t> read_payload_length(const nlohmann::json& data) {
  if (!data.contains("payloadLength")) {
    return std::nullopt;
  }
  const auto& p = data["payloadLength"];
  if (p.is_number_unsigned()) {
    return p.get<std::uint64_t>();
  }
  if (p.is_number_integer()) {
    const auto v = p.get<std::int64_t>();
    if (v >= 0) {
      return static_cast<std::uint64_t>(v);
    }
  }
  return std::nullopt;
}

}  // namespace protocol