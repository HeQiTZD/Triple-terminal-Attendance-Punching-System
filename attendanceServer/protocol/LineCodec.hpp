#pragma once

#include <optional>
#include <string_view>

#include <nlohmann/json.hpp>

namespace protocol {

// 将一行 UTF-8 文本解析为 JSON object；失败返回 nullopt（不向调用方抛异常）。
std::optional<nlohmann::json> parse_json_line(std::string_view line);

}  // namespace protocol
