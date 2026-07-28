#include "config/ConfigFile.hpp"

#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>

namespace config {

namespace {

std::string getenv_copy(const char* key) {
  const char* v = std::getenv(key);
  return v ? std::string(v) : std::string{};
}

std::string duplicatePolicyToString(service::DuplicateDevicePolicy p) {
  return p == service::DuplicateDevicePolicy::RejectNew ? "reject_new"
                                                      : "kick_old";
}

bool duplicatePolicyFromString(const std::string& s,
                               service::DuplicateDevicePolicy& out) {
  if (s == "reject_new" || s == "RejectNew") {
    out = service::DuplicateDevicePolicy::RejectNew;
    return true;
  }
  if (s == "kick_old" || s == "KickOld" || s == "kick") {
    out = service::DuplicateDevicePolicy::KickOld;
    return true;
  }
  return false;
}

void mergeJsonIntoConfig(const nlohmann::json& j, ServerConfig& cfg) {
  if (j.contains("db") && j["db"].is_object()) {
    const auto& db = j["db"];
    if (db.contains("host") && db["host"].is_string()) {
      cfg.db.host = db["host"].get<std::string>();
    }
    if (db.contains("port") && db["port"].is_number_integer()) {
      cfg.db.port = db["port"].get<int>();
    }
    if (db.contains("user") && db["user"].is_string()) {
      cfg.db.user = db["user"].get<std::string>();
    }
    if (db.contains("password") && db["password"].is_string()) {
      cfg.db.password = db["password"].get<std::string>();
    }
    if (db.contains("schema") && db["schema"].is_string()) {
      cfg.db.schema = db["schema"].get<std::string>();
    }
  }

  if (j.contains("tcp") && j["tcp"].is_object()) {
    const auto& tcp = j["tcp"];
    if (tcp.contains("enabled") && tcp["enabled"].is_boolean()) {
      cfg.enable_tcp = tcp["enabled"].get<bool>();
    }
    if (tcp.contains("bind") && tcp["bind"].is_string()) {
      cfg.tcp_bind = tcp["bind"].get<std::string>();
    }
    if (tcp.contains("port") && tcp["port"].is_number_unsigned()) {
      cfg.tcp_port = static_cast<unsigned short>(tcp["port"].get<unsigned int>());
    }
    if (tcp.contains("worker_threads") &&
        tcp["worker_threads"].is_number_unsigned()) {
      cfg.tcp_worker_threads = tcp["worker_threads"].get<std::size_t>();
    }
  }

  if (j.contains("db_pool_threads") && j["db_pool_threads"].is_number_unsigned()) {
    cfg.db_pool_threads = j["db_pool_threads"].get<std::size_t>();
  }

  if (j.contains("gateway") && j["gateway"].is_object()) {
    const auto& gw = j["gateway"];
    if (gw.contains("heartbeat_sec") && gw["heartbeat_sec"].is_number_integer()) {
      cfg.gateway.heartbeat_sec = gw["heartbeat_sec"].get<int>();
    }
    if (gw.contains("heartbeat_grace_multiplier") &&
        gw["heartbeat_grace_multiplier"].is_number_integer()) {
      cfg.gateway.heartbeat_grace_multiplier =
          gw["heartbeat_grace_multiplier"].get<int>();
    }
    if (gw.contains("duplicate_policy") &&
        gw["duplicate_policy"].is_string()) {
      service::DuplicateDevicePolicy p;
      if (duplicatePolicyFromString(gw["duplicate_policy"].get<std::string>(),
                                    p)) {
        cfg.gateway.duplicate_policy = p;
      }
    }
    if (gw.contains("default_device_key") &&
        gw["default_device_key"].is_string()) {
      cfg.gateway.default_device_key =
          gw["default_device_key"].get<std::string>();
    }
    if (gw.contains("device_keys") && gw["device_keys"].is_object()) {
      for (auto it = gw["device_keys"].begin(); it != gw["device_keys"].end();
           ++it) {
        if (it.value().is_string()) {
          cfg.gateway.device_keys[it.key()] = it.value().get<std::string>();
        }
      }
    }
  }

  if (j.contains("arcface") && j["arcface"].is_object()) {
    const auto& af = j["arcface"];
    if (af.contains("enabled") && af["enabled"].is_boolean()) {
      cfg.arcface.enabled = af["enabled"].get<bool>();
    }
    if (af.contains("app_id") && af["app_id"].is_string()) {
      cfg.arcface.app_id = af["app_id"].get<std::string>();
    }
    if (af.contains("sdk_key") && af["sdk_key"].is_string()) {
      cfg.arcface.sdk_key = af["sdk_key"].get<std::string>();
    }
    if (af.contains("min_face_size") && af["min_face_size"].is_number_integer()) {
      cfg.arcface.min_face_size = af["min_face_size"].get<int>();
    }
  }
}

nlohmann::json configToJson(const ServerConfig& cfg) {
  nlohmann::json j;
  j["db"] = {{"host", cfg.db.host},
             {"port", cfg.db.port},
             {"user", cfg.db.user},
             {"password", cfg.db.password},
             {"schema", cfg.db.schema}};
  j["tcp"] = {{"enabled", cfg.enable_tcp},
              {"bind", cfg.tcp_bind},
              {"port", cfg.tcp_port},
              {"worker_threads", cfg.tcp_worker_threads}};
  j["db_pool_threads"] = cfg.db_pool_threads;
  j["gateway"] = {{"heartbeat_sec", cfg.gateway.heartbeat_sec},
                  {"heartbeat_grace_multiplier",
                   cfg.gateway.heartbeat_grace_multiplier},
                  {"duplicate_policy",
                   duplicatePolicyToString(cfg.gateway.duplicate_policy)},
                  {"default_device_key", cfg.gateway.default_device_key}};
  if (!cfg.gateway.device_keys.empty()) {
    j["gateway"]["device_keys"] = nlohmann::json::object();
    for (const auto& [k, v] : cfg.gateway.device_keys) {
      j["gateway"]["device_keys"][k] = v;
    }
  }
  j["arcface"] = {{"enabled", cfg.arcface.enabled},
                  {"app_id", cfg.arcface.app_id},
                  {"sdk_key", cfg.arcface.sdk_key},
                  {"min_face_size", cfg.arcface.min_face_size}};
  return j;
}

}  // namespace

std::string defaultConfigPath() { return "attendance.json"; }

std::string resolveConfigPath(int argc, char** argv) {
  std::string v = getenv_copy("ATTENDANCE_CONFIG_FILE");
  if (!v.empty()) {
    return v;
  }
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--config" && i + 1 < argc) {
      return argv[i + 1];
    }
  }
  return defaultConfigPath();
}

bool loadFromJson(const std::string& path, ServerConfig& cfg, std::string& err) {
  err.clear();
  std::ifstream in(path);
  if (!in) {
    return false;
  }
  try {
    nlohmann::json j;
    in >> j;
    if (!j.is_object()) {
      err = "配置文件根节点必须是 JSON 对象: " + path;
      return false;
    }
    mergeJsonIntoConfig(j, cfg);
    return true;
  } catch (const std::exception& ex) {
    err = std::string("解析配置文件失败: ") + ex.what();
    return false;
  }
}

bool saveToJson(const std::string& path, const ServerConfig& cfg,
                std::string& err) {
  err.clear();
  try {
    std::ofstream out(path);
    if (!out) {
      err = "无法写入配置文件: " + path;
      return false;
    }
    out << configToJson(cfg).dump(2) << '\n';
    return true;
  } catch (const std::exception& ex) {
    err = std::string("保存配置文件失败: ") + ex.what();
    return false;
  }
}

}  // namespace config
