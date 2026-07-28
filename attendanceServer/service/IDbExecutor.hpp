#pragma once

#include <exception>
#include <functional>

namespace mysqlx {
inline namespace abi2 {
inline namespace r0 {
class Session;
}
}
}

namespace service {

class IDbExecutor {
 public:
  using Work = std::function<void(mysqlx::Session&)>;
  using Done = std::function<void(std::exception_ptr)>;

  virtual ~IDbExecutor() = default;

  virtual void dispatch(Work work, Done done) = 0;
};

}  // namespace service
