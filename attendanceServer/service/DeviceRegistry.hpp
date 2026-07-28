#pragma once

#include "service/IDeviceRegistry.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "net/Session.hpp"

namespace service {

// deviceId -> Session（weak）；单播 / 广播雏形。线程安全。
class DeviceRegistry : public IDeviceRegistry {
 public:
  DeviceRegistry() = default;

  DeviceRegistry(const DeviceRegistry&) = delete;
  DeviceRegistry& operator=(const DeviceRegistry&) = delete;

  // 实现 IDeviceRegistry 接口
  DeviceRegisterResult register_or_replace(
      const std::string& device_id,
      std::weak_ptr<net::ISession> weak_sess,
      DuplicateDevicePolicy policy) override;

  void unregister(const std::string& device_id,
                  const net::ISession* raw) override;

  bool has_live_session(const std::string& device_id) override;

  void with_session(const std::string& device_id,
                    std::function<void(net::ISession&)> fn) override;

  template <typename Fn>
  void for_each_device(Fn&& fn) {
    std::vector<std::shared_ptr<net::ISession>> snapshot;
    {
      std::lock_guard<std::mutex> lk(mu_);
      snapshot.reserve(devices_.size());
      for (auto it = devices_.begin(); it != devices_.end();) {
        if (auto sp = it->second.lock()) {
          snapshot.push_back(std::move(sp));
          ++it;
        } else {
          it = devices_.erase(it);
        }
      }
    }
    for (const auto& sp : snapshot) {
      fn(*sp);
    }
  }

 private:
  mutable std::mutex mu_;
  std::unordered_map<std::string, std::weak_ptr<net::ISession>> devices_;
};

}  // namespace service
