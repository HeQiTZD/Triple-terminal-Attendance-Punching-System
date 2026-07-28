#include "service/AdminRegistry.hpp"

#include <algorithm>

namespace service {

void AdminRegistry::compact_unlocked(const std::string& operator_id) {
  auto it = sessions_.find(operator_id);
  if (it == sessions_.end()) {
    return;
  }
  auto& vec = it->second;
  vec.erase(std::remove_if(vec.begin(), vec.end(),
                           [](const std::weak_ptr<net::ISession>& w) {
                             return w.expired();
                           }),
            vec.end());
  if (vec.empty()) {
    sessions_.erase(it);
  }
}

void AdminRegistry::register_session(const std::string& operator_id,
                                     std::weak_ptr<net::ISession> weak_sess) {
  if (operator_id.empty()) {
    return;
  }
  std::lock_guard<std::mutex> lk(mu_);
  compact_unlocked(operator_id);
  sessions_[operator_id].push_back(std::move(weak_sess));
}

void AdminRegistry::unregister_session(const std::string& operator_id,
                                       const net::ISession* raw) {
  if (!raw || operator_id.empty()) {
    return;
  }
  std::lock_guard<std::mutex> lk(mu_);
  auto it = sessions_.find(operator_id);
  if (it == sessions_.end()) {
    return;
  }
  auto& vec = it->second;
  vec.erase(std::remove_if(vec.begin(), vec.end(),
                           [raw](const std::weak_ptr<net::ISession>& w) {
                             auto sp = w.lock();
                             return !sp || sp.get() == raw;
                           }),
            vec.end());
  if (vec.empty()) {
    sessions_.erase(it);
  }
}

}  // namespace service
