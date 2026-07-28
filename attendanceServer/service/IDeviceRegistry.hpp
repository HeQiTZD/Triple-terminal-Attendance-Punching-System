#pragma once

#include <functional>
#include <memory>
#include <string>

namespace net {
class ISession;
}

namespace service {

enum class DuplicateDevicePolicy { KickOld, RejectNew };
enum class DeviceRegisterStatus { Ok, RejectedDuplicate };

struct DeviceRegisterResult {
  DeviceRegisterStatus status;
};

class IDeviceRegistry {
 public:
  virtual ~IDeviceRegistry() = default;

  virtual DeviceRegisterResult register_or_replace(
      const std::string& device_id,
      std::weak_ptr<net::ISession> weak_sess,
      DuplicateDevicePolicy policy) = 0;

  virtual void unregister(const std::string& device_id,
                          const net::ISession* raw) = 0;

  virtual bool has_live_session(const std::string& device_id) = 0;

  virtual void with_session(const std::string& device_id,
                            std::function<void(net::ISession&)> fn) = 0;
};

}  // namespace service
