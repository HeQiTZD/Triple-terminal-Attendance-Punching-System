#include "config/ConsoleConfigUi.hpp"

#include "config/ConfigFile.hpp"
#include "util/PasswordHash.hpp"

#include <cctype>
#include <charconv>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace config {

namespace {

bool schemaCharsOk(const std::string& s) {
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

std::string readLine(const std::string& prompt) {
  std::cout << prompt;
  std::cout.flush();
  std::string line;
  std::getline(std::cin, line);
  return line;
}

bool promptYesNo(const std::string& question, bool default_yes = false) {
  std::ostringstream os;
  os << question << " [" << (default_yes ? "Y/n" : "y/N") << "]: ";
  std::string line = readLine(os.str());
  if (line.empty()) {
    return default_yes;
  }
  if (line == "y" || line == "Y" || line == "1") {
    return true;
  }
  if (line == "n" || line == "N" || line == "0") {
    return false;
  }
  std::cout << "  无效输入，视为 "
            << (default_yes ? "是" : "否") << "。" << std::endl;
  return default_yes;
}

std::string promptString(const std::string& label, const std::string& current) {
  std::ostringstream os;
  os << "  " << label << " [" << current << "]: ";
  std::string line = readLine(os.str());
  return line.empty() ? current : line;
}

bool promptBool(const std::string& label, bool current) {
  std::ostringstream os;
  os << "  " << label << " [" << (current ? "y" : "n") << "] (y/n): ";
  std::string line = readLine(os.str());
  if (line.empty()) {
    return current;
  }
  if (line == "y" || line == "Y" || line == "1") {
    return true;
  }
  if (line == "n" || line == "N" || line == "0") {
    return false;
  }
  std::cout << "  无效输入，保留原值。" << std::endl;
  return current;
}

bool parseIntLine(const std::string& line, int& out) {
  if (line.empty()) {
    return false;
  }
  long v = 0;
  auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), v);
  if (ec != std::errc() || ptr != line.data() + line.size()) {
    return false;
  }
  out = static_cast<int>(v);
  return true;
}

bool parseSizeLine(const std::string& line, std::size_t& out) {
  if (line.empty()) {
    return false;
  }
  unsigned long long v = 0;
  auto [ptr, ec] =
      std::from_chars(line.data(), line.data() + line.size(), v);
  if (ec != std::errc() || ptr != line.data() + line.size() || v > 65535) {
    return false;
  }
  out = static_cast<std::size_t>(v);
  return true;
}

int promptInt(const std::string& label, int current) {
  for (;;) {
    std::ostringstream os;
    os << "  " << label << " [" << current << "]: ";
    std::string line = readLine(os.str());
    if (line.empty()) {
      return current;
    }
    int v = 0;
    if (parseIntLine(line, v)) {
      return v;
    }
    std::cout << "  请输入有效整数。" << std::endl;
  }
}

std::size_t promptSize(const std::string& label, std::size_t current) {
  for (;;) {
    std::ostringstream os;
    os << "  " << label << " [" << current << "] (0=自动): ";
    std::string line = readLine(os.str());
    if (line.empty()) {
      return current;
    }
    std::size_t v = 0;
    if (parseSizeLine(line, v)) {
      return v;
    }
    std::cout << "  请输入 0-65535 的整数。" << std::endl;
  }
}

unsigned short promptPort(unsigned short current) {
  for (;;) {
    std::ostringstream os;
    os << "  端口 [" << current << "]: ";
    std::string line = readLine(os.str());
    if (line.empty()) {
      return current;
    }
    unsigned long v = 0;
    auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), v);
    if (ec == std::errc() && ptr == line.data() + line.size() && v > 0 &&
        v <= 65535) {
      return static_cast<unsigned short>(v);
    }
    std::cout << "  请输入 1-65535 的端口。" << std::endl;
  }
}

void pauseContinue() { readLine("\n按 Enter 返回主菜单..."); }

std::string promptPassword(const std::string& label) {
  std::ostringstream os;
  os << "  " << label << ": ";
  return readLine(os.str());
}

std::string promptNewPassword() {
  for (;;) {
    std::string pw = promptPassword("请输入密码");
    std::string err = util::validate_password_strength(pw);
    if (!err.empty()) {
      // 将英文错误信息翻译为中文
      if (err.find("at least 8") != std::string::npos) {
        std::cout << "  密码长度至少 8 个字符。" << std::endl;
      } else if (err.find("letter and one digit") != std::string::npos) {
        std::cout << "  密码至少包含一个字母和一个数字。" << std::endl;
      } else {
        std::cout << "  " << err << std::endl;
      }
      continue;
    }
    std::string pw2 = promptPassword("请确认密码");
    if (pw != pw2) {
      std::cout << "  两次密码不一致，请重试。" << std::endl;
      continue;
    }
    return pw;
  }
}

void registerSuperAdmin(ServerConfig& cfg) {
  std::cout << "\n======== 注册超级管理员 ========\n";
  if (cfg.pending_super_admin.has_value()) {
    std::cout << "已暂存超级管理员信息（工号: "
              << cfg.pending_super_admin->employee_id
              << "），将在服务启动后自动创建。\n";
    pauseContinue();
    return;
  }

  std::string employee_id = readLine("  请输入员工工号: ");
  if (employee_id.empty()) {
    std::cout << "  工号不能为空。\n";
    pauseContinue();
    return;
  }

  std::string name = readLine("  请输入姓名: ");
  if (name.empty()) {
    std::cout << "  姓名不能为空。\n";
    pauseContinue();
    return;
  }

  std::string password = promptNewPassword();

  if (!promptYesNo("确认创建超级管理员?")) {
    std::cout << "  已取消。\n";
    pauseContinue();
    return;
  }

  cfg.pending_super_admin = PendingSuperAdmin{
      std::move(employee_id), std::move(name), std::move(password)};
  std::cout << "  信息已暂存，将在服务启动后自动创建。\n";
  pauseContinue();
}

template <typename T>
void printField(std::ostream& os, const char* name, const T& value,
                const T& defval) {
  os << "  " << name << '=' << value;
  if (value != defval) {
    os << "  (默认: " << defval << ')';
  }
  os << '\n';
}

void printPasswordField(std::ostream& os, const char* name,
                        const std::string& value,
                        const std::string& defval) {
  os << "  " << name << '='
     << (value.empty() ? "(empty)" : "****");
  if (value != defval) {
    os << "  (默认: " << (defval.empty() ? "(empty)" : "****") << ')';
  }
  os << '\n';
}

void printBoolField(std::ostream& os, const char* name, bool value, bool defval) {
  os << "  " << name << '=' << (value ? "yes" : "no");
  if (value != defval) {
    os << "  (默认: " << (defval ? "yes" : "no") << ')';
  }
  os << '\n';
}

void printDatabaseSection(const ServerConfig& cfg, const ServerConfig& def,
                          std::ostream& os) {
  os << "--- 数据库 (MySQL X) ---\n";
  printField(os, "host", cfg.db.host, def.db.host);
  printField(os, "port", cfg.db.port, def.db.port);
  printField(os, "user", cfg.db.user, def.db.user);
  printPasswordField(os, "password", cfg.db.password, def.db.password);
  printField(os, "schema", cfg.db.schema, def.db.schema);
}

void printTcpSection(const ServerConfig& cfg, const ServerConfig& def,
                     std::ostream& os) {
  os << "--- TCP 网关 ---\n";
  printBoolField(os, "enabled", cfg.enable_tcp, def.enable_tcp);
  printField(os, "bind", cfg.tcp_bind, def.tcp_bind);
  printField(os, "port", cfg.tcp_port, def.tcp_port);
  printField(os, "workers", cfg.tcp_worker_threads, def.tcp_worker_threads);
  os << "  (workers: 0=自动)\n";
}

void printDbPoolSection(const ServerConfig& cfg, const ServerConfig& def,
                        std::ostream& os) {
  os << "--- 数据库线程池 ---\n";
  printField(os, "db_pool_threads", cfg.db_pool_threads, def.db_pool_threads);
  os << "  (0=自动)\n";
}

void printGatewaySection(const ServerConfig& cfg, const ServerConfig& def,
                         std::ostream& os) {
  os << "--- 网关协议 ---\n";
  printField(os, "heartbeat_sec", cfg.gateway.heartbeat_sec,
             def.gateway.heartbeat_sec);
  printField(os, "grace_multiplier", cfg.gateway.heartbeat_grace_multiplier,
             def.gateway.heartbeat_grace_multiplier);
  const char* policy =
      cfg.gateway.duplicate_policy == service::DuplicateDevicePolicy::KickOld
          ? "kick_old"
          : "reject_new";
  const char* def_policy =
      def.gateway.duplicate_policy == service::DuplicateDevicePolicy::KickOld
          ? "kick_old"
          : "reject_new";
  printField(os, "duplicate_policy", std::string(policy), std::string(def_policy));
  printField(os, "default_device_key", cfg.gateway.default_device_key,
             def.gateway.default_device_key);
  os << "  device_keys 条目数=" << cfg.gateway.device_keys.size() << '\n';
}

void printArcfaceSection(const ServerConfig& cfg, const ServerConfig& def,
                         std::ostream& os) {
  os << "--- ArcFace ---\n";
  printBoolField(os, "enabled", cfg.arcface.enabled, def.arcface.enabled);
  printField(os, "min_face_size", cfg.arcface.min_face_size,
             def.arcface.min_face_size);
  os << "  app_id="
     << (cfg.arcface.app_id.empty() ? "(empty)" : cfg.arcface.app_id);
  if (cfg.arcface.app_id != def.arcface.app_id) {
    os << "  (默认: "
       << (def.arcface.app_id.empty() ? "(empty)" : def.arcface.app_id) << ')';
  }
  os << '\n';
  os << "  sdk_key="
     << (cfg.arcface.sdk_key.empty() ? "(empty)" : "****");
  if (cfg.arcface.sdk_key != def.arcface.sdk_key) {
    os << "  (默认: "
       << (def.arcface.sdk_key.empty() ? "(empty)" : "****") << ')';
  }
  os << '\n';
}

bool saveConfigIfValid(const std::string& config_path, const ServerConfig& cfg) {
  if (!schemaCharsOk(cfg.db.schema)) {
    std::cout << "  schema 无效，未保存（仅允许字母、数字、下划线且非空）。"
              << std::endl;
    return false;
  }
  std::string err;
  if (!saveToJson(config_path, cfg, err)) {
    std::cout << "  " << err << std::endl;
    return false;
  }
  std::cout << "  已保存到 " << config_path << std::endl;
  return true;
}

void editDatabase(ServerConfig& cfg) {
  std::cout << "\n--- 修改数据库 ---\n";
  cfg.db.host = promptString("主机 host", cfg.db.host);
  cfg.db.port = promptInt("端口 port", cfg.db.port);
  cfg.db.user = promptString("用户 user", cfg.db.user);
  cfg.db.password = promptString("密码 password", cfg.db.password);
  for (;;) {
    cfg.db.schema = promptString("库名 schema", cfg.db.schema);
    if (schemaCharsOk(cfg.db.schema)) {
      break;
    }
    std::cout << "  schema 仅允许字母、数字、下划线且非空。" << std::endl;
  }
}

void editTcp(ServerConfig& cfg) {
  std::cout << "\n--- 修改 TCP 网关 ---\n";
  std::cout << "  修改监听地址/端口需重启进程后生效。\n";
  cfg.enable_tcp = promptBool("启用 TCP", cfg.enable_tcp);
  cfg.tcp_bind = promptString("监听地址 bind", cfg.tcp_bind);
  cfg.tcp_port = promptPort(cfg.tcp_port);
  cfg.tcp_worker_threads =
      promptSize("IO 线程数 workers", cfg.tcp_worker_threads);
}

void editDbPool(ServerConfig& cfg) {
  std::cout << "\n--- 修改数据库线程池 ---\n";
  std::cout << "  修改后需重启进程后生效。\n";
  cfg.db_pool_threads =
      promptSize("DbExecutor 线程数", cfg.db_pool_threads);
}

void editGateway(ServerConfig& cfg) {
  std::cout << "\n--- 修改网关协议行为 ---\n";
  cfg.gateway.heartbeat_sec =
      promptInt("心跳间隔 heartbeat_sec (秒)", cfg.gateway.heartbeat_sec);
  cfg.gateway.heartbeat_grace_multiplier = promptInt(
      "心跳宽限倍数 heartbeat_grace_multiplier",
      cfg.gateway.heartbeat_grace_multiplier);
  std::cout << "  重复连接策略: 1=KickOld(踢旧) 2=RejectNew(拒绝新) ["
            << (cfg.gateway.duplicate_policy ==
                        service::DuplicateDevicePolicy::KickOld
                    ? "1"
                    : "2")
            << "]: ";
  std::string line = readLine("");
  if (line == "1") {
    cfg.gateway.duplicate_policy = service::DuplicateDevicePolicy::KickOld;
  } else if (line == "2") {
    cfg.gateway.duplicate_policy = service::DuplicateDevicePolicy::RejectNew;
  }
  cfg.gateway.default_device_key =
      promptString("默认设备密钥 default_device_key",
                   cfg.gateway.default_device_key);
}

void editArcface(ServerConfig& cfg) {
  std::cout << "\n--- 修改 ArcFace ---\n";
  cfg.arcface.enabled = promptBool("启用 ArcFace", cfg.arcface.enabled);
  cfg.arcface.app_id = promptString("AppId", cfg.arcface.app_id);
  cfg.arcface.sdk_key = promptString("SdkKey", cfg.arcface.sdk_key);
  cfg.arcface.min_face_size =
      promptInt("最小人脸 min_face_size", cfg.arcface.min_face_size);
}

void handleSection(const char* title, ServerConfig& cfg,
                   const ServerConfig& defaults, const std::string& config_path,
                   void (*print_section)(const ServerConfig&, const ServerConfig&,
                                         std::ostream&),
                   void (*edit_section)(ServerConfig&)) {
  std::cout << '\n' << title << '\n';
  print_section(cfg, defaults, std::cout);
  if (!promptYesNo("是否修改以上配置?")) {
    pauseContinue();
    return;
  }
  edit_section(cfg);
  applyConfigDefaults(cfg);
  if (promptYesNo("是否保存到配置文件?")) {
    saveConfigIfValid(config_path, cfg);
  }
  pauseContinue();
}

void printMainMenu(const std::string& config_path) {
  std::cout << "\n======== 考勤服务端配置 ========\n";
  std::cout << "配置文件: " << config_path << "\n";
  std::cout << "[1] 数据库          [2] TCP 网关\n";
  std::cout << "[3] 数据库线程池    [4] 网关协议行为\n";
  std::cout << "[5] ArcFace         [6] 查看全部配置\n";
  std::cout << "[7] 保存并启动      [8] 注册超级管理员\n";
  std::cout << "[0] 退出（不启动）\n";
  std::cout << "（各分项先显示当前值，可选修改并保存）\n";
  std::cout << "请选择: ";
  std::cout.flush();
}

}  // namespace

bool isInteractiveTerminal() {
#ifdef _WIN32
  return _isatty(_fileno(stdin)) != 0;
#else
  return isatty(STDIN_FILENO) != 0;
#endif
}

void printConfigDetail(const ServerConfig& cfg, std::ostream& os) {
  const ServerConfig defaults = defaultServerConfig();
  printDatabaseSection(cfg, defaults, os);
  printTcpSection(cfg, defaults, os);
  printDbPoolSection(cfg, defaults, os);
  printGatewaySection(cfg, defaults, os);
  printArcfaceSection(cfg, defaults, os);
}

bool runConsoleConfigUi(ServerConfig& cfg, const std::string& config_path) {
  const ServerConfig defaults = defaultServerConfig();
  applyConfigDefaults(cfg);

  for (;;) {
    printMainMenu(config_path);
    std::string choice = readLine("");
    if (choice == "1") {
      handleSection("【数据库】", cfg, defaults, config_path, printDatabaseSection,
                    editDatabase);
    } else if (choice == "2") {
      handleSection("【TCP 网关】", cfg, defaults, config_path, printTcpSection,
                    editTcp);
    } else if (choice == "3") {
      handleSection("【数据库线程池】", cfg, defaults, config_path,
                    printDbPoolSection, editDbPool);
    } else if (choice == "4") {
      handleSection("【网关协议行为】", cfg, defaults, config_path,
                    printGatewaySection, editGateway);
    } else if (choice == "5") {
      handleSection("【ArcFace】", cfg, defaults, config_path,
                    printArcfaceSection, editArcface);
    } else if (choice == "6") {
      std::cout << '\n';
      printConfigDetail(cfg, std::cout);
      pauseContinue();
    } else if (choice == "8") {
      registerSuperAdmin(cfg);
    } else if (choice == "7") {
      applyConfigDefaults(cfg);
      if (!schemaCharsOk(cfg.db.schema)) {
        std::cout << "schema 无效，请先在 [1] 数据库 中修正。" << std::endl;
        pauseContinue();
        continue;
      }
      std::string err;
      if (!saveToJson(config_path, cfg, err)) {
        std::cout << err << std::endl;
        pauseContinue();
        continue;
      }
      std::cout << "已保存到 " << config_path << "，正在启动服务..."
                << std::endl;
      return true;
    } else if (choice == "0") {
      std::cout << "已退出，未启动服务。" << std::endl;
      return false;
    } else {
      std::cout << "无效选项，请重试。" << std::endl;
    }
  }
}

}  // namespace config
