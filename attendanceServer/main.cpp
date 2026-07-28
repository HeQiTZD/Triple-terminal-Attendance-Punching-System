#include <exception>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

#include "config/ConfigFile.hpp"
#include "config/ConsoleConfigUi.hpp"
#include "config/ServerConfig.hpp"
#include "db/DbClient.hpp"
#include "db/Schema.hpp"
#include "net/TcpServer.hpp"
#include "protocol/GatewaySessionHandler.hpp"
#include "service/AdminRegistry.hpp"
#include "service/DbExecutor.hpp"
#include "service/DeviceRegistry.hpp"
#include "service/EventSubscriptionRegistry.hpp"
#include "db/PersonRepository.hpp"
#include "db/RoleRepository.hpp"
#include "db/UserAccountRepository.hpp"
#include "util/ArcFaceEngine.hpp"
#include "util/PasswordHash.hpp"

namespace {

void createSuperAdminIfPending(mysqlx::Session& session,
                               const std::string& schema,
                               config::ServerConfig& cfg) {
  if (!cfg.pending_super_admin.has_value()) {
    return;
  }

  auto& admin = *cfg.pending_super_admin;

  // 检查是否已有超级管理员
  int existing = db::RoleRepository::countUsersWithRole(session, "super_admin");
  if (existing > 0) {
    std::cout << "超级管理员已存在（" << existing << " 个），跳过创建。"
              << std::endl;
    cfg.pending_super_admin.reset();
    return;
  }

  try {
    session.sql("USE " + schema).execute();

    // 1. 确保 Person 记录存在（INSERT IGNORE）
    session.sql("START TRANSACTION").execute();
    session
        .sql("INSERT IGNORE INTO Person (name, employee_id, department, "
             "position) VALUES (?, ?, '', '')")
        .bind(admin.name, admin.employee_id)
        .execute();

    // 2. 创建 UserAccount
    std::string hashed = util::hash_password(admin.password);
    db::UserAccountCreateInput user_input;
    user_input.employee_id = admin.employee_id;
    user_input.name = admin.name;
    user_input.password = hashed;
    db::UserAccountRepository::insertUser(session, user_input);

    // 3. 查询 user_id
    db::UserAccountQueryInput query;
    query.employee_id = admin.employee_id;
    auto user_rows = db::UserAccountRepository::selectUser(session, query);
    if (user_rows.empty()) {
      throw std::runtime_error("创建 UserAccount 后查询失败");
    }
    int user_id = user_rows[0].id;

    // 4. 分配 super_admin 角色
    db::RoleRepository::assignUserRole(session, user_id, "super_admin");

    session.sql("COMMIT").execute();
    std::cout << "超级管理员创建成功！" << std::endl;
    std::cout << "  工号: " << admin.employee_id << std::endl;
    std::cout << "  姓名: " << admin.name << std::endl;
  } catch (const mysqlx::Error& err) {
    try { session.sql("ROLLBACK").execute(); } catch (...) {}
    std::cerr << "超级管理员创建失败: " << err.what() << std::endl;
    std::cerr << "服务将继续启动。" << std::endl;
  } catch (const std::exception& ex) {
    try { session.sql("ROLLBACK").execute(); } catch (...) {}
    std::cerr << "超级管理员创建失败: " << ex.what() << std::endl;
    std::cerr << "服务将继续启动。" << std::endl;
  }

  // 清空明文密码
  cfg.pending_super_admin.reset();
}

}  // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif

  config::ServerConfig cfg;
  const std::string cfg_err = config::loadServerConfig(argc, argv, cfg);
  if (cfg_err == "help") {
    return 0;
  }
  if (!cfg_err.empty()) {
    std::cerr << cfg_err << std::endl;
    return 1;
  }

  const std::string config_path = config::resolveConfigPath(argc, argv);

  if (config::isInteractiveTerminal()) {
    if (!config::runConsoleConfigUi(cfg, config_path)) {
      return 0;
    }
  }

  config::logConfigSummary(cfg);

  if (cfg.arcface.enabled) {
    util::ArcFaceEngine& engine = util::ArcFaceEngine::instance();
    if (!engine.initialize(cfg.arcface.app_id, cfg.arcface.sdk_key)) {
      std::cerr << "警告: ArcFace 引擎初始化失败，人脸注册功能不可用；"
                   "服务将继续启动（可用 --no-arcface 跳过尝试）。"
                << std::endl;
    } else {
      std::cout << "ArcFace 引擎已就绪" << std::endl;
    }
  }

  db::DbClient client(cfg.db);

  try {
    client.connect();
    db::Schema::ensure_tables(client.session(), cfg.db.schema);
    db::Schema::ensure_rbac_seed_data(client.session(), cfg.db.schema);
    createSuperAdminIfPending(client.session(), cfg.db.schema, cfg);

    std::cout << "MySQL 连接成功！" << std::endl;
    std::cout << "URL: " << client.connection_url() << std::endl;
    std::cout << "Ping: " << (client.ping() ? "ok" : "failed") << std::endl;

    if (cfg.enable_tcp) {
      service::DeviceRegistry device_registry;
      service::AdminRegistry admin_registry;
      service::EventSubscriptionRegistry subscriptions;
      service::DbExecutor db_executor(cfg.db, cfg.db_pool_threads);

      protocol::GatewaySessionHandler gateway_handler(
          device_registry, db_executor, cfg.gateway, admin_registry,
          subscriptions);

      net::TcpServer tcp_server(config::makeTcpServerConfig(cfg),
                                gateway_handler);
      tcp_server.start();
      config::logFramingLimitsSummary();

      std::cout << "TCP 监听 " << cfg.tcp_bind << ":" << cfg.tcp_port
                << "（输入 quit 停止服务）" << std::endl;
      for (;;) {
        std::cout << "> ";
        std::cout.flush();
        std::string line;
        if (!std::getline(std::cin, line)) {
          break;
        }
        if (line == "quit" || line == "exit" || line == "q") {
          break;
        }
        if (!line.empty()) {
          std::cout << "未知命令，输入 quit 停止服务。" << std::endl;
        }
      }
      tcp_server.stop();
      db_executor.stop();
    }

    client.disconnect();
    return 0;
  } catch (const mysqlx::Error& err) {
    std::cerr << "MySQL 错误！" << std::endl;
    std::cerr << "错误: " << err.what() << std::endl;
    return 1;
  } catch (const std::exception& ex) {
    std::cerr << "运行时异常: " << ex.what() << std::endl;
    return 2;
  }
}
