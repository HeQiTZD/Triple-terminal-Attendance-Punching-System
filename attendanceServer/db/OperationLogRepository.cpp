#include "db/OperationLogRepository.hpp"

#include <algorithm>

namespace db {

static void useDb(mysqlx::Session& session) {
  session.sql("USE attendanceserver").execute();
}

void OperationLogRepository::insertLog(mysqlx::Session& session,
                                       const OperationLogEntry& entry) {
  useDb(session);
  session
      .sql("INSERT INTO OperationLog "
           "(user_id, employee_id, action, target_type, target_id, "
           "detail, ip_address, result) "
           "VALUES (?, ?, ?, ?, ?, ?, ?, ?)")
      .bind(entry.user_id,
            entry.employee_id,
            entry.action,
            entry.target_type,
            entry.target_id,
            entry.detail.empty() ? "null" : entry.detail,
            entry.ip_address,
            entry.result.empty() ? "success" : entry.result)
      .execute();
}

std::vector<OperationLogRecord> OperationLogRepository::queryLogs(
    mysqlx::Session& session, const OperationLogQueryInput& input) {
  useDb(session);

  std::string sql =
      "SELECT id, IFNULL(user_id, 0), IFNULL(employee_id, ''), action, "
      "IFNULL(target_type, ''), IFNULL(target_id, ''), "
      "IFNULL(detail, ''), IFNULL(ip_address, ''), "
      "IFNULL(result, 'success'), "
      "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s') "
      "FROM OperationLog";

  bool first = true;
  auto appendCond = [&](const char* clause) {
    sql += first ? " WHERE " : " AND ";
    sql += clause;
    first = false;
  };

  if (input.user_id > 0) appendCond("user_id = ?");
  if (!input.action.empty()) appendCond("action = ?");
  if (!input.start_time.empty()) appendCond("created_at >= ?");
  if (!input.end_time.empty()) appendCond("created_at <= ?");

  sql += " ORDER BY id DESC";

  const int page_size = std::clamp(input.page_size, 1, 500);
  const int offset = (std::max(1, input.page) - 1) * page_size;
  sql += " LIMIT " + std::to_string(page_size) +
         " OFFSET " + std::to_string(offset);

  auto stmt = session.sql(sql);
  if (input.user_id > 0) stmt.bind(input.user_id);
  if (!input.action.empty()) stmt.bind(input.action);
  if (!input.start_time.empty()) stmt.bind(input.start_time);
  if (!input.end_time.empty()) stmt.bind(input.end_time);

  auto result = stmt.execute();

  std::vector<OperationLogRecord> records;
  for (auto row : result.fetchAll()) {
    OperationLogRecord r;
    r.id = row[0].get<int64_t>();
    r.user_id = static_cast<int>(row[1].get<int64_t>());
    r.employee_id = row[2].get<std::string>();
    r.action = row[3].get<std::string>();
    r.target_type = row[4].get<std::string>();
    r.target_id = row[5].get<std::string>();
    r.detail = row[6].get<std::string>();
    r.ip_address = row[7].get<std::string>();
    r.result = row[8].get<std::string>();
    r.created_at = row[9].get<std::string>();
    records.push_back(std::move(r));
  }
  return records;
}

}  // namespace db
