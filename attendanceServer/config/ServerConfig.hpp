#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include "db/DbConfig.hpp"
#include "net/TcpServer.hpp"
#include "protocol/GatewayConfig.hpp"

namespace config {

struct ArcFaceConfig {
    std::string app_id;
    std::string sdk_key;
    bool enabled = false;
    int min_face_size = 80;
};

struct PendingSuperAdmin {
  std::string employee_id;
  std::string name;
  std::string password;  // 明文，仅在内存中短暂持有，不序列化到 JSON
};

// 单一进程配置：默认值 → 环境变量 ATTENDANCE_* → 命令行（后者覆盖前者）。
// 环境变量与 CLI 说明见 ServerConfig.cpp 顶部注释。
struct ServerConfig {
  db::DbConfig db;
  bool enable_tcp = false;
  std::string tcp_bind = "0.0.0.0";
  unsigned short tcp_port = 8080;
  std::size_t tcp_worker_threads = 0;
  std::size_t db_pool_threads = 0;
  protocol::GatewayConfig gateway;
  ArcFaceConfig arcface;
  std::optional<PendingSuperAdmin> pending_super_admin;  // 运行时临时字段，不序列化
};




// 内置默认配置（结构体成员初始值与 attendance.json.example 一致）。
ServerConfig defaultServerConfig();

// 将空字符串等无效项恢复为默认值（JSON/环境变量合并后调用）。
void applyConfigDefaults(ServerConfig& cfg);

// 成功返回空字符串；否则返回 UTF-8 错误说明。
std::string loadServerConfig(int argc, char** argv, ServerConfig& out);

void logConfigSummary(const ServerConfig& cfg);

net::TcpServer::Config makeTcpServerConfig(const ServerConfig& cfg);

// 打印 Framing 限额（与通信协议 §9.2 / net/Framing.hpp 一致）。
void logFramingLimitsSummary();

}  // namespace config
