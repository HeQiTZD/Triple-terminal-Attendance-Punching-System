#pragma once

#include <mysqlx/xdevapi.h>

#include <string>

namespace db::Schema {

// Creates tables if they don't exist (idempotent).
// Does NOT drop or migrate existing tables.
// schema: 须已由调用方校验为安全标识符（字母数字下划线）。
void ensure_tables(mysqlx::Session& session, const std::string& schema);

// Seeds RBAC data (permissions, roles, role-permission mappings).
// Idempotent: uses INSERT IGNORE. Call after ensure_tables.
void ensure_rbac_seed_data(mysqlx::Session& session, const std::string& schema);

}  // namespace db::Schema

