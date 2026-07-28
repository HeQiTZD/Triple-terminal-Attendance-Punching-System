#pragma once

#include <optional>
#include <string>

#include <mysqlx/xdevapi.h>

#include "DbConfig.hpp"

namespace db {

class DbClient {
 public:
  explicit DbClient(DbConfig config);

  // mysqlx://user:password@host:port[/schema] — 与成员 connection_url() 一致。
  static std::string connection_url(const DbConfig& cfg);

  // Establishes a new session. Throws mysqlx::Error on failure.
  void connect();
  void disconnect() noexcept;

  bool is_connected() const noexcept;

  // Lightweight check. Returns false if not connected or if query fails.
  bool ping() noexcept;

  // Exposed for services that need raw session access.
  mysqlx::Session& session();
  const mysqlx::Session& session() const;

  std::string connection_url() const { return DbClient::connection_url(config_); }

 private:
  DbConfig config_;
  std::optional<mysqlx::Session> session_;
};

}  // namespace db

