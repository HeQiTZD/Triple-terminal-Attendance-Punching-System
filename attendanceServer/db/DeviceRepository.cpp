#include "DeviceRepository.hpp"

#include <stdexcept>

#include "protocol/AppError.hpp"

namespace db {

static void requireNonEmpty(const char* fieldName, const std::string& value) {
  if (value.empty()) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, std::string(fieldName) + " must not be empty");
  }
}

void DeviceRepository::insertDevice(mysqlx::Session& session,
                                    const DeviceCreateInput& input) {
  requireNonEmpty("device_id", input.device_id);
  requireNonEmpty("device_name", input.device_name);

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  const bool hasIp = !input.ip_address.empty();
  const bool hasLastOnline = !input.last_online.empty();
  const bool hasStatus = !input.status.empty();

  // 动态拼接列名 / 占位符：可选字段为空时直接不出现在 INSERT 列表里，
  // 由数据库自行处理（ip_address / last_online → NULL；status → 默认值 'offline'）。
  // last_online 以 'YYYY-MM-DD HH:MM:SS' 字符串形式传入，MySQL 会按会话时区
  // 隐式转换成 TIMESTAMP；格式非法时会以 mysqlx::Error 抛出。
  std::string columns = "device_id, device_name";
  std::string placeholders = "?, ?";
  if (hasIp) {
    columns += ", ip_address";
    placeholders += ", ?";
  }
  if (hasLastOnline) {
    columns += ", last_online";
    placeholders += ", ?";
  }
  if (hasStatus) {
    columns += ", status";
    placeholders += ", ?";
  }

  const std::string sql =
      "INSERT INTO Device (" + columns + ") VALUES (" + placeholders + ")";

  auto stmt = session.sql(sql);
  stmt.bind(input.device_id);
  stmt.bind(input.device_name);
  if (hasIp) stmt.bind(input.ip_address);
  if (hasLastOnline) stmt.bind(input.last_online);
  if (hasStatus) stmt.bind(input.status);
  stmt.execute();
}

void DeviceRepository::updateDevice(mysqlx::Session& session,
                                    const std::string& device_id,
                                    const DeviceUpdateInput& input) {
  requireNonEmpty("device_id", device_id);

  const bool updateName = !input.device_name.empty();
  const bool updateIp = !input.ip_address.empty();
  const bool updateLastOnlineStr =
      !input.last_online_now && !input.last_online.empty();
  const bool updateLastOnlineNow = input.last_online_now;
  const bool updateStatus = !input.status.empty();

  if (!updateName && !updateIp && !updateLastOnlineStr && !updateLastOnlineNow &&
      !updateStatus) {
    throw protocol::AppError(protocol::ErrorCode::kBusinessValidation, "at least one field must be non-empty");
  }

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  // 动态拼接 SET 子句，仅包含传入的非空字段；Device 表无 updated_at 列，无需追加。
  std::string sql = "UPDATE Device SET ";
  bool first = true;

  auto appendSet = [&](const char* clause) {
    if (!first) sql += ", ";
    sql += clause;
    first = false;
  };

  if (updateName) appendSet("device_name = ?");
  if (updateIp) appendSet("ip_address = ?");
  if (updateLastOnlineStr) appendSet("last_online = ?");
  if (updateLastOnlineNow) appendSet("last_online = NOW()");
  if (updateStatus) appendSet("status = ?");

  sql += " WHERE device_id = ?";

  auto stmt = session.sql(sql);
  if (updateName) stmt.bind(input.device_name);
  if (updateIp) stmt.bind(input.ip_address);
  if (updateLastOnlineStr) stmt.bind(input.last_online);
  if (updateStatus) stmt.bind(input.status);
  stmt.bind(device_id);
  stmt.execute();
}

void DeviceRepository::upsertDeviceOnline(mysqlx::Session& session,
                                          const std::string& device_id,
                                          const std::string& ip_address) {
  requireNonEmpty("device_id", device_id);
  session.sql("USE attendanceserver").execute();

  std::string sql =
      "UPDATE Device SET status = 'online', last_online = NOW()";
  const bool setIp = !ip_address.empty();
  if (setIp) {
    sql += ", ip_address = ?";
  }
  sql += " WHERE device_id = ?";

  auto stmt = session.sql(sql);
  if (setIp) {
    stmt.bind(ip_address);
  }
  stmt.bind(device_id);
  mysqlx::SqlResult result = stmt.execute();
  if (result.getAffectedItemsCount() > 0) {
    return;
  }

  DeviceCreateInput in;
  in.device_id = device_id;
  in.device_name = device_id;
  in.ip_address = ip_address;
  in.last_online = {};
  in.status = "online";
  insertDevice(session, in);
  session.sql(
         "UPDATE Device SET last_online = NOW(), status = 'online' WHERE "
         "device_id = ?")
      .bind(device_id)
      .execute();
}

std::string DeviceRepository::getDeviceStatus(mysqlx::Session& session,
                                              const std::string& device_id) {
  requireNonEmpty("device_id", device_id);
  session.sql("USE attendanceserver").execute();
  auto row = session.sql(
                    "SELECT IFNULL(status, '') FROM Device WHERE device_id = ?")
                 .bind(device_id)
                 .execute()
                 .fetchOne();
  if (!row) {
    return {};
  }
  return row[0].get<std::string>();
}

void DeviceRepository::markDeviceOffline(mysqlx::Session& session,
                                         const std::string& device_id) {
  requireNonEmpty("device_id", device_id);
  session.sql("USE attendanceserver").execute();
  session.sql("UPDATE Device SET status = 'offline' WHERE device_id = ?")
      .bind(device_id)
      .execute();
}

std::vector<DeviceRecord> DeviceRepository::selectDevice(
    mysqlx::Session& session, const DeviceQueryInput& input) {
  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  const bool filterDeviceId = !input.device_id.empty();
  const bool filterDeviceName = !input.device_name.empty();
  const bool filterIp = !input.ip_address.empty();
  const bool filterLastOnline = !input.last_online.empty();

  // 用 IFNULL 把可空列折叠为空串，方便后续无脑转 std::string；
  // last_online 用 DATE_FORMAT 直接转成字符串，避免依赖 mysqlx 对 TIMESTAMP 的隐式转换；
  // 再外套 IFNULL，把"从未上线过"折叠成空串。
  std::string sql =
      "SELECT id, device_id, IFNULL(device_name, ''), IFNULL(ip_address, ''), "
      "IFNULL(DATE_FORMAT(last_online, '%Y-%m-%d %H:%i:%s'), ''), "
      "IFNULL(status, '') "
      "FROM Device";

  bool first = true;
  auto appendCond = [&](const char* clause) {
    sql += first ? " WHERE " : " AND ";
    sql += clause;
    first = false;
  };

  if (filterDeviceId) appendCond("device_id = ?");
  if (filterDeviceName) appendCond("device_name = ?");
  if (filterIp) appendCond("ip_address = ?");
  // last_online 使用 DATE_FORMAT 后做前缀 LIKE 匹配：
  // 既能精确匹配完整时间串，也能用 'YYYY-MM-DD' 命中整天、'YYYY-MM' 命中整月。
  if (filterLastOnline) {
    appendCond(
        "DATE_FORMAT(last_online, '%Y-%m-%d %H:%i:%s') LIKE CONCAT(?, '%')");
  }

  sql += " ORDER BY id";

  auto stmt = session.sql(sql);
  if (filterDeviceId) stmt.bind(input.device_id);
  if (filterDeviceName) stmt.bind(input.device_name);
  if (filterIp) stmt.bind(input.ip_address);
  if (filterLastOnline) stmt.bind(input.last_online);

  mysqlx::SqlResult result = stmt.execute();

  std::vector<DeviceRecord> records;
  for (mysqlx::Row row : result.fetchAll()) {
    DeviceRecord r;
    r.id = static_cast<int>(row[0].get<int64_t>());
    r.device_id = row[1].get<std::string>();
    r.device_name = row[2].get<std::string>();
    r.ip_address = row[3].get<std::string>();
    r.last_online = row[4].get<std::string>();
    r.status = row[5].get<std::string>();
    records.push_back(std::move(r));
  }
  return records;
}

void DeviceRepository::deleteDevice(mysqlx::Session& session,
                                    const std::string& device_id) {
  requireNonEmpty("device_id", device_id);

  // 确保已选中目标数据库（防御性处理：连接 URL 可能已包含 schema，但这里再显式指定一次）。
  session.sql("USE attendanceserver").execute();

  // AttendanceRecord / AttendanceRecordArchive 虽然有同名 device_id 列，但未配置
  // 外键约束（仅作普通列保留），单表 DELETE 不会因约束失败，也不会级联。
  session.sql("DELETE FROM Device WHERE device_id = ?")
      .bind(device_id)
      .execute();
}

}  // namespace db
