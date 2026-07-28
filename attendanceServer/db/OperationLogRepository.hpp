#pragma once

#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>

namespace db {

struct OperationLogEntry {
  int user_id = 0;
  std::string employee_id;
  std::string action;
  std::string target_type;
  std::string target_id;
  std::string detail;      // JSON string
  std::string ip_address;
  std::string result;      // "success" | "denied" | "error"
};

struct OperationLogRecord {
  int64_t id = 0;
  int user_id = 0;
  std::string employee_id;
  std::string action;
  std::string target_type;
  std::string target_id;
  std::string detail;
  std::string ip_address;
  std::string result;
  std::string created_at;
};

struct OperationLogQueryInput {
  int user_id = 0;           // 0 means no filter
  std::string action;        // empty means no filter
  std::string start_time;    // 'YYYY-MM-DD HH:MM:SS', empty = no filter
  std::string end_time;
  int page = 1;
  int page_size = 50;
};

class OperationLogRepository {
 public:
  static void insertLog(mysqlx::Session& session,
                        const OperationLogEntry& entry);

  static std::vector<OperationLogRecord> queryLogs(
      mysqlx::Session& session, const OperationLogQueryInput& input);
};

}  // namespace db
