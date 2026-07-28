#include "protocol/LineCodec.hpp"

#include <cctype>
#include <string>

namespace protocol {

namespace {

bool is_blank(std::string_view s) {
  for (unsigned char c : s) {
    if (!std::isspace(c)) {
      return false;
    }
  }
  return true;
}

}  // namespace

std::optional<nlohmann::json> parse_json_line(std::string_view line) {
  if (line.empty() || is_blank(line)) {
    return std::nullopt;
  }

  try {
    nlohmann::json j = nlohmann::json::parse(line.begin(), line.end());
    if (!j.is_object()) {
      return std::nullopt;
    }
    return j;
  } catch (const nlohmann::json::exception&) {
    return std::nullopt;
  }
}

}  // namespace protocol
