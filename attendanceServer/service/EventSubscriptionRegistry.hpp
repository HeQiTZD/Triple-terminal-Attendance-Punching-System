#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "net/Session.hpp"

namespace service {

// 管理端 subscribe 主题分发骨架（通信协议 §5.3 / §10.3）。
class EventSubscriptionRegistry {
 public:
  EventSubscriptionRegistry() = default;
  EventSubscriptionRegistry(const EventSubscriptionRegistry&) = delete;
  EventSubscriptionRegistry& operator=(const EventSubscriptionRegistry&) =
      delete;

  void subscribe(net::ISession* raw, const std::vector<std::string>& topics);
  void unsubscribe_all(net::ISession* raw);
  void publish(const std::string& topic, const std::string& json_line);

 private:
  void unsubscribe_all_unlocked(net::ISession* raw);

  std::mutex mu_;
  std::unordered_map<std::string, std::vector<std::weak_ptr<net::ISession>>>
      topic_subscribers_;
  std::unordered_map<net::ISession*, std::unordered_set<std::string>>
      session_topics_;
};

}  // namespace service
