#include "db/RoleRepository.hpp"

#include <stdexcept>

#include "protocol/AppError.hpp"

namespace db {

static void useDb(mysqlx::Session& session) {
  session.sql("USE attendanceserver").execute();
}

// ── Auth-time queries ──

std::vector<std::string> RoleRepository::loadUserRoles(
    mysqlx::Session& session, int user_id) {
  useDb(session);
  auto result =
      session
          .sql("SELECT r.role_key FROM UserRole ur "
               "JOIN Role r ON ur.role_id = r.id "
               "WHERE ur.user_id = ?")
          .bind(user_id)
          .execute();

  std::vector<std::string> roles;
  for (auto row : result.fetchAll()) {
    roles.push_back(row[0].get<std::string>());
  }
  return roles;
}

std::unordered_set<std::string> RoleRepository::loadUserPermissions(
    mysqlx::Session& session, int user_id) {
  useDb(session);
  auto result =
      session
          .sql("SELECT DISTINCT p.perm_key FROM UserRole ur "
               "JOIN RolePermission rp ON ur.role_id = rp.role_id "
               "JOIN Permission p ON rp.permission_id = p.id "
               "WHERE ur.user_id = ?")
          .bind(user_id)
          .execute();

  std::unordered_set<std::string> perms;
  for (auto row : result.fetchAll()) {
    perms.insert(row[0].get<std::string>());
  }
  return perms;
}

// ── Role CRUD ──

void RoleRepository::insertRole(mysqlx::Session& session,
                                const RoleCreateInput& input) {
  if (input.role_key.empty() || input.role_name.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "role_key and role_name must not be empty");
  }
  useDb(session);
  session
      .sql("INSERT INTO Role (role_key, role_name, description, is_system) "
           "VALUES (?, ?, ?, 0)")
      .bind(input.role_key, input.role_name, input.description)
      .execute();
}

void RoleRepository::updateRole(mysqlx::Session& session,
                                const std::string& role_key,
                                const RoleUpdateInput& input) {
  if (role_key.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "role_key must not be empty");
  }
  useDb(session);
  std::string sql = "UPDATE Role SET ";
  bool first = true;
  auto append = [&](const char* clause) {
    if (!first) sql += ", ";
    sql += clause;
    first = false;
  };
  if (!input.role_name.empty()) append("role_name = ?");
  if (!input.description.empty()) append("description = ?");
  if (first) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "at least one field must be non-empty");
  }
  sql += " WHERE role_key = ? AND is_system = 0";

  auto stmt = session.sql(sql);
  if (!input.role_name.empty()) stmt.bind(input.role_name);
  if (!input.description.empty()) stmt.bind(input.description);
  stmt.bind(role_key);
  stmt.execute();
}

void RoleRepository::deleteRole(mysqlx::Session& session,
                                const std::string& role_key) {
  if (role_key.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "role_key must not be empty");
  }
  useDb(session);
  session.sql("DELETE FROM Role WHERE role_key = ? AND is_system = 0")
      .bind(role_key)
      .execute();
}

std::vector<RoleRecord> RoleRepository::selectRoles(mysqlx::Session& session) {
  useDb(session);
  auto result =
      session
          .sql("SELECT id, role_key, role_name, IFNULL(description, ''), "
               "is_system, "
               "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s'), "
               "DATE_FORMAT(updated_at, '%Y-%m-%d %H:%i:%s') "
               "FROM Role ORDER BY id")
          .execute();

  std::vector<RoleRecord> records;
  for (auto row : result.fetchAll()) {
    RoleRecord r;
    r.id = static_cast<int>(row[0].get<int64_t>());
    r.role_key = row[1].get<std::string>();
    r.role_name = row[2].get<std::string>();
    r.description = row[3].get<std::string>();
    r.is_system = row[4].get<int64_t>() != 0;
    r.created_at = row[5].get<std::string>();
    r.updated_at = row[6].get<std::string>();
    records.push_back(std::move(r));
  }
  return records;
}

// ── UserRole operations ──

void RoleRepository::assignUserRole(mysqlx::Session& session, int user_id,
                                    const std::string& role_key) {
  useDb(session);
  session
      .sql("INSERT IGNORE INTO UserRole (user_id, role_id) "
           "SELECT ?, id FROM Role WHERE role_key = ?")
      .bind(user_id, role_key)
      .execute();
}

void RoleRepository::revokeUserRole(mysqlx::Session& session, int user_id,
                                    const std::string& role_key) {
  useDb(session);
  session
      .sql("DELETE ur FROM UserRole ur "
           "JOIN Role r ON ur.role_id = r.id "
           "WHERE ur.user_id = ? AND r.role_key = ?")
      .bind(user_id, role_key)
      .execute();
}

std::vector<std::string> RoleRepository::queryUserRoles(
    mysqlx::Session& session, int user_id) {
  return loadUserRoles(session, user_id);
}

// ── RolePermission bulk set ──

void RoleRepository::setRolePermissions(
    mysqlx::Session& session, const std::string& role_key,
    const std::vector<std::string>& perm_keys) {
  useDb(session);

  auto res = session.sql("SELECT id FROM Role WHERE role_key = ?")
                 .bind(role_key)
                 .execute();
  auto row = res.fetchOne();
  if (!row) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "role not found: " + role_key);
  }
  int role_id = static_cast<int>(row[0].get<int64_t>());

  session.sql("DELETE FROM RolePermission WHERE role_id = ?")
      .bind(role_id)
      .execute();

  for (const auto& pk : perm_keys) {
    session
        .sql("INSERT IGNORE INTO RolePermission (role_id, permission_id) "
             "SELECT ?, id FROM Permission WHERE perm_key = ?")
        .bind(role_id, pk)
        .execute();
  }
}

// ── Permission listing ──

std::vector<PermissionRecord> RoleRepository::queryAllPermissions(
    mysqlx::Session& session) {
  useDb(session);
  auto result =
      session
          .sql("SELECT id, perm_key, perm_name, resource, "
               "IFNULL(description, '') FROM Permission ORDER BY id")
          .execute();

  std::vector<PermissionRecord> records;
  for (auto row : result.fetchAll()) {
    PermissionRecord r;
    r.id = static_cast<int>(row[0].get<int64_t>());
    r.perm_key = row[1].get<std::string>();
    r.perm_name = row[2].get<std::string>();
    r.resource = row[3].get<std::string>();
    r.description = row[4].get<std::string>();
    records.push_back(std::move(r));
  }
  return records;
}

std::vector<std::string> RoleRepository::getRolePermissionKeys(
    mysqlx::Session& session, const std::string& role_key) {
  useDb(session);
  auto result =
      session
          .sql("SELECT p.perm_key FROM RolePermission rp "
               "JOIN Permission p ON rp.permission_id = p.id "
               "JOIN Role r ON rp.role_id = r.id "
               "WHERE r.role_key = ? ORDER BY p.perm_key")
          .bind(role_key)
          .execute();
  std::vector<std::string> keys;
  for (auto row : result.fetchAll()) {
    keys.push_back(row[0].get<std::string>());
  }
  return keys;
}

// ── Utility ──

int RoleRepository::countUsersWithRole(mysqlx::Session& session,
                                       const std::string& role_key) {
  useDb(session);
  auto result =
      session
          .sql("SELECT COUNT(*) FROM UserRole ur "
               "JOIN Role r ON ur.role_id = r.id "
               "WHERE r.role_key = ?")
          .bind(role_key)
          .execute();
  auto row = result.fetchOne();
  return row ? static_cast<int>(row[0].get<int64_t>()) : 0;
}

}  // namespace db
