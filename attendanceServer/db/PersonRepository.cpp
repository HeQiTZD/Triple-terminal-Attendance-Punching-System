#include "PersonRepository.hpp"
#include "AttendanceRecordRepository.hpp"

#include <stdexcept>

#include "protocol/AppError.hpp"

namespace db {

static void requireNonEmpty(const char* fieldName, const std::string& value) {
  if (value.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, std::string(fieldName) + " must not be empty");
  }
}

void PersonRepository::insert(mysqlx::Session& session,
                              const PersonCreateInput& input) {
  requireNonEmpty("name", input.name);
  requireNonEmpty("employee_id", input.employee_id);
  requireNonEmpty("department", input.department);
  requireNonEmpty("position", input.position);

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  // 使用参数绑定，避免 SQL 注入。
  session
      .sql(
          "INSERT INTO Person (name, employee_id, department, position) "
          "VALUES (?, ?, ?, ?)")
      .bind(input.name, input.employee_id, input.department, input.position)
      .execute();
}

void deletePerson(mysqlx::Session& session, const std::string& employee_id) {
  requireNonEmpty("employee_id", employee_id);

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  // 先将该员工的打卡记录归档至 AttendanceRecordArchive（快照 name/department/position），
  // 再删除 AttendanceRecord 中的活动记录，最后删除 Person（会级联删除 UserAccount/face_data）。
  session.sql("START TRANSACTION").execute();
  try {
    AttendanceRecordRepository::insertArchive(session, employee_id,
                                               "employee_deleted");

    session.sql("DELETE FROM AttendanceRecord WHERE employee_id = ?")
        .bind(employee_id)
        .execute();

    session.sql("DELETE FROM Person WHERE employee_id = ?")
        .bind(employee_id)
        .execute();

    session.sql("COMMIT").execute();
  } catch (...) {
    try {
      session.sql("ROLLBACK").execute();
    } catch (...) {
      // best-effort rollback
    }
    throw;
  }
}

std::vector<PersonRecord> PersonRepository::selectPerson(
    mysqlx::Session& session, const PersonQueryInput& input) {
  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  const bool filterName = !input.name.empty();
  const bool filterEmployeeId = !input.employee_id.empty();
  const bool filterDepartment = !input.department.empty();
  const bool filterPosition = !input.position.empty();
  const bool filterCreatedAt = !input.created_at.empty();
  const bool filterUpdatedAt = !input.updated_at.empty();

  // 使用 IFNULL 把可空列折叠为空串，方便后续无脑转 std::string；
  // 时间列用 DATE_FORMAT 直接转成字符串，避免依赖 mysqlx 对 DATETIME 的隐式转换。
  std::string sql =
      "SELECT id, name, employee_id, "
      "IFNULL(department, ''), IFNULL(position, ''), "
      "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s'), "
      "DATE_FORMAT(updated_at, '%Y-%m-%d %H:%i:%s') "
      "FROM Person";

  bool first = true;
  auto appendCond = [&](const char* clause) {
    sql += first ? " WHERE " : " AND ";
    sql += clause;
    first = false;
  };

  if (filterName) appendCond("name = ?");
  if (filterEmployeeId) appendCond("employee_id = ?");
  if (filterDepartment) appendCond("department = ?");
  if (filterPosition) appendCond("position = ?");
  // created_at / updated_at 使用 DATE_FORMAT 后做前缀 LIKE 匹配：
  // 既能精确匹配完整时间串，也能用 'YYYY-MM-DD' 命中整天、'YYYY-MM' 命中整月。
  if (filterCreatedAt) {
    appendCond(
        "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s') LIKE CONCAT(?, '%')");
  }
  if (filterUpdatedAt) {
    appendCond(
        "DATE_FORMAT(updated_at, '%Y-%m-%d %H:%i:%s') LIKE CONCAT(?, '%')");
  }

  sql += " ORDER BY id";

  auto stmt = session.sql(sql);
  if (filterName) stmt.bind(input.name);
  if (filterEmployeeId) stmt.bind(input.employee_id);
  if (filterDepartment) stmt.bind(input.department);
  if (filterPosition) stmt.bind(input.position);
  if (filterCreatedAt) stmt.bind(input.created_at);
  if (filterUpdatedAt) stmt.bind(input.updated_at);

  mysqlx::SqlResult result = stmt.execute();

  std::vector<PersonRecord> records;
  for (mysqlx::Row row : result.fetchAll()) {
    PersonRecord r;
    r.id = static_cast<int>(row[0].get<int64_t>());
    r.name = row[1].get<std::string>();
    r.employee_id = row[2].get<std::string>();
    r.department = row[3].get<std::string>();
    r.position = row[4].get<std::string>();
    r.created_at = row[5].get<std::string>();
    r.updated_at = row[6].get<std::string>();
    records.push_back(std::move(r));
  }
  return records;
}

std::vector<PersonRecord> PersonRepository::listPersonPage(
    mysqlx::Session& session, const PersonPageQuery& query) {
  if (query.limit <= 0) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "limit must be positive");
  }

  session.sql("USE attendanceserver").execute();

  mysqlx::SqlResult result =
      session
          .sql(
              "SELECT id, name, employee_id, "
              "IFNULL(department, ''), IFNULL(position, ''), "
              "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s'), "
              "DATE_FORMAT(updated_at, '%Y-%m-%d %H:%i:%s') "
              "FROM Person WHERE id > ? ORDER BY id ASC LIMIT ?")
          .bind(query.after_id)
          .bind(query.limit)
          .execute();

  std::vector<PersonRecord> records;
  for (mysqlx::Row row : result.fetchAll()) {
    PersonRecord r;
    r.id = static_cast<int>(row[0].get<int64_t>());
    r.name = row[1].get<std::string>();
    r.employee_id = row[2].get<std::string>();
    r.department = row[3].get<std::string>();
    r.position = row[4].get<std::string>();
    r.created_at = row[5].get<std::string>();
    r.updated_at = row[6].get<std::string>();
    records.push_back(std::move(r));
  }
  return records;
}

void PersonRepository::updatePerson(mysqlx::Session& session,
                                    const std::string& employee_id,
                                    const PersonUpdateInput& input) {
  requireNonEmpty("employee_id", employee_id);

  const bool updateName = !input.name.empty();
  const bool updateDepartment = !input.department.empty();
  const bool updatePosition = !input.position.empty();

  if (!updateName && !updateDepartment && !updatePosition) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "at least one field must be non-empty");
  }

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  std::string sql = "UPDATE Person SET ";
  bool first = true;

  auto appendSet = [&](const char* clause) {
    if (!first) sql += ", ";
    sql += clause;
    first = false;
  };

  if (updateName) appendSet("name = ?");
  if (updateDepartment) appendSet("department = ?");
  if (updatePosition) appendSet("position = ?");
  appendSet("updated_at = CURRENT_TIMESTAMP");

  sql += " WHERE employee_id = ?";

  auto stmt = session.sql(sql);
  if (updateName) stmt.bind(input.name);
  if (updateDepartment) stmt.bind(input.department);
  if (updatePosition) stmt.bind(input.position);
  stmt.bind(employee_id);
  stmt.execute();
}

}  // namespace db

