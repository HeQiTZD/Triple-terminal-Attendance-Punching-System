#pragma once

#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>

namespace db {

struct PersonCreateInput {
  std::string name;
  std::string employee_id;
  std::string department;
  std::string position;
};

struct PersonUpdateInput {
  // 约定：字段传空字符串表示“不更新该字段”。
  std::string name;
  std::string department;
  std::string position;
};

struct PersonQueryInput {
  // 约定：字段传空字符串表示“不作为该字段的过滤条件”。
  // 所有非空字段以 AND 方式组合。
  // - name / employee_id / department / position：精确匹配。
  // - created_at / updated_at：按 'YYYY-MM-DD HH:MM:SS' 格式做前缀匹配，
  //   例如传 "2026-05-08" 可命中该日全部记录，传完整时间字符串等价于精确匹配。
  std::string name;
  std::string employee_id;
  std::string department;
  std::string position;
  std::string created_at;
  std::string updated_at;
};

struct PersonRecord {
  int id = 0;
  std::string name;
  std::string employee_id;
  std::string department;
  std::string position;
  std::string created_at;
  std::string updated_at;
};

struct PersonPageQuery {
  // 键集分页：返回 id > after_id 的记录；首屏传 0。
  int after_id = 0;
  int limit = 100;
};

class PersonRepository {
 public:
  // 新增人员：所有字段都不能为空；若有空值则抛出 std::invalid_argument。
  // 数据库层错误（例如 employee_id 重复）会抛出 mysqlx::Error。
  static void insert(mysqlx::Session& session, const PersonCreateInput& input);

  // 更新人员信息：根据 employee_id 定位；仅更新传入的非空字段，并更新 updated_at。
  // 若三个字段都为空，则抛出 std::invalid_argument。
  static void updatePerson(mysqlx::Session& session,
                           const std::string& employee_id,
                           const PersonUpdateInput& input);

  // 查询人员：根据 input 中的非空字段拼接 WHERE 条件（精确匹配，AND 组合）。
  // 若所有字段均为空，则返回全部记录。结果按 id 升序排列。
  static std::vector<PersonRecord> selectPerson(mysqlx::Session& session,
                                                const PersonQueryInput& input);

  // 同步用分页：按 id 升序，WHERE id > after_id LIMIT limit（limit 须为正）。
  static std::vector<PersonRecord> listPersonPage(
      mysqlx::Session& session, const PersonPageQuery& query);
};

// 删除人员（按 employee_id）。
// 说明：
// - 为满足外键约束，会先删除 AttendanceRecord 中该员工的记录。
// - AttendanceRecordArchive 为离职归档表，删除人员时保留归档记录。
// - face_data/UserAccount 会通过 ON DELETE CASCADE 自动删除。
void deletePerson(mysqlx::Session& session, const std::string& employee_id);

}  // namespace db

