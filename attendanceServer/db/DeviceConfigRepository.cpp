#include "DeviceConfigRepository.hpp"

#include <stdexcept>

#include "protocol/AppError.hpp"

namespace db {

namespace {

void require_non_empty(const char* field, const std::string& value) {
  if (value.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, std::string(field) + " must not be empty");
  }
}

std::int64_t last_insert_id(mysqlx::Session& session) {
  auto row = session.sql("SELECT LAST_INSERT_ID()").execute().fetchOne();
  if (!row) {
    throw protocol::AppError(protocol::ErrorCode::kDbError, "failed to read LAST_INSERT_ID()");
  }
  return static_cast<std::int64_t>(row[0].get<std::uint64_t>());
}

std::string row_string_or_empty(const mysqlx::Row& row, unsigned index) {
  if (row[index].isNull()) {
    return {};
  }
  return row[index].get<std::string>();
}

}  // namespace

std::int64_t DeviceConfigRepository::insertDeploy(
    mysqlx::Session& session, const DeviceConfigDeployInput& input) {
  require_non_empty("config_version", input.config_version);
  require_non_empty("config_content", input.config_content);
  require_non_empty("config_hash", input.config_hash);
  require_non_empty("created_by", input.created_by);

  session.sql("USE attendanceserver").execute();
  session
      .sql("INSERT INTO device_config_deploy "
           "(config_version, config_content, config_hash, description, "
           "created_by) VALUES (?, ?, ?, ?, ?)")
      .bind(input.config_version, input.config_content, input.config_hash,
            input.description, input.created_by)
      .execute();
  return last_insert_id(session);
}

std::int64_t DeviceConfigRepository::insertTarget(
    mysqlx::Session& session, std::int64_t deploy_id,
    const std::string& device_id) {
  if (deploy_id <= 0) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "deploy_id must be positive");
  }
  require_non_empty("device_id", device_id);

  session.sql("USE attendanceserver").execute();
  session
      .sql("INSERT INTO device_config_target (deploy_id, device_id, status) "
           "VALUES (?, ?, 'pending')")
      .bind(deploy_id, device_id)
      .execute();
  return last_insert_id(session);
}

int DeviceConfigRepository::countDeploysWithVersionPrefix(
    mysqlx::Session& session, const std::string& prefix) {
  require_non_empty("prefix", prefix);
  session.sql("USE attendanceserver").execute();
  auto row = session
                 .sql("SELECT COUNT(*) FROM device_config_deploy "
                      "WHERE config_version LIKE CONCAT(?, '%')")
                 .bind(prefix)
                 .execute()
                 .fetchOne();
  if (!row) {
    return 0;
  }
  return static_cast<int>(row[0].get<std::uint64_t>());
}

void DeviceConfigRepository::markTargetSent(
    mysqlx::Session& session, std::int64_t target_id,
    const std::string& forward_msg_id) {
  if (target_id <= 0) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "target_id must be positive");
  }
  require_non_empty("forward_msg_id", forward_msg_id);

  session.sql("USE attendanceserver").execute();
  session
      .sql("UPDATE device_config_target SET status = 'sent', "
           "forward_msg_id = ?, error_message = NULL, sent_at = NOW() "
           "WHERE id = ? AND status = 'pending'")
      .bind(forward_msg_id, target_id)
      .execute();
}

void DeviceConfigRepository::markTargetFailedByForwardId(
    mysqlx::Session& session, const std::string& forward_msg_id,
    const std::string& error_message) {
  require_non_empty("forward_msg_id", forward_msg_id);
  session.sql("USE attendanceserver").execute();
  session
      .sql("UPDATE device_config_target SET status = 'failed', "
           "error_message = ?, ack_at = NOW() WHERE forward_msg_id = ?")
      .bind(error_message, forward_msg_id)
      .execute();
}

void DeviceConfigRepository::markTargetFailedById(
    mysqlx::Session& session, std::int64_t target_id,
    const std::string& error_message) {
  if (target_id <= 0) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "target_id must be positive");
  }
  session.sql("USE attendanceserver").execute();
  session
      .sql("UPDATE device_config_target SET status = 'failed', "
           "error_message = ?, ack_at = NOW() WHERE id = ?")
      .bind(error_message, target_id)
      .execute();
}

void DeviceConfigRepository::applyAckByForwardId(
    mysqlx::Session& session, const std::string& forward_msg_id, bool success,
    const std::string& error_message) {
  require_non_empty("forward_msg_id", forward_msg_id);
  session.sql("USE attendanceserver").execute();

  auto row = session
                 .sql("SELECT t.device_id, d.config_version, d.config_hash "
                      "FROM device_config_target t "
                      "INNER JOIN device_config_deploy d ON d.id = t.deploy_id "
                      "WHERE t.forward_msg_id = ? LIMIT 1")
                 .bind(forward_msg_id)
                 .execute()
                 .fetchOne();
  if (!row) {
    return;
  }

  const std::string device_id = row_string_or_empty(row, 0);
  const std::string version = row_string_or_empty(row, 1);
  const std::string hash = row_string_or_empty(row, 2);

  session.sql("START TRANSACTION").execute();
  try {
    if (success) {
      session
          .sql("UPDATE device_config_target SET status = 'applied', "
               "ack_at = NOW(), applied_at = NOW(), error_message = NULL "
               "WHERE forward_msg_id = ?")
          .bind(forward_msg_id)
          .execute();
      session
          .sql("INSERT INTO device_config_current "
               "(device_id, config_version, config_hash) VALUES (?, ?, ?) "
               "ON DUPLICATE KEY UPDATE config_version = VALUES(config_version), "
               "config_hash = VALUES(config_hash), updated_at = NOW()")
          .bind(device_id, version, hash)
          .execute();
    } else {
      session
          .sql("UPDATE device_config_target SET status = 'failed', "
               "ack_at = NOW(), error_message = ? WHERE forward_msg_id = ?")
          .bind(error_message, forward_msg_id)
          .execute();
    }
    session.sql("COMMIT").execute();
  } catch (...) {
    session.sql("ROLLBACK").execute();
    throw;
  }
}

void DeviceConfigRepository::applyAckByTargetId(mysqlx::Session& session,
                                                std::int64_t target_id,
                                                bool success,
                                                const std::string& error_message) {
  if (target_id <= 0) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "target_id must be positive");
  }
  session.sql("USE attendanceserver").execute();

  auto row = session
                 .sql("SELECT t.device_id, d.config_version, d.config_hash "
                      "FROM device_config_target t "
                      "INNER JOIN device_config_deploy d ON d.id = t.deploy_id "
                      "WHERE t.id = ? LIMIT 1")
                 .bind(target_id)
                 .execute()
                 .fetchOne();
  if (!row) {
    return;
  }

  const std::string device_id = row_string_or_empty(row, 0);
  const std::string version = row_string_or_empty(row, 1);
  const std::string hash = row_string_or_empty(row, 2);

  session.sql("START TRANSACTION").execute();
  try {
    if (success) {
      session
          .sql("UPDATE device_config_target SET status = 'applied', "
               "ack_at = NOW(), applied_at = NOW(), error_message = NULL "
               "WHERE id = ?")
          .bind(target_id)
          .execute();
      session
          .sql("INSERT INTO device_config_current "
               "(device_id, config_version, config_hash) VALUES (?, ?, ?) "
               "ON DUPLICATE KEY UPDATE config_version = VALUES(config_version), "
               "config_hash = VALUES(config_hash), updated_at = NOW()")
          .bind(device_id, version, hash)
          .execute();
    } else {
      session
          .sql("UPDATE device_config_target SET status = 'failed', "
               "ack_at = NOW(), error_message = ? WHERE id = ?")
          .bind(error_message, target_id)
          .execute();
    }
    session.sql("COMMIT").execute();
  } catch (...) {
    session.sql("ROLLBACK").execute();
    throw;
  }
}

std::vector<PendingDeviceConfigTarget>
DeviceConfigRepository::listPendingTargetsForDevice(
    mysqlx::Session& session, const std::string& device_id) {
  require_non_empty("device_id", device_id);
  session.sql("USE attendanceserver").execute();
  auto result =
      session
          .sql("SELECT t.id, t.deploy_id, t.device_id, d.config_version, "
               "d.config_content, d.config_hash "
               "FROM device_config_target t "
               "INNER JOIN device_config_deploy d ON d.id = t.deploy_id "
               "WHERE t.device_id = ? AND t.status = 'pending' "
               "ORDER BY t.id")
          .bind(device_id)
          .execute();

  std::vector<PendingDeviceConfigTarget> rows;
  for (auto row : result.fetchAll()) {
    PendingDeviceConfigTarget item;
    item.target_id = static_cast<std::int64_t>(row[0].get<std::uint64_t>());
    item.deploy_id = static_cast<std::int64_t>(row[1].get<std::uint64_t>());
    item.device_id = row_string_or_empty(row, 2);
    item.config_version = row_string_or_empty(row, 3);
    item.config_content = row_string_or_empty(row, 4);
    item.config_hash = row_string_or_empty(row, 5);
    rows.push_back(std::move(item));
  }
  return rows;
}

}  // namespace db
