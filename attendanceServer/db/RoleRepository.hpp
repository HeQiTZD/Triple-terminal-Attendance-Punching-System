#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include <mysqlx/xdevapi.h>

namespace db {

struct RoleRecord {
  int id = 0;
  std::string role_key;
  std::string role_name;
  std::string description;
  bool is_system = false;
  std::string created_at;
  std::string updated_at;
};

struct PermissionRecord {
  int id = 0;
  std::string perm_key;
  std::string perm_name;
  std::string resource;
  std::string description;
};

struct RoleCreateInput {
  std::string role_key;
  std::string role_name;
  std::string description;
};

struct RoleUpdateInput {
  std::string role_name;
  std::string description;
};

class RoleRepository {
 public:
  // Load all role_key values for a given user (by UserAccount.id).
  static std::vector<std::string> loadUserRoles(mysqlx::Session& session,
                                                int user_id);

  // Load the aggregated perm_key set across all roles for a given user.
  static std::unordered_set<std::string> loadUserPermissions(
      mysqlx::Session& session, int user_id);

  // Role CRUD
  static void insertRole(mysqlx::Session& session,
                         const RoleCreateInput& input);
  static void updateRole(mysqlx::Session& session, const std::string& role_key,
                         const RoleUpdateInput& input);
  static void deleteRole(mysqlx::Session& session, const std::string& role_key);
  static std::vector<RoleRecord> selectRoles(mysqlx::Session& session);

  // UserRole operations
  static void assignUserRole(mysqlx::Session& session, int user_id,
                             const std::string& role_key);
  static void revokeUserRole(mysqlx::Session& session, int user_id,
                             const std::string& role_key);
  static std::vector<std::string> queryUserRoles(mysqlx::Session& session,
                                                 int user_id);

  // RolePermission operations
  static void setRolePermissions(mysqlx::Session& session,
                                 const std::string& role_key,
                                 const std::vector<std::string>& perm_keys);

  static std::vector<std::string> getRolePermissionKeys(
      mysqlx::Session& session, const std::string& role_key);

  // Permission listing
  static std::vector<PermissionRecord> queryAllPermissions(
      mysqlx::Session& session);

  // Count users who hold a specific role
  static int countUsersWithRole(mysqlx::Session& session,
                                const std::string& role_key);
};

}  // namespace db
