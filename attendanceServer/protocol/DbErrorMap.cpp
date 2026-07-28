#include "protocol/DbErrorMap.hpp"

#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace protocol {

namespace {

constexpr unsigned kErDupEntry = 1062;
constexpr unsigned kErNoReferencedRow = 1452;
constexpr unsigned kErRowIsReferenced = 1451;

// mysqlx::Error 仅为 common::Error（runtime_error），无 SQL errno API；多数驱动会把 (errno) 写进 what()。
std::optional<unsigned> errno_from_parentheses(std::string_view w) {
  for (std::size_t i = 0; i < w.size(); ++i) {
    if (w[i] != '(') {
      continue;
    }
    unsigned val = 0;
    std::size_t j = i + 1;
    bool any = false;
    while (j < w.size() &&
           std::isdigit(static_cast<unsigned char>(w[j])) != 0) {
      any = true;
      val = val * 10u + static_cast<unsigned>(w[j] - '0');
      ++j;
    }
    if (any && j < w.size() && w[j] == ')') {
      return val;
    }
  }
  return std::nullopt;
}

bool icontains(std::string_view haystack, const char* needle_lower_ascii) {
  std::string h(haystack);
  for (char& c : h) {
    c = static_cast<char>(
        std::tolower(static_cast<unsigned char>(c)));
  }
  return h.find(needle_lower_ascii) != std::string::npos;
}

unsigned guess_errno_from_message(std::string_view w) {
  if (icontains(w, "duplicate")) {
    return kErDupEntry;
  }
  if (icontains(w, "foreign key") ||
      icontains(w, "cannot add or update a child row")) {
    return kErNoReferencedRow;
  }
  return 0;
}

}  // namespace

MappedError map_mysqlx_error(const mysqlx::Error& e) {
  MappedError out;
  const char* raw = e.what();
  const std::string_view w = raw ? std::string_view(raw) : std::string_view();

  unsigned err_code = 0;
  if (const auto parsed = errno_from_parentheses(w)) {
    err_code = *parsed;
  } else {
    err_code = guess_errno_from_message(w);
  }

  if (err_code == kErDupEntry) {
    out.code = 6001;
    out.msg = "duplicate key";
    return out;
  }
  if (err_code == kErNoReferencedRow || err_code == kErRowIsReferenced) {
    out.code = 4001;
    out.msg = "referenced record does not exist";
    return out;
  }

  out.code = 6002;
  out.msg = raw ? std::string(raw) : std::string("database error");
  return out;
}

}  // namespace protocol
