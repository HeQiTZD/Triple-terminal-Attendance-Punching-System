#include "UserAccountRepository.hpp"

#include <stdexcept>

#include "protocol/AppError.hpp"

namespace db {

static void requireNonEmpty(const char* fieldName, const std::string& value) {
  if (value.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, std::string(fieldName) + " must not be empty");
  }
}

void UserAccountRepository::insertUser(mysqlx::Session& session,
                                       const UserAccountCreateInput& input) {
  requireNonEmpty("employee_id", input.employee_id);
  requireNonEmpty("password", input.password);

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  // name 可为 NULL：根据是否传值选择不同的 INSERT 语句，避免显式插入空字符串
  // 与“未提供”混淆。也可以用 NULLIF(?, '')，这里保持直观。
  if (input.name.empty()) {
    session
        .sql(
            "INSERT INTO UserAccount (employee_id, password) "
            "VALUES (?, ?)")
        .bind(input.employee_id, input.password)
        .execute();
  } else {
    session
        .sql(
            "INSERT INTO UserAccount (employee_id, name, password) "
            "VALUES (?, ?, ?)")
        .bind(input.employee_id, input.name, input.password)
        .execute();
  }
}

void UserAccountRepository::deleteUser(mysqlx::Session& session,
                                       const std::string& employee_id) {
  requireNonEmpty("employee_id", employee_id);

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  // 仅删除 UserAccount 表中的行；schema 的 ON DELETE CASCADE 是 Person→UserAccount
  // 单向级联，UserAccount 这端的 DELETE 不会反向影响 Person，不需要事务。
  session.sql("DELETE FROM UserAccount WHERE employee_id = ?")
      .bind(employee_id)
      .execute();
}

void UserAccountRepository::updateUser(mysqlx::Session& session,
                                       const std::string& employee_id,
                                       const UserAccountUpdateInput& input) {
  requireNonEmpty("employee_id", employee_id);

  const bool updateName = !input.name.empty();
  const bool updatePassword = !input.password.empty();

  if (!updateName && !updatePassword) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "at least one field must be non-empty");
  }

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  // 动态拼接 SET 子句，仅包含传入的非空字段；UserAccount 表无 updated_at 列，无需追加。
  std::string sql = "UPDATE UserAccount SET ";
  bool first = true;

  auto appendSet = [&](const char* clause) {
    if (!first) sql += ", ";
    sql += clause;
    first = false;
  };

  if (updateName) appendSet("name = ?");
  if (updatePassword) appendSet("password = ?");

  sql += " WHERE employee_id = ?";

  auto stmt = session.sql(sql);
  if (updateName) stmt.bind(input.name);
  if (updatePassword) stmt.bind(input.password);
  stmt.bind(employee_id);
  stmt.execute();
}

std::vector<UserAccountRecord> UserAccountRepository::selectUser(
    mysqlx::Session& session, const UserAccountQueryInput& input) {
  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  const bool filterEmployeeId = !input.employee_id.empty();
  const bool filterCreatedAt = !input.created_at.empty();

  // 用 IFNULL 把可空列折叠为空串，方便后续无脑转 std::string；
  // 时间列用 DATE_FORMAT 直接转成字符串，避免依赖 mysqlx 对 DATETIME 的隐式转换；
  // last_login_time 可空，再用 IFNULL 把"未登录过"折叠成空串。
  std::string sql =
      "SELECT id, employee_id, IFNULL(name, ''), password, "
      "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s'), "
      "IFNULL(DATE_FORMAT(last_login_time, '%Y-%m-%d %H:%i:%s'), '') "
      "FROM UserAccount";

  bool first = true;
  auto appendCond = [&](const char* clause) {
    sql += first ? " WHERE " : " AND ";
    sql += clause;
    first = false;
  };

  if (filterEmployeeId) appendCond("employee_id = ?");
  // created_at 使用 DATE_FORMAT 后做前缀 LIKE 匹配：
  // 既能精确匹配完整时间串，也能用 'YYYY-MM-DD' 命中整天、'YYYY-MM' 命中整月。
  if (filterCreatedAt) {
    appendCond(
        "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s') LIKE CONCAT(?, '%')");
  }

  sql += " ORDER BY id";

  auto stmt = session.sql(sql);
  if (filterEmployeeId) stmt.bind(input.employee_id);
  if (filterCreatedAt) stmt.bind(input.created_at);

  mysqlx::SqlResult result = stmt.execute();

  std::vector<UserAccountRecord> records;
  for (mysqlx::Row row : result.fetchAll()) {
    UserAccountRecord r;
    r.id = static_cast<int>(row[0].get<int64_t>());
    r.employee_id = row[1].get<std::string>();
    r.name = row[2].get<std::string>();
    r.password = row[3].get<std::string>();
    r.created_at = row[4].get<std::string>();
    r.last_login_time = row[5].get<std::string>();
    records.push_back(std::move(r));
  }
  return records;
}

}  // namespace db
