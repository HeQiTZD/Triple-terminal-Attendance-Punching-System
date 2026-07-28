#include "service/DeviceConfigService.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include "db/DeviceConfigRepository.hpp"
#include "db/OperationLogRepository.hpp"
#include "net/Session.hpp"
#include "protocol/DbErrorMap.hpp"
#include "protocol/ProtocolTypes.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "service/DbExecutor.hpp"
#include "service/MessageRouter.hpp"

namespace service {

namespace {

using IniData = std::map<std::string, std::map<std::string, std::string>>;

std::string trim(std::string s) {
  auto is_space = [](unsigned char c) { return std::isspace(c) != 0; };
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [&](unsigned char c) { return !is_space(c); }));
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [&](unsigned char c) { return !is_space(c); })
              .base(),
          s.end());
  return s;
}

std::optional<std::string> parse_ini(const std::string& content, IniData& out) {
  std::istringstream is(content);
  std::string line;
  std::string section;
  int line_no = 0;
  while (std::getline(is, line)) {
    ++line_no;
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    line = trim(line);
    if (line.empty() || line[0] == '#' || line[0] == ';') {
      continue;
    }
    if (line.front() == '[') {
      if (line.back() != ']' || line.size() <= 2) {
        return "invalid section header at line " + std::to_string(line_no);
      }
      section = trim(line.substr(1, line.size() - 2));
      if (section.empty()) {
        return "empty section header at line " + std::to_string(line_no);
      }
      out[section];
      continue;
    }
    const auto pos = line.find('=');
    if (pos == std::string::npos || section.empty()) {
      return "invalid key-value at line " + std::to_string(line_no);
    }
    std::string key = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));
    if (key.empty()) {
      return "empty key at line " + std::to_string(line_no);
    }
    out[section][key] = value;
  }
  return std::nullopt;
}

std::optional<std::string> require_section(const IniData& ini,
                                           const char* section) {
  if (ini.find(section) == ini.end()) {
    return std::string("missing [") + section + "] section";
  }
  return std::nullopt;
}

std::optional<std::string> require_key(const IniData& ini, const char* section,
                                       const char* key) {
  auto sit = ini.find(section);
  if (sit == ini.end() || sit->second.find(key) == sit->second.end()) {
    return std::string("missing key ") + section + "." + key;
  }
  return std::nullopt;
}

std::string ini_value(const IniData& ini, const char* section, const char* key) {
  auto sit = ini.find(section);
  if (sit == ini.end()) {
    return {};
  }
  auto kit = sit->second.find(key);
  return kit == sit->second.end() ? std::string{} : kit->second;
}

bool parse_int(const std::string& s, int& out) {
  if (s.empty()) {
    return false;
  }
  char* end = nullptr;
  long v = std::strtol(s.c_str(), &end, 10);
  if (end == nullptr || *end != '\0') {
    return false;
  }
  out = static_cast<int>(v);
  return true;
}

bool valid_time_hhmm(const std::string& s) {
  if (s.size() != 5 || s[2] != ':' || !std::isdigit(s[0]) ||
      !std::isdigit(s[1]) || !std::isdigit(s[3]) || !std::isdigit(s[4])) {
    return false;
  }
  const int hour = (s[0] - '0') * 10 + (s[1] - '0');
  const int minute = (s[3] - '0') * 10 + (s[4] - '0');
  return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

bool valid_bool(const std::string& s) {
  return s == "true" || s == "false" || s == "1" || s == "0";
}

std::optional<std::string> validate_int_range(const IniData& ini,
                                              const char* section,
                                              const char* key, int min_v,
                                              int max_v) {
  if (auto err = require_key(ini, section, key)) {
    return err;
  }
  int value = 0;
  const std::string raw = ini_value(ini, section, key);
  if (!parse_int(raw, value) || value < min_v || value > max_v) {
    return std::string("invalid value for ") + section + "." + key;
  }
  return std::nullopt;
}

std::optional<std::string> validate_time(const IniData& ini,
                                         const char* section,
                                         const char* key) {
  if (auto err = require_key(ini, section, key)) {
    return err;
  }
  if (!valid_time_hhmm(ini_value(ini, section, key))) {
    return std::string("invalid time for ") + section + "." + key;
  }
  return std::nullopt;
}

std::optional<std::string> validate_bool_key(const IniData& ini,
                                             const char* section,
                                             const char* key) {
  if (auto err = require_key(ini, section, key)) {
    return err;
  }
  if (!valid_bool(ini_value(ini, section, key))) {
    return std::string("invalid bool for ") + section + "." + key;
  }
  return std::nullopt;
}

std::optional<std::string> validate_config_content(
    const std::string& content) {
  if (trim(content).empty()) {
    return "configContent must not be empty";
  }

  IniData ini;
  if (auto err = parse_ini(content, ini)) {
    return err;
  }

  for (const char* section : {"FaceRecognition", "Attendance", "Device"}) {
    if (auto err = require_section(ini, section)) {
      return err;
    }
  }

  if (auto err = validate_int_range(ini, "FaceRecognition", "FaceThreshold", 0,
                                    100)) return err;
  if (auto err = validate_int_range(ini, "FaceRecognition", "MaxFaceCount", 1,
                                    100)) return err;
  if (auto err = validate_int_range(ini, "FaceRecognition", "RecognizeTimeout",
                                    1, 3600)) return err;

  for (const char* key :
       {"WorkStartTime", "WorkEndTime", "CoreWorkStart", "CoreWorkEnd",
        "LunchBreakStart", "LunchBreakEnd"}) {
    if (auto err = validate_time(ini, "Attendance", key)) return err;
  }
  for (const char* key :
       {"FlexibleWorkEnabled", "LunchBreakEnabled", "AllowCrossDay"}) {
    if (auto err = validate_bool_key(ini, "Attendance", key)) return err;
  }
  if (auto err = validate_int_range(ini, "Attendance", "CheckInStartOffset",
                                    -1440, 1440)) return err;
  for (const char* key :
       {"LateAllowance", "EarlyLeaveAllowance", "CheckOutEndOffset",
        "FlexibleRange", "MinCheckInterval", "MaxRetryCount",
        "RetryBackoffBaseMs"}) {
    if (auto err = validate_int_range(ini, "Attendance", key, 0, 86400)) {
      return err;
    }
  }
  if (auto err = validate_int_range(ini, "Attendance", "MaxWorkHours", 1, 24)) {
    return err;
  }
  if (auto err = require_key(ini, "Device", "FwVersion")) {
    return err;
  }
  return std::nullopt;
}

std::array<std::uint32_t, 8> sha256_initial_state() {
  return {0x6a09e667U, 0xbb67ae85U, 0x3c6ef372U, 0xa54ff53aU,
          0x510e527fU, 0x9b05688cU, 0x1f83d9abU, 0x5be0cd19U};
}

std::uint32_t rotr(std::uint32_t x, std::uint32_t n) {
  return (x >> n) | (x << (32U - n));
}

std::string sha256_hex(const std::string& input) {
  static constexpr std::array<std::uint32_t, 64> k = {
      0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U, 0x3956c25bU,
      0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U, 0xd807aa98U, 0x12835b01U,
      0x243185beU, 0x550c7dc3U, 0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U,
      0xc19bf174U, 0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU,
      0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU, 0x983e5152U,
      0xa831c66dU, 0xb00327c8U, 0xbf597fc7U, 0xc6e00bf3U, 0xd5a79147U,
      0x06ca6351U, 0x14292967U, 0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU,
      0x53380d13U, 0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
      0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U, 0xd192e819U,
      0xd6990624U, 0xf40e3585U, 0x106aa070U, 0x19a4c116U, 0x1e376c08U,
      0x2748774cU, 0x34b0bcb5U, 0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU,
      0x682e6ff3U, 0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U,
      0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U};

  std::vector<std::uint8_t> bytes(input.begin(), input.end());
  const std::uint64_t bit_len = static_cast<std::uint64_t>(bytes.size()) * 8U;
  bytes.push_back(0x80U);
  while ((bytes.size() % 64U) != 56U) {
    bytes.push_back(0U);
  }
  for (int i = 7; i >= 0; --i) {
    bytes.push_back(static_cast<std::uint8_t>((bit_len >> (i * 8)) & 0xffU));
  }

  auto h = sha256_initial_state();
  for (std::size_t offset = 0; offset < bytes.size(); offset += 64U) {
    std::array<std::uint32_t, 64> w{};
    for (std::size_t i = 0; i < 16; ++i) {
      const std::size_t j = offset + i * 4U;
      w[i] = (static_cast<std::uint32_t>(bytes[j]) << 24U) |
             (static_cast<std::uint32_t>(bytes[j + 1]) << 16U) |
             (static_cast<std::uint32_t>(bytes[j + 2]) << 8U) |
             static_cast<std::uint32_t>(bytes[j + 3]);
    }
    for (std::size_t i = 16; i < 64; ++i) {
      const std::uint32_t s0 =
          rotr(w[i - 15], 7U) ^ rotr(w[i - 15], 18U) ^ (w[i - 15] >> 3U);
      const std::uint32_t s1 =
          rotr(w[i - 2], 17U) ^ rotr(w[i - 2], 19U) ^ (w[i - 2] >> 10U);
      w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    std::uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
    std::uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];
    for (std::size_t i = 0; i < 64; ++i) {
      const std::uint32_t s1 = rotr(e, 6U) ^ rotr(e, 11U) ^ rotr(e, 25U);
      const std::uint32_t ch = (e & f) ^ ((~e) & g);
      const std::uint32_t temp1 = hh + s1 + ch + k[i] + w[i];
      const std::uint32_t s0 = rotr(a, 2U) ^ rotr(a, 13U) ^ rotr(a, 22U);
      const std::uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
      const std::uint32_t temp2 = s0 + maj;
      hh = g;
      g = f;
      f = e;
      e = d + temp1;
      d = c;
      c = b;
      b = a;
      a = temp1 + temp2;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d;
    h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
  }

  std::ostringstream os;
  os << std::hex << std::setfill('0');
  for (auto part : h) {
    os << std::setw(8) << part;
  }
  return os.str();
}

std::string date_version_prefix() {
  std::time_t now = std::time(nullptr);
  std::tm tm{};
#ifdef _WIN32
  localtime_s(&tm, &now);
#else
  localtime_r(&now, &tm);
#endif
  char buf[16] = {};
  std::strftime(buf, sizeof(buf), "%Y.%m.%d.", &tm);
  return buf;
}

std::string make_version(const std::string& prefix, int seq) {
  std::ostringstream os;
  os << prefix << std::setw(3) << std::setfill('0') << seq;
  return os.str();
}

std::string opt_string(const nlohmann::json& data, const char* key) {
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
  } catch (const std::invalid_argument& e) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, e.what(), msg_id, to));
  } catch (const std::exception& ex) {
    session->write_line(protocol::build_error(protocol::kCodeDbError, ex.what(),
                                              msg_id, to));
  }
}

void mark_failed(DbExecutor& db, std::int64_t target_id,
                 const std::string& error) {
  db.dispatch(
      [target_id, error](mysqlx::Session& s) {
        db::DeviceConfigRepository::markTargetFailedById(s, target_id, error);
      },
      [](std::exception_ptr) {});
}

void apply_ack(DbExecutor& db, std::int64_t target_id,
               const protocol::ParsedEnvelope& env) {
  bool success = false;
  if (env.data.contains("success") && env.data["success"].is_boolean()) {
    success = env.data["success"].get<bool>();
  } else {
    success = env.code == 0;
  }

  std::string error;
  if (!success) {
    error = opt_string(env.data, "result");
    if (error.empty()) {
      error = env.msg.empty() ? "device config apply failed" : env.msg;
    }
  }

  db.dispatch(
      [target_id, success, error](mysqlx::Session& s) {
        db::DeviceConfigRepository::applyAckByTargetId(s, target_id, success,
                                                       error);
      },
      [](std::exception_ptr) {});
}

nlohmann::json build_update_config_data(
    const db::PendingDeviceConfigTarget& target) {
  return {{"command", "update_config"},
          {"params",
           {{"config", target.config_content},
            {"configVersion", target.config_version},
            {"configHash", target.config_hash}}}};
}

MessageRouter::ForwardResult forward_config_target(
    const db::PendingDeviceConfigTarget& target, DbExecutor& db,
    MessageRouter& router) {
  nlohmann::json data = build_update_config_data(target);
  const std::string seed = "config." + std::to_string(target.deploy_id) + "." +
                           std::to_string(target.target_id);
  auto result = router.forward_server_device_command(
      target.device_id, data, seed,
      [&db, target_id = target.target_id](const std::string&,
                                          const protocol::ParsedEnvelope& env) {
        apply_ack(db, target_id, env);
      },
      [&db, target_id = target.target_id](const std::string&) {
        mark_failed(db, target_id, "device.command timeout");
      });

  if (result.delivered) {
    db.dispatch(
        [target_id = target.target_id, forward_id = result.forward_id](
            mysqlx::Session& s) {
          db::DeviceConfigRepository::markTargetSent(s, target_id, forward_id);
        },
        [](std::exception_ptr) {});
  }
  return result;
}

}  // namespace

void admin_config_deploy(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, DbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit, MessageRouter& router) {
  std::vector<std::string> devices;
  if (env.data.contains("targetDevices") &&
      env.data["targetDevices"].is_array()) {
    std::unordered_set<std::string> seen;
    for (const auto& item : env.data["targetDevices"]) {
      if (!item.is_string()) {
        continue;
      }
      std::string device_id = trim(item.get<std::string>());
      if (!device_id.empty() && seen.insert(device_id).second) {
        devices.push_back(std::move(device_id));
      }
    }
  }
  if (devices.empty()) {
    session->write_line(protocol::build_error(
        protocol::kCodeBusinessValidation, "targetDevices must not be empty",
        env.msg_id, reply_to));
    return;
  }

  const std::string config_content = opt_string(env.data, "configContent");
  if (auto err = validate_config_content(config_content)) {
    session->write_line(protocol::build_error(protocol::kCodeBusinessValidation,
                                              *err, env.msg_id, reply_to));
    return;
  }

  const std::string requested_version = opt_string(env.data, "configVersion");
  const std::string description = opt_string(env.data, "description");
  const std::string config_hash = "sha256:" + sha256_hex(config_content);

  // 从配置内容中提取 DeviceKey，用于同步到 Device 表的 key_hash 字段
  std::string device_key;
  {
    IniData ini;
    if (!parse_ini(config_content, ini)) {
      device_key = ini_value(ini, "Device", "DeviceKey");
    }
  }

  const std::string msg_id = env.msg_id;
  const std::string to = reply_to;

  struct DeployState {
    std::int64_t deploy_id = 0;
    std::string config_version;
    std::vector<db::PendingDeviceConfigTarget> targets;
  };
  auto state = std::make_shared<DeployState>();

  db.dispatch(
      [devices, config_content, requested_version, description, config_hash,
       created_by = audit.employee_id, audit, device_key, state](mysqlx::Session& s) {
        s.sql("USE attendanceserver").execute();
        s.sql("START TRANSACTION").execute();
        try {
          std::string version = requested_version;
          if (version.empty()) {
            const std::string prefix = date_version_prefix();
            const int count =
                db::DeviceConfigRepository::countDeploysWithVersionPrefix(
                    s, prefix);
            version = make_version(prefix, count + 1);
          }

          db::DeviceConfigDeployInput input;
          input.config_version = version;
          input.config_content = config_content;
          input.config_hash = config_hash;
          input.description = description;
          input.created_by = created_by.empty() ? "admin" : created_by;
          const std::int64_t deploy_id =
              db::DeviceConfigRepository::insertDeploy(s, input);

          state->deploy_id = deploy_id;
          state->config_version = version;
          for (const auto& device_id : devices) {
            db::PendingDeviceConfigTarget target;
            target.target_id =
                db::DeviceConfigRepository::insertTarget(s, deploy_id,
                                                         device_id);
            target.deploy_id = deploy_id;
            target.device_id = device_id;
            target.config_version = version;
            target.config_content = config_content;
            target.config_hash = config_hash;
            state->targets.push_back(std::move(target));
          }

          // 同步 DeviceKey 到 Device 表的 key_hash 字段
          if (!device_key.empty()) {
            for (const auto& device_id : devices) {
              s.sql("UPDATE Device SET key_hash = ? WHERE device_id = ?")
                  .bind(device_key, device_id)
                  .execute();
            }
          }

          db::OperationLogEntry log;
          log.user_id = audit.user_id;
          log.employee_id = audit.employee_id;
          log.action = "config.deploy";
          log.target_type = "device_config_deploy";
          log.target_id = std::to_string(deploy_id);
          log.detail = nlohmann::json({{"configVersion", version},
                                       {"configHash", config_hash},
                                       {"targetCount", devices.size()}})
                           .dump();
          log.ip_address = audit.ip_address;
          log.result = "success";
          db::OperationLogRepository::insertLog(s, log);
          s.sql("COMMIT").execute();
        } catch (...) {
          s.sql("ROLLBACK").execute();
          throw;
        }
      },
      [session, msg_id, to, state, config_hash, &db,
       &router](std::exception_ptr ep) {
        if (!session) return;
        if (ep) {
          send_db_error(session, ep, msg_id, to);
          return;
        }

        nlohmann::json targets = nlohmann::json::array();
        for (const auto& target : state->targets) {
          auto result = forward_config_target(target, db, router);
          targets.push_back({{"deviceId", target.device_id},
                             {"status", result.delivered ? "sent" : "pending"},
                             {"forwardMsgId", result.forward_id}});
        }

        nlohmann::json data = {{"deployId", state->deploy_id},
                               {"configVersion", state->config_version},
                               {"configHash", config_hash},
                               {"targets", std::move(targets)}};
        session->write_line(protocol::build_report_ack(
            protocol::kTypeConfigDeployResponse, msg_id, to, 0, "ok", &data));
      });
}

void push_pending_config_for_device(const std::string& device_id,
                                    DbExecutor& db, MessageRouter& router) {
  auto targets = std::make_shared<std::vector<db::PendingDeviceConfigTarget>>();
  db.dispatch(
      [device_id, targets](mysqlx::Session& s) {
        *targets =
            db::DeviceConfigRepository::listPendingTargetsForDevice(s,
                                                                    device_id);
      },
      [targets, &db, &router](std::exception_ptr ep) {
        if (ep) {
          return;
        }
        for (const auto& target : *targets) {
          forward_config_target(target, db, router);
        }
      });
}

}  // namespace service
