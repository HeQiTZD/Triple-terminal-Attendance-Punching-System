#pragma once

#include <string>
#include <vector>

#include <mysqlx/xdevapi.h>

namespace db {

struct DeviceCreateInput {
  // 必填字段：为空则抛 std::invalid_argument。
  std::string device_id;     // 设备唯一标识（UNIQUE）
  std::string device_name;   // 设备显示名
  // 可选字段：传空字符串表示“不提供该列”。
  // - ip_address / last_online 为空 → 写入 NULL（schema 允许 NULL）。
  // - status 为空 → 使用 schema 默认值 'offline'。
  std::string ip_address;    // 形如 "192.168.1.10"
  std::string last_online;   // 最近在线时间，'YYYY-MM-DD HH:MM:SS' 字符串
  std::string status;        // 'online' / 'offline' 等业务约定值
};

struct DeviceUpdateInput {
  // 约定：字段传空字符串表示“不更新该字段”。
  // status：如 'online' / 'offline'（cpp-server-dev §6.3 认证成功写 online）。
  std::string device_name;
  std::string ip_address;
  std::string last_online;  // 'YYYY-MM-DD HH:MM:SS' 字符串；若 last_online_now 为 true 则忽略本字段
  std::string status;
  // 为 true 时在 SQL 中使用 NOW() 更新 last_online（与绑定字符串二选一）。
  bool last_online_now = false;
};

struct DeviceQueryInput {
  // 约定：字段传空字符串表示“不作为该字段的过滤条件”。
  // 所有非空字段以 AND 方式组合。
  // - device_id / device_name / ip_address：精确匹配。
  // - last_online：按 'YYYY-MM-DD HH:MM:SS' 格式做前缀匹配，
  //   例如传 "2026-05-08" 可命中该日全部记录，传完整时间字符串等价于精确匹配。
  std::string device_id;
  std::string device_name;
  std::string ip_address;
  std::string last_online;
};

struct DeviceRecord {
  int id = 0;
  std::string device_id;
  std::string device_name;   // 表中可空，NULL 会以空串返回
  std::string ip_address;    // 表中可空，NULL 会以空串返回
  std::string last_online;   // 'YYYY-MM-DD HH:MM:SS' 字符串；NULL 以空串返回
  std::string status;        // 'online' / 'offline' 等业务约定值；NULL 以空串返回
};

class DeviceRepository {
 public:
  // 新增设备：
  // - device_id / device_name 不能为空，否则抛 std::invalid_argument。
  // - ip_address / last_online 为空字符串时，对应列写入 NULL。
  // - status 为空字符串时，使用 schema 默认值 'offline'。
  // - 数据库层错误（例如 device_id 重复、last_online 格式非法）会抛 mysqlx::Error。
  static void insertDevice(mysqlx::Session& session,
                           const DeviceCreateInput& input);

  // 更新设备信息：根据 device_id 定位，仅更新传入的非空字段。
  // - device_id 为空 / 无任何有效更新项时抛 std::invalid_argument。
  // - 若该 device_id 不存在，UPDATE 影响 0 行，不抛异常。
  // - last_online 仍以 'YYYY-MM-DD HH:MM:SS' 字符串形式传入，由 MySQL 隐式
  //   转换成 TIMESTAMP；格式非法时会以 mysqlx::Error 抛出。
  static void updateDevice(mysqlx::Session& session,
                           const std::string& device_id,
                           const DeviceUpdateInput& input);

  // 认证上线：status='online'、last_online=NOW()；可选更新 ip_address。
  // 若无该行则插入占位（device_name 暂用 device_id），避免必须先手工建设备。
  static void upsertDeviceOnline(mysqlx::Session& session,
                                 const std::string& device_id,
                                 const std::string& ip_address);

  // 查询单个设备状态；不存在时返回空字符串。
  static std::string getDeviceStatus(mysqlx::Session& session,
                                     const std::string& device_id);

  // 心跳超时或断连：status='offline'（不修改 last_online，保留上次在线时间）。
  static void markDeviceOffline(mysqlx::Session& session,
                                const std::string& device_id);

  // 查询设备：根据 input 中的非空字段拼接 WHERE 条件（AND 组合）。
  // 若所有字段均为空，则返回全部记录。结果按 id 升序排列。
  static std::vector<DeviceRecord> selectDevice(
      mysqlx::Session& session, const DeviceQueryInput& input);

  // 删除设备：仅按 device_id 删除 Device 表中的对应行。
  // - AttendanceRecord / AttendanceRecordArchive 中虽存有 device_id 列，但未配
  //   置外键，因此单表 DELETE 不会因约束失败，也不会级联到其它表。
  // - 若该 device_id 不存在则不抛异常（与 deleteUser/deletePerson 的语义一致）。
  // - device_id 为空时抛 std::invalid_argument。
  static void deleteDevice(mysqlx::Session& session,
                           const std::string& device_id);
};

}  // namespace db
