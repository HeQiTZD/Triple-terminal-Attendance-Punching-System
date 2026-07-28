#include "config/ServerConfig.hpp"

#include "config/ConfigFile.hpp"
#include "config/ConsoleConfigUi.hpp"
#include "net/Framing.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace config {

namespace {

// 环境变量（前缀 ATTENDANCE_）：
//   DB_HOST, DB_PORT, DB_USER, DB_PASSWORD, DB_SCHEMA
//   TCP_BIND, TCP_PORT, TCP_WORKERS, DB_POOL_THREADS
//   HEARTBEAT_SEC
//   ARCFACE (=1/0/true/false), ARCFACE_APP_ID, ARCFACE_SDK_KEY
//   CONFIG_FILE           配置文件路径（等同 --config）
//
// 加载顺序：默认值 → attendance.json（或 --config 指定）→ 环境变量 → 命令行
//
// 命令行：
//   --tcp [PORT]          启用监听；可选端口，默认 8080
//   --bind ADDR           监听地址，默认 0.0.0.0
//   --workers N           TcpServer IO 线程数，0=自动
//   --db-workers N        DbExecutor 线程池大小，0=自动
//   --db-host / --db-port / --db-user / --db-password / --db-schema
//   --arcface             启用人脸引擎（需有效 AppId/SdkKey 或已激活环境）
//   --no-arcface          禁用人脸引擎（默认）
//   --config PATH         配置文件路径（默认 attendance.json）
//   --help                打印帮助并退出（返回错误串 "help"）

std::string getenv_copy(const char* key) {
  const char* v = std::getenv(key);
  return v ? std::string(v) : std::string{};
}

bool schema_chars_ok(const std::string& s) {
  if (s.empty()) {
    return false;
  }
  for (unsigned char c : s) {
    if (!(std::isalnum(c) != 0 || c == '_')) {
      return false;
    }
  }
  return true;
}

bool parse_u16(std::string_view sv, unsigned short& out) {
  unsigned long v = 0;
  auto first = sv.data();
  auto last = sv.data() + sv.size();
  auto res = std::from_chars(first, last, v);
  if (res.ec != std::errc() || res.ptr != last || v == 0 || v > 65535) {
    return false;
  }
  out = static_cast<unsigned short>(v);
  return true;
}

bool parse_size(std::string_view sv, std::size_t& out) {
  unsigned long long v = 0;
  auto first = sv.data();
  auto last = sv.data() + sv.size();
  auto res = std::from_chars(first, last, v);
  if (res.ec != std::errc() || res.ptr != last || v > 65535) {
    return false;
  }
  out = static_cast<std::size_t>(v);
  return true;
}

bool parse_bool(std::string_view sv, bool& out) {
  if (sv == "1" || sv == "true" || sv == "True" || sv == "TRUE" ||
      sv == "yes" || sv == "on") {
    out = true;
    return true;
  }
  if (sv == "0" || sv == "false" || sv == "False" || sv == "FALSE" ||
      sv == "no" || sv == "off") {
    out = false;
    return true;
  }
  return false;
}

bool parse_int_strict(std::string_view sv, int& out) {
  long v = 0;
  auto first = sv.data();
  auto last = sv.data() + sv.size();
  auto res = std::from_chars(first, last, v);
  if (res.ec != std::errc() || res.ptr != last || v < 0 ||
      v > 86400) {  // 合理心跳上界
    return false;
  }
  out = static_cast<int>(v);
  return true;
}

void apply_env(ServerConfig& cfg) {
  std::string v;

  v = getenv_copy("ATTENDANCE_DB_HOST");
  if (!v.empty()) {
    cfg.db.host = std::move(v);
  }
  v = getenv_copy("ATTENDANCE_DB_PORT");
  if (!v.empty()) {
    int p = 0;
    if (auto [ptr, ec] = std::from_chars(v.data(), v.data() + v.size(), p);
        ec == std::errc() && ptr == v.data() + v.size() && p > 0 &&
        p <= 65535) {
      cfg.db.port = p;
    }
  }
  v = getenv_copy("ATTENDANCE_DB_USER");
  if (!v.empty()) {
    cfg.db.user = std::move(v);
  }
  v = getenv_copy("ATTENDANCE_DB_PASSWORD");
  if (!v.empty()) {
    cfg.db.password = std::move(v);
  }
  v = getenv_copy("ATTENDANCE_DB_SCHEMA");
  if (!v.empty()) {
    cfg.db.schema = std::move(v);
  }

  v = getenv_copy("ATTENDANCE_TCP_BIND");
  if (!v.empty()) {
    cfg.tcp_bind = std::move(v);
  }
  v = getenv_copy("ATTENDANCE_TCP_PORT");
  if (!v.empty()) {
    unsigned short p = 0;
    if (parse_u16(v, p)) {
      cfg.tcp_port = p;
    }
  }
  v = getenv_copy("ATTENDANCE_TCP_WORKERS");
  if (!v.empty()) {
    std::size_t n = 0;
    if (parse_size(v, n)) {
      cfg.tcp_worker_threads = n;
    }
  }
  v = getenv_copy("ATTENDANCE_DB_POOL_THREADS");
  if (!v.empty()) {
    std::size_t n = 0;
    if (parse_size(v, n)) {
      cfg.db_pool_threads = n;
    }
  }
  v = getenv_copy("ATTENDANCE_HEARTBEAT_SEC");
  if (!v.empty()) {
    int hb = 0;
    if (parse_int_strict(v, hb)) {
      cfg.gateway.heartbeat_sec = hb;
    }
  }
  v = getenv_copy("ATTENDANCE_ARCFACE");
  if (!v.empty()) {
    bool on = false;
    if (parse_bool(v, on)) {
      cfg.arcface.enabled = on;
    }
  }
  v = getenv_copy("ATTENDANCE_ARCFACE_APP_ID");
  if (!v.empty()) {
    cfg.arcface.app_id = std::move(v);
  }
  v = getenv_copy("ATTENDANCE_ARCFACE_SDK_KEY");
  if (!v.empty()) {
    cfg.arcface.sdk_key = std::move(v);
  }
}

std::string validate(const ServerConfig& cfg) {
  if (!schema_chars_ok(cfg.db.schema)) {
    return "无效的 DB schema（仅允许字母数字与下划线，且非空）";
  }
  return {};
}

}  // namespace

std::string loadServerConfig(int argc, char** argv, ServerConfig& out) {
  out = ServerConfig{};

  const std::string config_path = resolveConfigPath(argc, argv);
  std::string json_err;
  if (loadFromJson(config_path, out, json_err)) {
    // merged
  } else if (!json_err.empty()) {
    return json_err;
  }

  apply_env(out);

  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--help" || a == "-h") {
      std::ostringstream os;
      os << "用法: attendanceServer [选项]\n"
         << "  --tcp [PORT]       启用 TCP 网关（默认端口 8080）\n"
         << "  --bind ADDR        监听地址（默认 0.0.0.0）\n"
         << "  --workers N        IO 线程数，0=自动\n"
         << "  --db-workers N     DB 线程池大小，0=自动\n"
         << "  --db-host HOST\n"
         << "  --db-port PORT\n"
         << "  --db-user USER\n"
         << "  --db-password PASS\n"
         << "  --db-schema NAME\n"
         << "  --config PATH      配置文件（默认 attendance.json）\n"
         << "  --arcface            启用人脸引擎\n"
         << "  --no-arcface         禁用人脸引擎（默认）\n"
         << "环境变量: ATTENDANCE_DB_* ATTENDANCE_TCP_* ATTENDANCE_DB_POOL_THREADS "
            "ATTENDANCE_HEARTBEAT_SEC ATTENDANCE_ARCFACE* ATTENDANCE_CONFIG_FILE\n"
         << "交互式终端下启动将进入配置菜单；配置保存至 attendance.json\n";
      std::cerr << os.str();
      return "help";
    }
    if (a == "--config") {
      if (i + 1 >= argc) {
        return "--config 需要参数";
      }
      ++i;
      continue;
    }
    if (a == "--tcp") {
      out.enable_tcp = true;
      if (i + 1 < argc) {
        std::string next = argv[i + 1];
        if (!next.empty() && next[0] != '-') {
          unsigned short p = 0;
          if (parse_u16(next, p)) {
            out.tcp_port = p;
          }
          ++i;
        }
      }
      continue;
    }
    auto take_next = [&](std::string& dest) -> bool {
      if (i + 1 >= argc) {
        return false;
      }
      dest = argv[++i];
      return true;
    };
    if (a == "--bind") {
      std::string v;
      if (!take_next(v)) {
        return "--bind 需要参数";
      }
      out.tcp_bind = std::move(v);
      continue;
    }
    if (a == "--workers") {
      std::string v;
      if (!take_next(v)) {
        return "--workers 需要参数";
      }
      std::size_t n = 0;
      if (!parse_size(v, n)) {
        return "无效的 --workers";
      }
      out.tcp_worker_threads = n;
      continue;
    }
    if (a == "--db-workers") {
      std::string v;
      if (!take_next(v)) {
        return "--db-workers 需要参数";
      }
      std::size_t n = 0;
      if (!parse_size(v, n)) {
        return "无效的 --db-workers";
      }
      out.db_pool_threads = n;
      continue;
    }
    if (a == "--db-host") {
      std::string v;
      if (!take_next(v)) {
        return "--db-host 需要参数";
      }
      out.db.host = std::move(v);
      continue;
    }
    if (a == "--db-port") {
      std::string v;
      if (!take_next(v)) {
        return "--db-port 需要参数";
      }
      int p = 0;
      if (auto [ptr, ec] =
              std::from_chars(v.data(), v.data() + v.size(), p);
          ec != std::errc() || ptr != v.data() + v.size() || p <= 0 ||
          p > 65535) {
        return "无效的 --db-port";
      }
      out.db.port = p;
      continue;
    }
    if (a == "--db-user") {
      std::string v;
      if (!take_next(v)) {
        return "--db-user 需要参数";
      }
      out.db.user = std::move(v);
      continue;
    }
    if (a == "--db-password") {
      std::string v;
      if (!take_next(v)) {
        return "--db-password 需要参数";
      }
      out.db.password = std::move(v);
      continue;
    }
    if (a == "--db-schema") {
      std::string v;
      if (!take_next(v)) {
        return "--db-schema 需要参数";
      }
      out.db.schema = std::move(v);
      continue;
    }
    if (a == "--arcface") {
      out.arcface.enabled = true;
      continue;
    }
    if (a == "--no-arcface") {
      out.arcface.enabled = false;
      continue;
    }
  }

  applyConfigDefaults(out);
  return validate(out);
}

ServerConfig defaultServerConfig() { return ServerConfig{}; }

void applyConfigDefaults(ServerConfig& cfg) {
  const ServerConfig d = defaultServerConfig();
  if (cfg.db.host.empty()) {
    cfg.db.host = d.db.host;
  }
  if (cfg.db.port <= 0) {
    cfg.db.port = d.db.port;
  }
  if (cfg.db.user.empty()) {
    cfg.db.user = d.db.user;
  }
  if (cfg.db.password.empty()) {
    cfg.db.password = d.db.password;
  }
  if (cfg.db.schema.empty()) {
    cfg.db.schema = d.db.schema;
  }
  if (cfg.tcp_bind.empty()) {
    cfg.tcp_bind = d.tcp_bind;
  }
  if (cfg.tcp_port == 0) {
    cfg.tcp_port = d.tcp_port;
  }
  if (cfg.gateway.default_device_key.empty()) {
    cfg.gateway.default_device_key = d.gateway.default_device_key;
  }
  if (cfg.gateway.heartbeat_sec <= 0) {
    cfg.gateway.heartbeat_sec = d.gateway.heartbeat_sec;
  }
  if (cfg.gateway.heartbeat_grace_multiplier <= 0) {
    cfg.gateway.heartbeat_grace_multiplier = d.gateway.heartbeat_grace_multiplier;
  }
  if (cfg.arcface.min_face_size <= 0) {
    cfg.arcface.min_face_size = d.arcface.min_face_size;
  }
}

void logConfigSummary(const ServerConfig& cfg) {
  printConfigDetail(cfg, std::cerr);
}

net::TcpServer::Config makeTcpServerConfig(const ServerConfig& cfg) {
  net::TcpServer::Config t;
  t.bind_address = cfg.tcp_bind;
  t.port = cfg.tcp_port;
  t.worker_threads = cfg.tcp_worker_threads;
  return t;
}

void logFramingLimitsSummary() {
  std::cerr << "limits: max_json_line_bytes=" << net::kMaxJsonLineBytes
            << " max_binary_payload_bytes=" << net::kMaxBinaryPayloadBytes
            << " max_recv_buffer_bytes=" << net::kMaxRecvBufferBytes << '\n';
}

}  // namespace config
