#include "FaceDataRepository.hpp"

#include <stdexcept>

#include "protocol/AppError.hpp"

namespace db {

namespace {

void requireNonEmpty(const char* fieldName, const std::string& value) {
  if (value.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, std::string(fieldName) + " must not be empty");
  }
}

std::string decodeHex(std::string_view hex) {
  if (hex.size() % 2 != 0) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "HEX length must be even");
  }
  std::string out;
  out.reserve(hex.size() / 2);
  auto hexVal = [](char c) -> unsigned {
    if (c >= '0' && c <= '9') return static_cast<unsigned>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<unsigned>(c - 'a' + 10);
    if (c >= 'A' && c <= 'F') return static_cast<unsigned>(c - 'A' + 10);
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "invalid hex digit in feature_vector");
  };
  for (std::size_t i = 0; i < hex.size(); i += 2) {
    const unsigned hi = hexVal(hex[i]);
    const unsigned lo = hexVal(hex[i + 1]);
    out.push_back(static_cast<char>((hi << 4) | lo));
  }
  return out;
}

FaceDataRecord rowToRecord(const mysqlx::Row& row) {
  FaceDataRecord r;
  r.id = static_cast<int>(row[0].get<std::int64_t>());
  r.employee_id = row[1].get<std::string>();
  const std::string hex_feat = row[2].get<std::string>();
  r.feature_vector = decodeHex(hex_feat);
  r.feature_size = static_cast<int>(row[3].get<std::int64_t>());
  r.created_at = row[4].get<std::string>();
  r.updated_at = row[5].get<std::string>();
  if (r.feature_size != static_cast<int>(r.feature_vector.size())) {
    throw protocol::AppError(protocol::ErrorCode::kDbError,
        "face_data feature_size mismatch with decoded feature_vector length");
  }
  return r;
}

}  // namespace

void FaceDataRepository::upsert(mysqlx::Session& session,
                                const FaceDataUpsertInput& input) {
  requireNonEmpty("employee_id", input.employee_id);
  requireNonEmpty("feature_vector", input.feature_vector);
  if (input.feature_size <= 0) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "feature_size must be positive");
  }
  if (static_cast<std::size_t>(input.feature_size) != input.feature_vector.size()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation,
        "feature_size must equal feature_vector.size()");
  }

  session.sql("USE attendanceserver").execute();

  session
      .sql(
          "INSERT INTO face_data (employee_id, feature_vector, feature_size) "
          "VALUES (?, ?, ?) "
          "ON DUPLICATE KEY UPDATE "
          "feature_vector = VALUES(feature_vector), "
          "feature_size = VALUES(feature_size), "
          "updated_at = CURRENT_TIMESTAMP")
      .bind(input.employee_id)
      .bind(mysqlx::bytes(
          reinterpret_cast<const mysqlx::byte*>(input.feature_vector.data()),
          input.feature_vector.size()))
      .bind(input.feature_size)
      .execute();
}

std::optional<FaceDataRecord> FaceDataRepository::findByEmployeeId(
    mysqlx::Session& session, const std::string& employee_id) {
  requireNonEmpty("employee_id", employee_id);

  session.sql("USE attendanceserver").execute();

  mysqlx::SqlResult result =
      session
          .sql(
              "SELECT id, employee_id, HEX(feature_vector), feature_size, "
              "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s'), "
              "DATE_FORMAT(updated_at, '%Y-%m-%d %H:%i:%s') "
              "FROM face_data WHERE employee_id = ? LIMIT 1")
          .bind(employee_id)
          .execute();

  mysqlx::Row row = result.fetchOne();
  if (!row) {
    return std::nullopt;
  }
  return rowToRecord(row);
}

void FaceDataRepository::deleteByEmployeeId(mysqlx::Session& session,
                                            const std::string& employee_id) {
  requireNonEmpty("employee_id", employee_id);

  session.sql("USE attendanceserver").execute();

  session.sql("DELETE FROM face_data WHERE employee_id = ?")
      .bind(employee_id)
      .execute();
}

std::vector<FaceDataRecord> FaceDataRepository::listFaceDataPage(
    mysqlx::Session& session, const FaceDataPageQuery& query) {
  if (query.limit <= 0) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "limit must be positive");
  }

  session.sql("USE attendanceserver").execute();

  mysqlx::SqlResult result;
  if (query.after_employee_id.empty()) {
    result = session
                 .sql(
                     "SELECT id, employee_id, HEX(feature_vector), feature_size, "
                     "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s'), "
                     "DATE_FORMAT(updated_at, '%Y-%m-%d %H:%i:%s') "
                     "FROM face_data ORDER BY employee_id ASC LIMIT ?")
                 .bind(query.limit)
                 .execute();
  } else {
    result =
        session
            .sql(
                "SELECT id, employee_id, HEX(feature_vector), feature_size, "
                "DATE_FORMAT(created_at, '%Y-%m-%d %H:%i:%s'), "
                "DATE_FORMAT(updated_at, '%Y-%m-%d %H:%i:%s') "
                "FROM face_data WHERE employee_id > ? ORDER BY employee_id ASC "
                "LIMIT ?")
            .bind(query.after_employee_id)
            .bind(query.limit)
            .execute();
  }

  std::vector<FaceDataRecord> rows;
  for (mysqlx::Row row : result.fetchAll()) {
    rows.push_back(rowToRecord(row));
  }
  return rows;
}

}  // namespace db
