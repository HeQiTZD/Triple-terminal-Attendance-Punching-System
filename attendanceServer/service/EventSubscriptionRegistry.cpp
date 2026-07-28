#include "service/EventSubscriptionRegistry.hpp"

#include <algorithm>

namespace service {

void EventSubscriptionRegistry::unsubscribe_all_unlocked(net::ISession* raw) {
  if (!raw) {
    return;
  }
  auto it_st = session_topics_.find(raw);
  if (it_st == session_topics_.end()) {
    return;
  }
  for (const std::string& t : it_st->second) {
    auto it_top = topic_subscribers_.find(t);
    if (it_top == topic_subscribers_.end()) {
      continue;
    }
    auto& vec = it_top->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                             [raw](const std::weak_ptr<net::ISession>& w) {
                               auto s = w.lock();
                               return !s || s.get() == raw;
                             }),
              vec.end());
    if (vec.empty()) {
      topic_subscribers_.erase(it_top);
    }
  }
  session_topics_.erase(it_st);
}

void EventSubscriptionRegistry::subscribe(
    net::ISession* raw, const std::vector<std::string>& topics) {
  if (!raw || topics.empty()) {
    return;
  }
  auto sp = raw->weak_from_this().lock();
  if (!sp) {
    return;
  }

  std::lock_guard<std::mutex> lk(mu_);
  unsubscribe_all_unlocked(raw);

  auto& st = session_topics_[raw];
  for (const std::string& t : topics) {
    if (t.empty()) {
      continue;
    }
    topic_subscribers_[t].push_back(sp);
    st.insert(t);
  }
}

void EventSubscriptionRegistry::unsubscribe_all(net::ISession* raw) {
  std::lock_guard<std::mutex> lk(mu_);
  unsubscribe_all_unlocked(raw);
}

void EventSubscriptionRegistry::publish(const std::string& topic,
                                        const std::string& json_line) {
  std::vector<std::shared_ptr<net::ISession>> targets;
  {
    std::lock_guard<std::mutex> lk(mu_);
    auto it = topic_subscribers_.find(topic);
    if (it == topic_subscribers_.end()) {
      return;
    }
    auto& vec = it->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                             [](const std::weak_ptr<net::ISession>& w) {
                               return w.expired();
                             }),
              vec.end());
    for (const auto& w : vec) {
      if (auto sp = w.lock()) {
        targets.push_back(sp);
      }
    }
    if (vec.empty()) {
      topic_subscribers_.erase(it);
    }
  }
  for (const auto& sp : targets) {
    sp->write_line(json_line);
  }
}

}  // namespace service
