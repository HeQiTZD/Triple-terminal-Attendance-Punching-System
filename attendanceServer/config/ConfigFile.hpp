#pragma once

#include "config/ServerConfig.hpp"

#include <string>

namespace config {

// 默认配置文件路径（当前工作目录下的 attendance.json）。
std::string defaultConfigPath();

// 解析配置文件路径：ATTENDANCE_CONFIG_FILE → --config PATH → defaultConfigPath()。
// 仅从 argv 读取 --config，不修改 ServerConfig。
std::string resolveConfigPath(int argc, char** argv);

// 从 JSON 合并到 cfg（仅覆盖文件中出现的字段）。文件不存在返回 false（非错误）。
// 解析失败返回 false 且 err 非空。
bool loadFromJson(const std::string& path, ServerConfig& cfg, std::string& err);

// 写入 JSON。成功返回 true。
bool saveToJson(const std::string& path, const ServerConfig& cfg,
                std::string& err);

}  // namespace config
