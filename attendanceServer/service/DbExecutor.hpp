#pragma once

#include <exception>
#include <functional>
#include <memory>
#include <mutex>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include <mysqlx/xdevapi.h>

#include "db/ConnectionPool.hpp"
#include "db/DbConfig.hpp"
#include "service/IDbExecutor.hpp"

namespace service {

// 使用 boost::asio::thread_pool + ConnectionPool：池线程从连接池获取连接，
// 归还时自动健康检查 + 断连重建。连接数 = pool_size，不随线程数膨胀。
class DbExecutor : public IDbExecutor {
 public:
  // 原有构造函数：内部创建 ConnectionPool（pool_size 默认等于 pool_threads）。
  explicit DbExecutor(const db::DbConfig& config,
                      std::size_t pool_threads = 0);

  // 新构造函数：注入外部 ConnectionPool（便于测试和共享连接池）。
  DbExecutor(std::shared_ptr<db::ConnectionPool> pool,
             std::size_t pool_threads = 0);

  virtual ~DbExecutor();

  DbExecutor(const DbExecutor&) = delete;
  DbExecutor& operator=(const DbExecutor&) = delete;

  void stop();

  void dispatch(Work work, Done done) override;

 protected:
  // 无参构造：不创建连接池（供 MockDbExecutor 继承使用）。
  DbExecutor() = default;

 private:
  std::shared_ptr<db::ConnectionPool> conn_pool_;
  std::unique_ptr<boost::asio::thread_pool> pool_;
  std::mutex mu_;
  bool stopped_ = false;
};

}  // namespace service
