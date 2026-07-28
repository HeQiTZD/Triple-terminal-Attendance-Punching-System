#pragma once

#include <string>

namespace db {

struct DbConfig {
  std::string host = "localhost";
  int port = 33060;  // MySQL X Protocol default
  std::string user = "root";
  std::string password = "root";

  // Optional: MySQL schema (database name). Leave empty if not needed.
  std::string schema = "attendanceserver";
};

}  // namespace db

