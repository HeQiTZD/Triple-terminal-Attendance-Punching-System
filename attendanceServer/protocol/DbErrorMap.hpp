#pragma once

#include <string>

#include <mysqlx/xdevapi.h>

namespace protocol {

// Maps mysqlx::Error to protocol §8 codes + client-facing msg.
// Connector/C++ 的 mysqlx::Error 无 errno API，由 map_mysqlx_error 从 what() 解析 (errno) 或关键字回退。
struct MappedError {
  int code = 6002;
  std::string msg = "database error";
};

MappedError map_mysqlx_error(const mysqlx::Error& e);

}  // namespace protocol
