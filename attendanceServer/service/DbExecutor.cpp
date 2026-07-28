#include "service/DbExecutor.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <utility>

#include "db/DbClient.hpp"

namespace service {

DbExecutor::DbExecutor(const db::DbConfig& config, std::size_t pool_threads)
    : DbExecutor(std::make_shared<db::ConnectionPool>(
                     config,
                     pool_threads == 0
                         ? std::max<std::size_t>(
                               1, std::thread::hardware_concurrency())
                         : pool_threads),
                 pool_threads) {}

DbExecutor::DbExecutor(std::shared_ptr<db::ConnectionPool> pool,
                       std::size_t pool_threads)
    : conn_pool_(std::move(pool)) {
  std::size_t n = pool_threads;
  if (n == 0) {
    n = std::max<std::size_t>(1, std::thread::hardware_concurrency());
  }
  pool_ = std::make_unique<boost::asio::thread_pool>(n);
}

DbExecutor::~DbExecutor() { stop(); }

void DbExecutor::stop() {
  std::unique_ptr<boost::asio::thread_pool> pool;
  {
    std::lock_guard<std::mutex> lk(mu_);
    if (stopped_) {
      return;
    }
    stopped_ = true;
    pool = std::move(pool_);
  }
  if (pool) {
    pool->join();
  }
}

void DbExecutor::dispatch(Work work, Done done) {
  std::lock_guard<std::mutex> lk(mu_);
  if (stopped_ || !pool_) {
    throw std::runtime_error("DbExecutor::dispatch after stop");
  }
  auto pool = conn_pool_;
  boost::asio::post(*pool_, [pool, work = std::move(work),
                              done = std::move(done)]() mutable {
    auto session = pool->acquire();
    std::exception_ptr ep;
    try {
      work(*session);
    } catch (...) {
      ep = std::current_exception();
    }
    pool->release(std::move(session));
    if (static_cast<bool>(done)) {
      try {
        done(ep);
      } catch (const std::exception& ex) {
        std::cerr << "DbExecutor: completion callback failed: " << ex.what()
                  << '\n';
      } catch (...) {
        std::cerr << "DbExecutor: completion callback failed: unknown\n";
      }
    }
  });
}

}  // namespace service
