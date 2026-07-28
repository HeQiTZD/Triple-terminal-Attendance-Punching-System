#pragma once

#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>

namespace db {

struct UserAccountCreateInput {
  // 必填字段
  std::string employee_id;  // 必须已存在于 Person 表（外键约束）
  std::string password;     // 已散列后的密文，由调用方保证不是明文
  // 可选字段：传空字符串表示该列写入 NULL
  std::string name;
};

struct UserAccountUpdateInput {
  // 约定：字段传空字符串表示“不更新该字段”。
  std::string name;
  std::string password;  // 已散列后的密文，由调用方保证不是明文
};

struct UserAccountQueryInput {
  // 约定：字段传空字符串表示“不作为该字段的过滤条件”。
  // 所有非空字段以 AND 方式组合。
  // - employee_id：精确匹配。
  // - created_at：按 'YYYY-MM-DD HH:MM:SS' 格式做前缀匹配，
  //   例如传 "2026-05-08" 可命中该日全部记录，传完整时间字符串等价于精确匹配。
  std::string employee_id;
  std::string created_at;
};

struct UserAccountRecord {
  int id = 0;
  std::string employee_id;
  std::string name;             // 表中可空，NULL 会以空串返回
  std::string password;         // 已散列后的密文
  std::string created_at;       // 'YYYY-MM-DD HH:MM:SS' 字符串
  std::string last_login_time;  // 表中可空，NULL 会以空串返回
};

class UserAccountRepository {
 public:
  // 新增用户账号：
  // - employee_id / password 不能为空，否则抛 std::invalid_argument。
  // - name 为空字符串时，对应列写入 NULL（schema 允许 NULL）。
  // - created_at 由 DB 默认值生成；last_login_time 初始为 NULL。
  // - 数据库层错误（employee_id 重复、外键 Person 不存在等）会抛 mysqlx::Error。
  static void insertUser(mysqlx::Session& session,
                         const UserAccountCreateInput& input);

  // 删除用户账号：仅按 employee_id 删除 UserAccount 表中的对应行。
  // - 不会级联到 Person 表（schema 中的 ON DELETE CASCADE 是 Person→UserAccount
  //   方向，反向不生效，因此单表 DELETE 已满足“不级联 Person”要求）。
  // - 若该 employee_id 不存在则不抛异常（与 deletePerson 的语义保持一致）。
  // - employee_id 为空时抛 std::invalid_argument。
  static void deleteUser(mysqlx::Session& session,
                         const std::string& employee_id);

  // 更新用户账号：根据 employee_id 定位，仅更新传入的非空字段。
  // - employee_id 为空 / 两个可更新字段都为空时抛 std::invalid_argument。
  // - 若该 employee_id 不存在，UPDATE 影响 0 行，不抛异常。
  static void updateUser(mysqlx::Session& session,
                         const std::string& employee_id,
                         const UserAccountUpdateInput& input);

  // 查询用户账号：根据 input 中的非空字段拼接 WHERE 条件（AND 组合）。
  // 若所有字段均为空，则返回全部记录。结果按 id 升序排列。
  static std::vector<UserAccountRecord> selectUser(
      mysqlx::Session& session, const UserAccountQueryInput& input);
};

}  // namespace db
