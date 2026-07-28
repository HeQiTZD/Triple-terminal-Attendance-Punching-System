#pragma once

#include "config/ServerConfig.hpp"

#include <iosfwd>
#include <string>

namespace config {

bool isInteractiveTerminal();

void printConfigDetail(const ServerConfig& cfg, std::ostream& os);

// 交互式配置菜单。返回 true 表示保存并启动；false 表示用户选择退出（不启动）。
bool runConsoleConfigUi(ServerConfig& cfg, const std::string& config_path);

}  // namespace config
