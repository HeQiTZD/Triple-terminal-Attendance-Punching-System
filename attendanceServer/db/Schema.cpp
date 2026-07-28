#include "Schema.hpp"

#include <sstream>
#include <string>

namespace db::Schema {

namespace {

std::string quote_schema_ident(const std::string& schema) {
  std::ostringstream os;
  os << '`';
  for (char c : schema) {
    if (c == '`') {
      os << "``";
    } else {
      os << c;
    }
  }
  os << '`';
  return os.str();
}

}  // namespace

void ensure_tables(mysqlx::Session& session, const std::string& schema) {
  // Defensive: select schema explicitly even if URL already included it.
  session.sql("USE " + quote_schema_ident(schema)).execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS Person (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    employee_id VARCHAR(100) UNIQUE NOT NULL,
    department VARCHAR(100),
    position VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS UserAccount (
    id INT AUTO_INCREMENT PRIMARY KEY,
    employee_id VARCHAR(100) NOT NULL,
    name VARCHAR(100),
    password VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login_time TIMESTAMP NULL DEFAULT NULL,
    UNIQUE KEY uq_user_employee_id (employee_id),
    FOREIGN KEY (employee_id) REFERENCES Person(employee_id) ON DELETE CASCADE
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS Device (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) UNIQUE NOT NULL,
    device_name VARCHAR(100),
    ip_address VARCHAR(50),
    last_online TIMESTAMP,
    status VARCHAR(20) DEFAULT 'offline',
    key_hash VARCHAR(255) NULL
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS device_config_deploy (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    config_version VARCHAR(64) NOT NULL,
    config_content TEXT NOT NULL,
    config_hash VARCHAR(80) NOT NULL,
    description VARCHAR(500),
    created_by VARCHAR(64) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_version (config_version),
    INDEX idx_created_at (created_at)
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS device_config_target (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    deploy_id BIGINT NOT NULL,
    device_id VARCHAR(64) NOT NULL,
    forward_msg_id VARCHAR(128) NULL,
    status ENUM('pending', 'sent', 'acknowledged', 'applied', 'failed') DEFAULT 'pending',
    error_message VARCHAR(500),
    sent_at TIMESTAMP NULL DEFAULT NULL,
    ack_at TIMESTAMP NULL DEFAULT NULL,
    applied_at TIMESTAMP NULL DEFAULT NULL,
    FOREIGN KEY (deploy_id) REFERENCES device_config_deploy(id) ON DELETE CASCADE,
    INDEX idx_deploy_device (deploy_id, device_id),
    INDEX idx_device_status (device_id, status),
    INDEX idx_forward_msg_id (forward_msg_id)
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS device_config_current (
    device_id VARCHAR(64) PRIMARY KEY,
    config_version VARCHAR(64) NOT NULL,
    config_hash VARCHAR(80) NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS AttendanceRecord (
    id INT AUTO_INCREMENT PRIMARY KEY,
    employee_id VARCHAR(100) NOT NULL,
    check_time TIMESTAMP NOT NULL,
    device_id VARCHAR(50),
    status VARCHAR(20),
    photo LONGBLOB NULL,
    received_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_employee_id (employee_id),
    FOREIGN KEY (employee_id) REFERENCES Person(employee_id)
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS AttendanceRecordArchive (
    id INT AUTO_INCREMENT PRIMARY KEY,
    employee_id VARCHAR(100) NOT NULL,
    name        VARCHAR(100),
    department  VARCHAR(100),
    position    VARCHAR(100),
    check_time     TIMESTAMP NOT NULL,
    device_id      VARCHAR(50),
    status         VARCHAR(20),
    received_time  TIMESTAMP NULL DEFAULT NULL,
    archived_at    TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    archive_reason VARCHAR(50) DEFAULT 'resigned',
    INDEX idx_employee_id (employee_id),
    INDEX idx_check_time  (check_time)
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS face_data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    employee_id VARCHAR(100) NOT NULL UNIQUE,
    feature_vector LONGBLOB NOT NULL,
    feature_size INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (employee_id) REFERENCES Person(employee_id) ON DELETE CASCADE
);
)sql")
      .execute();

  // ── RBAC tables ──

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS Role (
    id INT AUTO_INCREMENT PRIMARY KEY,
    role_key VARCHAR(50) UNIQUE NOT NULL,
    role_name VARCHAR(100) NOT NULL,
    description VARCHAR(255),
    is_system TINYINT(1) DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS Permission (
    id INT AUTO_INCREMENT PRIMARY KEY,
    perm_key VARCHAR(100) UNIQUE NOT NULL,
    perm_name VARCHAR(100) NOT NULL,
    resource VARCHAR(50) NOT NULL,
    description VARCHAR(255)
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS RolePermission (
    role_id INT NOT NULL,
    permission_id INT NOT NULL,
    PRIMARY KEY (role_id, permission_id),
    FOREIGN KEY (role_id) REFERENCES Role(id) ON DELETE CASCADE,
    FOREIGN KEY (permission_id) REFERENCES Permission(id) ON DELETE CASCADE
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS UserRole (
    user_id INT NOT NULL,
    role_id INT NOT NULL,
    PRIMARY KEY (user_id, role_id),
    FOREIGN KEY (user_id) REFERENCES UserAccount(id) ON DELETE CASCADE,
    FOREIGN KEY (role_id) REFERENCES Role(id) ON DELETE CASCADE
);
)sql")
      .execute();

  session
      .sql(R"sql(
CREATE TABLE IF NOT EXISTS OperationLog (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    user_id INT,
    employee_id VARCHAR(100),
    action VARCHAR(100) NOT NULL,
    target_type VARCHAR(50),
    target_id VARCHAR(100),
    detail JSON,
    ip_address VARCHAR(50),
    result VARCHAR(20) DEFAULT 'success',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_oplog_user_id (user_id),
    INDEX idx_oplog_action (action),
    INDEX idx_oplog_created_at (created_at)
);
)sql")
      .execute();

  // Add login-lockout columns to UserAccount (idempotent: ignores error if
  // column already exists).
  auto try_alter = [&](const char* sql) {
    try {
      session.sql(sql).execute();
    } catch (const mysqlx::Error&) {
      // Column likely already exists; safe to ignore.
    }
  };
  try_alter("ALTER TABLE UserAccount ADD COLUMN failed_login_count INT "
            "NOT NULL DEFAULT 0");
  try_alter("ALTER TABLE UserAccount ADD COLUMN locked_until TIMESTAMP "
            "NULL DEFAULT NULL");
  try_alter("ALTER TABLE device_config_target ADD COLUMN forward_msg_id "
            "VARCHAR(128) NULL");
  try_alter("ALTER TABLE device_config_target ADD INDEX idx_forward_msg_id "
            "(forward_msg_id)");
  try_alter("ALTER TABLE Device ADD COLUMN key_hash VARCHAR(255) NULL");
}

// ── Seed data for RBAC ──

namespace {

struct PermDef {
  const char* key;
  const char* name;
  const char* resource;
};

constexpr PermDef kPermissions[] = {
    {"person.create", "新增员工", "person"},
    {"person.read", "查询员工", "person"},
    {"person.update", "修改员工", "person"},
    {"person.delete", "删除员工", "person"},
    {"attendance.create", "新增考勤", "attendance"},
    {"attendance.read", "查询考勤", "attendance"},
    {"attendance.update", "修改考勤", "attendance"},
    {"attendance.delete", "删除考勤", "attendance"},
    {"attendance.archive.read", "查询归档", "attendance"},
    {"attendance.archive.delete", "删除归档", "attendance"},
    {"device.create", "注册设备", "device"},
    {"device.read", "查询设备", "device"},
    {"device.update", "修改设备", "device"},
    {"device.delete", "删除设备", "device"},
    {"device.command", "设备指令", "device"},
    {"config.deploy", "配置下发", "config"},
    {"user.create", "创建账户", "user"},
    {"user.read", "查询账户", "user"},
    {"user.update", "修改账户", "user"},
    {"user.delete", "删除账户", "user"},
    {"face.register", "注册人脸", "face"},
    {"face.read", "查询人脸", "face"},
    {"face.delete", "删除人脸", "face"},
    {"event.subscribe", "订阅推送", "event"},
};

struct RoleDef {
  const char* key;
  const char* name;
  const char* desc;
};

constexpr RoleDef kRoles[] = {
    {"super_admin", "超级管理员", "拥有系统全部权限"},
    {"hr_admin", "人事管理员", "管理员工信息与考勤数据"},
    {"device_admin", "设备管理员", "管理考勤设备"},
    {"attendance_auditor", "考勤审核员", "查看/审核考勤数据"},
    {"viewer", "只读观察者", "仅可查看数据"},
};

// role_key -> array of perm_keys
struct RolePermMap {
  const char* role;
  const char* perms[25];  // NULL-terminated
};

constexpr RolePermMap kRolePerms[] = {
    {"super_admin",
     {"person.create", "person.read", "person.update", "person.delete",
      "attendance.create", "attendance.read", "attendance.update",
      "attendance.delete", "attendance.archive.read",
      "attendance.archive.delete", "device.create", "device.read",
      "device.update", "device.delete", "device.command", "user.create",
      "user.read", "user.update", "user.delete", "face.register", "face.read",
      "face.delete", "event.subscribe", "config.deploy", nullptr}},
    {"hr_admin",
     {"person.create", "person.read", "person.update", "person.delete",
      "attendance.create", "attendance.read", "attendance.update",
      "attendance.delete", "attendance.archive.read",
      "attendance.archive.delete", "face.register", "face.read", "face.delete",
      "event.subscribe", nullptr}},
    {"device_admin",
     {"device.create", "device.read", "device.update", "device.delete",
      "device.command", "config.deploy", "event.subscribe", nullptr}},
    {"attendance_auditor",
     {"person.read", "attendance.read", "attendance.update",
      "attendance.archive.read", "event.subscribe", nullptr}},
    {"viewer",
     {"person.read", "attendance.read", "attendance.archive.read",
      "device.read", "face.read", nullptr}},
};

}  // namespace

void ensure_rbac_seed_data(mysqlx::Session& session, const std::string& schema) {
  session.sql("USE " + quote_schema_ident(schema)).execute();

  // 1. Seed permissions (idempotent)
  for (const auto& p : kPermissions) {
    session
        .sql("INSERT IGNORE INTO Permission (perm_key, perm_name, resource) "
             "VALUES (?, ?, ?)")
        .bind(p.key, p.name, p.resource)
        .execute();
  }

  // 2. Seed roles (idempotent)
  for (const auto& r : kRoles) {
    session
        .sql("INSERT IGNORE INTO Role (role_key, role_name, description, "
             "is_system) VALUES (?, ?, ?, 1)")
        .bind(r.key, r.name, r.desc)
        .execute();
  }

  // 3. Seed role-permission mappings
  for (const auto& rm : kRolePerms) {
    auto res = session.sql("SELECT id FROM Role WHERE role_key = ?")
                   .bind(rm.role)
                   .execute();
    auto row = res.fetchOne();
    if (!row) continue;
    int role_id = static_cast<int>(row[0].get<int64_t>());

    for (int i = 0; rm.perms[i] != nullptr; ++i) {
      session
          .sql("INSERT IGNORE INTO RolePermission (role_id, permission_id) "
               "SELECT ?, id FROM Permission WHERE perm_key = ?")
          .bind(role_id, rm.perms[i])
          .execute();
    }
  }

  // 4. Migrate legacy users: assign super_admin only when the account has
  //    no role at all (NOT when it merely lacks super_admin).
  session
      .sql(R"sql(
INSERT IGNORE INTO UserRole (user_id, role_id)
SELECT ua.id, r.id
FROM UserAccount ua
CROSS JOIN Role r
WHERE r.role_key = 'super_admin'
  AND NOT EXISTS (
    SELECT 1 FROM UserRole ur WHERE ur.user_id = ua.id
  )
)sql")
      .execute();

  // 5. Repair mistaken dual-role rows from older migrations: drop super_admin
  //    when the user already has another role. (Use derived table: MySQL forbids
  //    referencing the DELETE target inside a subquery on the same table.)
  session
      .sql(R"sql(
DELETE ur FROM UserRole ur
INNER JOIN Role r_sa ON ur.role_id = r_sa.id AND r_sa.role_key = 'super_admin'
INNER JOIN (
  SELECT DISTINCT ur2.user_id AS user_id
  FROM UserRole ur2
  INNER JOIN Role r2 ON ur2.role_id = r2.id
  WHERE r2.role_key <> 'super_admin'
) other_roles ON other_roles.user_id = ur.user_id
)sql")
      .execute();
}

}  // namespace db::Schema

