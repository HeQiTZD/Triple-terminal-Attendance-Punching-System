#pragma once

#include <optional>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>

namespace db {

struct AttendanceRecordCreateInput {
  // 必填：任一为空则抛 std::invalid_argument。
  std::string employee_id;
  std::string check_time;  // 'YYYY-MM-DD HH:MM:SS'，由 MySQL 转为 TIMESTAMP
  std::string status;

  // 可选：空字符串表示不提供该列（写入 NULL）。
  std::string device_id;

  // 可选：打卡照片二进制；空表示不提供该列（写入 NULL）。
  std::string photo;
};

// 考勤记录定位条件：非空字段以 AND 组合；不包含 photo。
// - id 若有值：按主键精确匹配。
// - 字符串非空则参与 WHERE；空字符串不作为条件。
// 至少一项条件须有效，否则误删/误更新全表的接口会抛 std::invalid_argument。
struct AttendanceRecordLocateInput {
  std::optional<int> id;
  std::string employee_id;
  std::string check_time;    // 'YYYY-MM-DD HH:MM:SS'，与 TIMESTAMP 相等比较
  std::string device_id;
  std::string status;
  std::string received_time;
};

using AttendanceRecordDeleteInput = AttendanceRecordLocateInput;

// 查询过滤：非空字段以 AND 组合；不匹配 photo。
// 若所有条件均为“未指定”（无 id、各字符串为空），则返回整张表记录（仍按 id 排序）。
struct AttendanceRecordQueryInput {
  std::optional<int> id;
  std::string employee_id;
  std::string check_time;
  std::string device_id;
  std::string status;
  std::string received_time;
};

struct AttendanceRecordRecord {
  int id = 0;
  std::string employee_id;
  std::string check_time;     // 'YYYY-MM-DD HH:MM:SS'
  std::string device_id;      // NULL 以空串表示
  std::string status;          // NULL 以空串表示
  std::string photo;          // selectRecord 不重读 LONGBLOB，恒为空；入库见 insertRecord
  std::string received_time;  // 'YYYY-MM-DD HH:MM:SS'；NULL 以空串表示
};

// 归档表查询过滤：非空字符串与可选 id 以 AND 组合。
// 若所有条件均为“未指定”，则返回整张归档表记录（按 id 升序）。
struct AttendanceRecordArchiveQueryInput {
  std::optional<int> id;
  std::string employee_id;
  std::string name;
  std::string department;
  std::string position;
  std::string check_time;       // 'YYYY-MM-DD HH:MM:SS'，与 TIMESTAMP 相等比较
  std::string device_id;
  std::string status;
  std::string received_time;    // 'YYYY-MM-DD HH:MM:SS'；可与 NULL 列相等比较（不提供则无此条件）
  std::string archived_at;      // 'YYYY-MM-DD HH:MM:SS'
  std::string archive_reason;
};

struct AttendanceRecordArchiveRecord {
  int id = 0;
  std::string employee_id;
  std::string name;
  std::string department;
  std::string position;
  std::string check_time;
  std::string device_id;
  std::string status;
  std::string received_time;
  std::string archived_at;
  std::string archive_reason;
};

// 可更新字段：空字符串表示不修改该列（与 insert 中“可选字段”语义一致）。
struct AttendanceRecordUpdateInput {
  std::string check_time;     // 'YYYY-MM-DD HH:MM:SS'
  std::string device_id;
  std::string status;
  std::string received_time;  // 'YYYY-MM-DD HH:MM:SS'
};

class AttendanceRecordRepository {
 public:
  // 新增考勤记录。
  // - employee_id / check_time / status 不能为空。
  // - device_id / photo 为空时不出现在 INSERT 列表中，对应列为 NULL。
  // - received_time 使用表默认值 CURRENT_TIMESTAMP。
  // - 数据库错误（外键不存在、时间格式非法等）抛 mysqlx::Error。
  static void insertRecord(mysqlx::Session& session,
                           const AttendanceRecordCreateInput& input);

  // 按非空条件 AND 组合删除匹配行；无匹配行时不抛异常。
  static void deleteRecord(mysqlx::Session& session,
                           const AttendanceRecordDeleteInput& input);

  // 按 locate 中非空字段 AND 定位行（不匹配 photo）；仅更新 updates 中非空字段。
  // locate 至少一项有效、updates 至少一项非空，否则抛 std::invalid_argument。
  // 无匹配行时不抛异常。
  static void updateRecord(mysqlx::Session& session,
                           const AttendanceRecordLocateInput& locate,
                           const AttendanceRecordUpdateInput& updates);

  // 按 input 中非空字段 AND 筛选；不匹配 photo。
  // 结果集中不查询 photo 列（避免 Connector BLOB/getBytes 问题）；返回的 Record.photo 恒为空串。
  // 若无任何筛选条件则返回全部行，结果按 id 升序。
  static std::vector<AttendanceRecordRecord> selectRecord(
      mysqlx::Session& session, const AttendanceRecordQueryInput& input);

  // 按 input 中非空字段及可选 id 对 AttendanceRecordArchive 做 AND 筛选。
  // 时间字段均为 'YYYY-MM-DD HH:MM:SS'，与表中 TIMESTAMP 相等比较。
  // 若无任何筛选条件则返回全部归档行，结果按 id 升序。
  static std::vector<AttendanceRecordArchiveRecord> selectArchive(
      mysqlx::Session& session,
      const AttendanceRecordArchiveQueryInput& input);

  // 按 employee_id 删除 AttendanceRecordArchive 中所有匹配行；为空则抛 std::invalid_argument。
  // 无匹配行时不抛异常。
  static void deleteArchive(mysqlx::Session& session,
                            const std::string& employee_id);

  // 将指定 employee_id 的所有 AttendanceRecord 记录复制到 AttendanceRecordArchive，
  // 同时从 Person 表快照 name/department/position，并记录归档原因。
  // employee_id 和 reason 为空则抛 std::invalid_argument。
  // 应在删除人员前调用，与删除操作在同一事务中。
  static void insertArchive(mysqlx::Session& session,
                            const std::string& employee_id,
                            const std::string& reason);
};

}  // namespace db
