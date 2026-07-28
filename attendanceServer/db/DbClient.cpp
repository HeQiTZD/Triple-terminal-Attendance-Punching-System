#include "DbClient.hpp"

#include <utility>

namespace db {

DbClient::DbClient(DbConfig config) : config_(std::move(config)) {}

std::string DbClient::connection_url(const DbConfig& cfg) {
  std::string url = "mysqlx://" + cfg.user + ":" + cfg.password + "@" + cfg.host +
                    ":" + std::to_string(cfg.port);
  if (!cfg.schema.empty()) {
    url += "/" + cfg.schema;
  }
  return url;
}

void DbClient::connect() {
  disconnect();
  session_.emplace(connection_url());
}

void DbClient::disconnect() noexcept {
  if (session_.has_value()) {
    try {
      session_->close();
    } catch (...) {
      // Best-effort close; keep noexcept.
    }
    session_.reset();
  }
}

bool DbClient::is_connected() const noexcept { return session_.has_value(); }

bool DbClient::ping() noexcept {
  if (!session_.has_value()) return false;
  try {
    session_->sql("SELECT 1").execute();
    return true;
  } catch (...) {
    return false;
  }
}

mysqlx::Session& DbClient::session() { return session_.value(); }
const mysqlx::Session& DbClient::session() const { return session_.value(); }

}  // namespace db

