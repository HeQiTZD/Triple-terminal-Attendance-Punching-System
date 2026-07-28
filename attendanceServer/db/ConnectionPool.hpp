#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>

#include <mysqlx/xdevapi.h>

#include "db/DbClient.hpp"
#include "db/DbConfig.hpp"

namespace db {

// 固定大小连接池。acquire() 阻塞等待可用连接；release() 做健康检查，
// 失败则自动重建连接再归还。析构时关闭所有连接。
class ConnectionPool {
 public:
  ConnectionPool(const DbConfig& cfg, std::size_t pool_size)
      : cfg_(cfg), max_size_(std::max<std::size_t>(1, pool_size)) {
    for (std::size_t i = 0; i < max_size_; ++i) {
      pool_.push(std::make_shared<mysqlx::Session>(
          DbClient::connection_url(cfg)));
    }
  }

  ~ConnectionPool() {
    std::lock_guard<std::mutex> lk(mu_);
    while (!pool_.empty()) {
      try {
        pool_.front()->close();
      } catch (...) {
      }
      pool_.pop();
    }
  }

  ConnectionPool(const ConnectionPool&) = delete;
  ConnectionPool& operator=(const ConnectionPool&) = delete;

  // 阻塞获取一个可用连接。池空时等待 release() 归还。
  std::shared_ptr<mysqlx::Session> acquire() {
    std::unique_lock<std::mutex> lk(mu_);
    cv_.wait(lk, [this] { return !pool_.empty(); });
    auto sess = std::move(pool_.front());
    pool_.pop();
    return sess;
  }

  // 归还连接。先做 SELECT 1 健康检查，失败则重建连接。
  void release(std::shared_ptr<mysqlx::Session> sess) {
    if (!sess) return;
    bool healthy = false;
    try {
      sess->sql("SELECT 1").execute();
      healthy = true;
    } catch (...) {
    }
    if (!healthy) {
      try {
        sess = std::make_shared<mysqlx::Session>(
            DbClient::connection_url(cfg_));
      } catch (...) {
        return;
      }
    }
    std::lock_guard<std::mutex> lk(mu_);
    pool_.push(std::move(sess));
    cv_.notify_one();
  }

  // 当前池中可用连接数（用于监控/测试）
  std::size_t available() const {
    std::lock_guard<std::mutex> lk(mu_);
    return pool_.size();
  }

  // 池容量
  std::size_t capacity() const noexcept { return max_size_; }

 private:
  DbConfig cfg_;
  std::size_t max_size_;
  mutable std::mutex mu_;
  std::condition_variable cv_;
  std::queue<std::shared_ptr<mysqlx::Session>> pool_;
};

}  // namespace db
