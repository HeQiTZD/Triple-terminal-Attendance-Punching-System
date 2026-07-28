#include "AttendanceRecordRepository.hpp"

#include <cstdint>
#include <stdexcept>

#include "protocol/AppError.hpp"

#include "util/DebugLog.hpp"

namespace db {

static void requireNonEmpty(const char* fieldName, const std::string& value) {
  if (value.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, std::string(fieldName) + " must not be empty");
  }
}

void AttendanceRecordRepository::insertRecord(
    mysqlx::Session& session, const AttendanceRecordCreateInput& input) {
  DBG_DB("insertRecord 开始: employeeId=" << input.employee_id
         << " checkTime=" << input.check_time
         << " status=" << input.status
         << " deviceId=" << (input.device_id.empty() ? "(null)" : input.device_id)
         << " photoSize=" << input.photo.size() << " bytes");

  if (input.employee_id.empty() || input.check_time.empty() || input.status.empty()) {
    ERR_DB("insertRecord 失败: 必填字段为空"
           << "\n  - employeeId: " << (input.employee_id.empty() ? "✗ (空)" : "✓ (" + input.employee_id + ")")
           << "\n  - checkTime: " << (input.check_time.empty() ? "✗ (空)" : "✓ (" + input.check_time + ")")
           << "\n  - status: " << (input.status.empty() ? "✗ (空)" : "✓ (" + input.status + ")"));
  }

  requireNonEmpty("employee_id", input.employee_id);
  requireNonEmpty("check_time", input.check_time);
  requireNonEmpty("status", input.status);

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  const bool has_device = !input.device_id.empty();
  const bool has_photo = !input.photo.empty();

  std::string columns = "employee_id, check_time, status";
  std::string placeholders = "?, ?, ?";
  if (has_device) {
    columns += ", device_id";
    placeholders += ", ?";
  }
  if (has_photo) {
    columns += ", photo";
    placeholders += ", ?";
  }

  const std::string sql =
      "INSERT INTO AttendanceRecord (" + columns + ") VALUES (" + placeholders +
      ")";

  DBG_DB("SQL执行开始: INSERT INTO AttendanceRecord (" << columns << ")");

  auto stmt = session.sql(sql);
  stmt.bind(input.employee_id);
  stmt.bind(input.check_time);
  stmt.bind(input.status);
  if (has_device) stmt.bind(input.device_id);
  if (has_photo) {
    stmt.bind(mysqlx::bytes(
        reinterpret_cast<const mysqlx::byte*>(input.photo.data()),
        input.photo.size()));
  }
  stmt.execute();

  DBG_DB("SQL执行成功: affected_rows=1 employeeId=" << input.employee_id);
}

void AttendanceRecordRepository::deleteRecord(
    mysqlx::Session& session, const AttendanceRecordDeleteInput& input) {
  const bool filter_id = input.id.has_value();
  const bool filter_employee = !input.employee_id.empty();
  const bool filter_check_time = !input.check_time.empty();
  const bool filter_device_id = !input.device_id.empty();
  const bool filter_status = !input.status.empty();
  const bool filter_received = !input.received_time.empty();

  if (!filter_id && !filter_employee && !filter_check_time && !filter_device_id &&
      !filter_status && !filter_received) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation,
        "at least one delete filter must be specified (would otherwise delete "
        "all rows)");
  }

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  std::string sql = "DELETE FROM AttendanceRecord";
  bool first = true;

  auto appendCond = [&](const char* clause) {
    sql += first ? " WHERE " : " AND ";
    sql += clause;
    first = false;
  };

  if (filter_id) appendCond("id = ?");
  if (filter_employee) appendCond("employee_id = ?");
  if (filter_check_time) appendCond("check_time = ?");
  if (filter_device_id) appendCond("device_id = ?");
  if (filter_status) appendCond("status = ?");
  if (filter_received) appendCond("received_time = ?");

  auto stmt = session.sql(sql);
  if (filter_id) stmt.bind(static_cast<int64_t>(*input.id));
  if (filter_employee) stmt.bind(input.employee_id);
  if (filter_check_time) stmt.bind(input.check_time);
  if (filter_device_id) stmt.bind(input.device_id);
  if (filter_status) stmt.bind(input.status);
  if (filter_received) stmt.bind(input.received_time);
  stmt.execute();
}

void AttendanceRecordRepository::updateRecord(
    mysqlx::Session& session, const AttendanceRecordLocateInput& locate,
    const AttendanceRecordUpdateInput& updates) {
  const bool filter_id = locate.id.has_value();
  const bool filter_employee = !locate.employee_id.empty();
  const bool filter_check_time = !locate.check_time.empty();
  const bool filter_device_id = !locate.device_id.empty();
  const bool filter_status = !locate.status.empty();
  const bool filter_received = !locate.received_time.empty();

  if (!filter_id && !filter_employee && !filter_check_time && !filter_device_id &&
      !filter_status && !filter_received) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation,
        "at least one locate filter must be specified (would otherwise update "
        "all rows)");
  }

  const bool set_check_time = !updates.check_time.empty();
  const bool set_device_id = !updates.device_id.empty();
  const bool set_status = !updates.status.empty();
  const bool set_received = !updates.received_time.empty();

  if (!set_check_time && !set_device_id && !set_status && !set_received) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation,
        "at least one field must be non-empty in AttendanceRecordUpdateInput");
  }

  session.sql("USE attendanceserver").execute();

  std::string sql = "UPDATE AttendanceRecord SET ";
  bool first_set = true;

  auto appendSet = [&](const char* clause) {
    if (!first_set) sql += ", ";
    sql += clause;
    first_set = false;
  };

  if (set_check_time) appendSet("check_time = ?");
  if (set_device_id) appendSet("device_id = ?");
  if (set_status) appendSet("status = ?");
  if (set_received) appendSet("received_time = ?");

  bool first_where = true;
  auto appendCond = [&](const char* clause) {
    sql += first_where ? " WHERE " : " AND ";
    sql += clause;
    first_where = false;
  };

  if (filter_id) appendCond("id = ?");
  if (filter_employee) appendCond("employee_id = ?");
  if (filter_check_time) appendCond("check_time = ?");
  if (filter_device_id) appendCond("device_id = ?");
  if (filter_status) appendCond("status = ?");
  if (filter_received) appendCond("received_time = ?");

  auto stmt = session.sql(sql);
  if (set_check_time) stmt.bind(updates.check_time);
  if (set_device_id) stmt.bind(updates.device_id);
  if (set_status) stmt.bind(updates.status);
  if (set_received) stmt.bind(updates.received_time);
  if (filter_id) stmt.bind(static_cast<int64_t>(*locate.id));
  if (filter_employee) stmt.bind(locate.employee_id);
  if (filter_check_time) stmt.bind(locate.check_time);
  if (filter_device_id) stmt.bind(locate.device_id);
  if (filter_status) stmt.bind(locate.status);
  if (filter_received) stmt.bind(locate.received_time);
  stmt.execute();
}

std::vector<AttendanceRecordRecord> AttendanceRecordRepository::selectRecord(
    mysqlx::Session& session, const AttendanceRecordQueryInput& input) {
  session.sql("USE attendanceserver").execute();

  const bool filter_id = input.id.has_value();
  const bool filter_employee = !input.employee_id.empty();
  const bool filter_check_time = !input.check_time.empty();
  const bool filter_device_id = !input.device_id.empty();
  const bool filter_status = !input.status.empty();
  const bool filter_received = !input.received_time.empty();

  // 不选中 photo(LONGBLOB)：mysqlx getBytes/BLOB 在部分版本下会引发内部映射异常
  //（std::unordered_map.at 报错 "invalid map<K, T> key"）。需要照片时请另做专用接口或其它协议。
  std::string sql =
      "SELECT id, employee_id, "
      "DATE_FORMAT(check_time, '%Y-%m-%d %H:%i:%s'), "
      "IFNULL(device_id, ''), IFNULL(status, ''), "
      "IFNULL(DATE_FORMAT(received_time, '%Y-%m-%d %H:%i:%s'), '') "
      "FROM AttendanceRecord";

  bool first = true;
  auto appendCond = [&](const char* clause) {
    sql += first ? " WHERE " : " AND ";
    sql += clause;
    first = false;
  };

  if (filter_id) appendCond("id = ?");
  if (filter_employee) appendCond("employee_id = ?");
  if (filter_check_time) appendCond("check_time = ?");
  if (filter_device_id) appendCond("device_id = ?");
  if (filter_status) appendCond("status = ?");
  if (filter_received) appendCond("received_time = ?");

  sql += " ORDER BY id";

  auto stmt = session.sql(sql);
  if (filter_id) stmt.bind(static_cast<int64_t>(*input.id));
  if (filter_employee) stmt.bind(input.employee_id);
  if (filter_check_time) stmt.bind(input.check_time);
  if (filter_device_id) stmt.bind(input.device_id);
  if (filter_status) stmt.bind(input.status);
  if (filter_received) stmt.bind(input.received_time);

  mysqlx::SqlResult result = stmt.execute();

  std::vector<AttendanceRecordRecord> rows;
  for (mysqlx::Row row : result.fetchAll()) {
    AttendanceRecordRecord r;
    r.id = static_cast<int>(row[0].get<std::int64_t>());
    r.employee_id = row[1].get<std::string>();
    r.check_time = row[2].get<std::string>();
    r.device_id = row[3].get<std::string>();
    r.status = row[4].get<std::string>();
    r.photo.clear();
    r.received_time = row[5].get<std::string>();
    rows.push_back(std::move(r));
  }
  return rows;
}

std::vector<AttendanceRecordArchiveRecord>
AttendanceRecordRepository::selectArchive(
    mysqlx::Session& session,
    const AttendanceRecordArchiveQueryInput& input) {
  session.sql("USE attendanceserver").execute();

  const bool filter_id = input.id.has_value();
  const bool filter_employee = !input.employee_id.empty();
  const bool filter_name = !input.name.empty();
  const bool filter_department = !input.department.empty();
  const bool filter_position = !input.position.empty();
  const bool filter_check_time = !input.check_time.empty();
  const bool filter_device_id = !input.device_id.empty();
  const bool filter_status = !input.status.empty();
  const bool filter_received = !input.received_time.empty();
  const bool filter_archived_at = !input.archived_at.empty();
  const bool filter_archive_reason = !input.archive_reason.empty();

  std::string sql =
      "SELECT id, employee_id, "
      "IFNULL(name, ''), IFNULL(department, ''), IFNULL(position, ''), "
      "DATE_FORMAT(check_time, '%Y-%m-%d %H:%i:%s'), "
      "IFNULL(device_id, ''), IFNULL(status, ''), "
      "IFNULL(DATE_FORMAT(received_time, '%Y-%m-%d %H:%i:%s'), ''), "
      "DATE_FORMAT(archived_at, '%Y-%m-%d %H:%i:%s'), "
      "IFNULL(archive_reason, '') "
      "FROM AttendanceRecordArchive";

  bool first = true;
  auto appendCond = [&](const char* clause) {
    sql += first ? " WHERE " : " AND ";
    sql += clause;
    first = false;
  };

  if (filter_id) appendCond("id = ?");
  if (filter_employee) appendCond("employee_id = ?");
  if (filter_name) appendCond("name = ?");
  if (filter_department) appendCond("department = ?");
  if (filter_position) appendCond("position = ?");
  if (filter_check_time) appendCond("check_time = ?");
  if (filter_device_id) appendCond("device_id = ?");
  if (filter_status) appendCond("status = ?");
  if (filter_received) appendCond("received_time = ?");
  if (filter_archived_at) appendCond("archived_at = ?");
  if (filter_archive_reason) appendCond("archive_reason = ?");

  sql += " ORDER BY id";

  auto stmt = session.sql(sql);
  if (filter_id) stmt.bind(static_cast<int64_t>(*input.id));
  if (filter_employee) stmt.bind(input.employee_id);
  if (filter_name) stmt.bind(input.name);
  if (filter_department) stmt.bind(input.department);
  if (filter_position) stmt.bind(input.position);
  if (filter_check_time) stmt.bind(input.check_time);
  if (filter_device_id) stmt.bind(input.device_id);
  if (filter_status) stmt.bind(input.status);
  if (filter_received) stmt.bind(input.received_time);
  if (filter_archived_at) stmt.bind(input.archived_at);
  if (filter_archive_reason) stmt.bind(input.archive_reason);

  mysqlx::SqlResult result = stmt.execute();

  std::vector<AttendanceRecordArchiveRecord> rows;
  for (mysqlx::Row row : result.fetchAll()) {
    AttendanceRecordArchiveRecord r;
    r.id = static_cast<int>(row[0].get<std::int64_t>());
    r.employee_id = row[1].get<std::string>();
    r.name = row[2].get<std::string>();
    r.department = row[3].get<std::string>();
    r.position = row[4].get<std::string>();
    r.check_time = row[5].get<std::string>();
    r.device_id = row[6].get<std::string>();
    r.status = row[7].get<std::string>();
    r.received_time = row[8].get<std::string>();
    r.archived_at = row[9].get<std::string>();
    r.archive_reason = row[10].get<std::string>();
    rows.push_back(std::move(r));
  }
  return rows;
}

void AttendanceRecordRepository::deleteArchive(mysqlx::Session& session,
                                               const std::string& employee_id) {
  requireNonEmpty("employee_id", employee_id);

  session.sql("USE attendanceserver").execute();

  session.sql("DELETE FROM AttendanceRecordArchive WHERE employee_id = ?")
      .bind(employee_id)
      .execute();
}

void AttendanceRecordRepository::insertArchive(mysqlx::Session& session,
                                                const std::string& employee_id,
                                                const std::string& reason) {
  requireNonEmpty("employee_id", employee_id);
  requireNonEmpty("reason", reason);

  session.sql("USE attendanceserver").execute();

  const std::string sql =
      "INSERT INTO AttendanceRecordArchive "
      "(employee_id, name, department, position, check_time, device_id, "
      "status, received_time, archive_reason) "
      "SELECT a.employee_id, p.name, p.department, p.position, "
      "a.check_time, a.device_id, a.status, a.received_time, ? "
      "FROM AttendanceRecord a "
      "JOIN Person p ON a.employee_id = p.employee_id "
      "WHERE a.employee_id = ?";

  session.sql(sql).bind(reason).bind(employee_id).execute();
}

}  // namespace db
