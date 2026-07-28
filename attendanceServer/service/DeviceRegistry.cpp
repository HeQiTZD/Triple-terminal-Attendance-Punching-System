#include "service/DeviceRegistry.hpp"

#include <iostream>

namespace service {

DeviceRegisterResult DeviceRegistry::register_or_replace(
    const std::string& device_id, std::weak_ptr<net::ISession> weak_sess,
    DuplicateDevicePolicy policy) {
  auto locked_new = weak_sess.lock();
  if (!locked_new) {
    return {DeviceRegisterStatus::Ok};
  }

  std::lock_guard<std::mutex> lk(mu_);
  auto it = devices_.find(device_id);
  if (it != devices_.end()) {
    if (auto old = it->second.lock()) {
      if (old.get() == locked_new.get()) {
        it->second = weak_sess;
        return {DeviceRegisterStatus::Ok};
      }
      if (policy == DuplicateDevicePolicy::RejectNew) {
        return {DeviceRegisterStatus::RejectedDuplicate};
      }

      std::string old_ep = "?";
      std::string new_ep = "?";
      boost::system::error_code ec;
      auto e_old = old->socket().remote_endpoint(ec);
      if (!ec) {
        old_ep = e_old.address().to_string() + ":" +
                 std::to_string(e_old.port());
      }
      ec.clear();
      auto e_new = locked_new->socket().remote_endpoint(ec);
      if (!ec) {
        new_ep = e_new.address().to_string() + ":" +
                 std::to_string(e_new.port());
      }

      std::cerr << "device_registry: duplicate_login policy=kick_old deviceId="
                << device_id << " old_endpoint=" << old_ep
                << " new_endpoint=" << new_ep << '\n';

      old->request_close("duplicate_device_login");
    } else {
      devices_.erase(it);
    }
  }

  devices_[device_id] = weak_sess;
  return {DeviceRegisterStatus::Ok};
}

void DeviceRegistry::unregister(const std::string& device_id,
                                const net::ISession* raw) {
  std::lock_guard<std::mutex> lk(mu_);
  auto it = devices_.find(device_id);
  if (it == devices_.end()) {
    return;
  }
  if (auto sp = it->second.lock()) {
    if (sp.get() != raw) {
      return;
    }
  }
  devices_.erase(it);
}

bool DeviceRegistry::has_live_session(const std::string& device_id) {
  std::lock_guard<std::mutex> lk(mu_);
  auto it = devices_.find(device_id);
  if (it == devices_.end()) {
    return false;
  }
  if (auto sp = it->second.lock()) {
    (void)sp;
    return true;
  }
  devices_.erase(it);
  return false;
}

void DeviceRegistry::with_session(const std::string& device_id,
                                  std::function<void(net::ISession&)> fn) {
  std::shared_ptr<net::ISession> sp;
  {
    std::lock_guard<std::mutex> lk(mu_);
    auto it = devices_.find(device_id);
    if (it == devices_.end()) {
      return;
    }
    sp = it->second.lock();
    if (!sp) {
      devices_.erase(it);
      return;
    }
  }
  fn(*sp);
}

}  // namespace service
