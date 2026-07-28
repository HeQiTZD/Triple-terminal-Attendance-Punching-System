#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "net/Session.hpp"

namespace service {

// operatorId -> 多端会话（weak）；用于管理端在线表（通信协议 §6）。
class AdminRegistry {
 public:
  AdminRegistry() = default;
  AdminRegistry(const AdminRegistry&) = delete;
  AdminRegistry& operator=(const AdminRegistry&) = delete;

  void register_session(const std::string& operator_id,
                        std::weak_ptr<net::ISession> weak_sess);
  void unregister_session(const std::string& operator_id,
                          const net::ISession* raw);

  template <typename Fn>
  void with_session(const std::string& operator_id, Fn&& fn) {
    std::shared_ptr<net::ISession> sp;
    {
      std::lock_guard<std::mutex> lk(mu_);
      auto it = sessions_.find(operator_id);
      if (it == sessions_.end()) {
        return;
      }
      for (auto w : it->second) {
        if (sp = w.lock()) {
          break;
        }
      }
      if (!sp) {
        sessions_.erase(it);
        return;
      }
    }
    fn(*sp);
  }

 private:
  void compact_unlocked(const std::string& operator_id);

  mutable std::mutex mu_;
  std::unordered_map<std::string, std::vector<std::weak_ptr<net::ISession>>>
      sessions_;
};

}  // namespace service
