#pragma once

#include <optional>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>

namespace db {

struct FaceDataUpsertInput {
  std::string employee_id;
  // 原始特征字节；须与 feature_size 一致（通常等于 size()）。
  std::string feature_vector;
  int feature_size = 0;
};

struct FaceDataRecord {
  int id = 0;
  std::string employee_id;
  std::string feature_vector;
  int feature_size = 0;
  std::string created_at;
  std::string updated_at;
};

struct FaceDataPageQuery {
  // 空串表示从第一条开始；否则按 employee_id 字典序取「大于 after_employee_id」的下一页。
  std::string after_employee_id;
  int limit = 100;
};

class FaceDataRepository {
 public:
  // 插入或更新（employee_id UNIQUE）。feature_vector 非空且 feature_size == feature_vector.size()。
  static void upsert(mysqlx::Session& session, const FaceDataUpsertInput& input);

  // 按工号读取；不存在返回 nullopt。读取二进制经 HEX 解码（规避 mysqlx 部分版本读 LONGBLOB 映射问题）。
  static std::optional<FaceDataRecord> findByEmployeeId(
      mysqlx::Session& session, const std::string& employee_id);

  static void deleteByEmployeeId(mysqlx::Session& session,
                                 const std::string& employee_id);

  // 按键集分页，按 employee_id 升序；limit 须为正。
  static std::vector<FaceDataRecord> listFaceDataPage(
      mysqlx::Session& session, const FaceDataPageQuery& query);
};

}  // namespace db
