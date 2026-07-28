#pragma once

#include <string>
#include <unordered_map>

#include "service/DeviceRegistry.hpp"

namespace protocol {

struct GatewayConfig {
  int heartbeat_sec = 30;
  int heartbeat_grace_multiplier = 3;
  service::DuplicateDevicePolicy duplicate_policy =
      service::DuplicateDevicePolicy::KickOld;
  std::unordered_map<std::string, std::string> device_keys;
  std::string default_device_key = "changeme";

  // JWT 配置
  std::string jwt_secret = "default-jwt-secret-change-in-production-32bytes";
  int access_token_ttl = 3600;      // 访问令牌有效期（秒）
  int refresh_token_ttl = 604800;   // 刷新令牌有效期（秒）
  int clock_skew = 30;              // 时钟偏差容忍度（秒）
};

}  // namespace protocol
