#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>

namespace db {

struct DeviceConfigDeployInput {
  std::string config_version;
  std::string config_content;
  std::string config_hash;
  std::string description;
  std::string created_by;
};

struct PendingDeviceConfigTarget {
  std::int64_t target_id = 0;
  std::int64_t deploy_id = 0;
  std::string device_id;
  std::string config_version;
  std::string config_content;
  std::string config_hash;
};

class DeviceConfigRepository {
 public:
  static std::int64_t insertDeploy(mysqlx::Session& session,
                                   const DeviceConfigDeployInput& input);

  static std::int64_t insertTarget(mysqlx::Session& session,
                                   std::int64_t deploy_id,
                                   const std::string& device_id);

  static int countDeploysWithVersionPrefix(mysqlx::Session& session,
                                           const std::string& prefix);

  static void markTargetSent(mysqlx::Session& session, std::int64_t target_id,
                             const std::string& forward_msg_id);

  static void markTargetFailedByForwardId(mysqlx::Session& session,
                                          const std::string& forward_msg_id,
                                          const std::string& error_message);

  static void markTargetFailedById(mysqlx::Session& session,
                                   std::int64_t target_id,
                                   const std::string& error_message);

  static void applyAckByForwardId(mysqlx::Session& session,
                                  const std::string& forward_msg_id,
                                  bool success,
                                  const std::string& error_message);

  static void applyAckByTargetId(mysqlx::Session& session,
                                 std::int64_t target_id, bool success,
                                 const std::string& error_message);

  static std::vector<PendingDeviceConfigTarget> listPendingTargetsForDevice(
      mysqlx::Session& session, const std::string& device_id);
};

}  // namespace db
