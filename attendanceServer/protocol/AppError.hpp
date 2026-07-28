#pragma once

#include <stdexcept>
#include <string>

namespace protocol {

// 与 protocol/ProtocolTypes.hpp 中的 kCode* 常量对齐。
enum class ErrorCode {
  kParseError = 1001,
  kPayloadTooLarge = 1002,
  kNotAuthenticated = 2001,
  kAuthFailed = 2002,
  kDuplicateSession = 2003,
  kTokenInvalid = 2004,
  kTokenExpired = 2005,
  kRefreshTokenInvalid = 2006,
  kInsufficientPermissions = 2007,
  kForbidden = 3001,
  kBusinessValidation = 4000,
  kEmployeeNotFound = 4001,
  kDeviceOffline = 5001,
  kForwardTimeout = 5002,
  kDuplicateKey = 6001,
  kDbError = 6002,
};

// 统一业务异常：继承 std::runtime_error，现有 catch (const std::exception&) 全兼容。
class AppError : public std::runtime_error {
 public:
  AppError(ErrorCode code, std::string message)
      : std::runtime_error(std::move(message)), code_(code) {}

  ErrorCode code() const noexcept { return code_; }

  // 与 protocol::build_error(int, ...) 兼容的数值码
  int numeric_code() const noexcept { return static_cast<int>(code_); }

 private:
  ErrorCode code_;
};

}  // namespace protocol
