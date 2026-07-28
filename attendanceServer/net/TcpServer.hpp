#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

#include "net/TlsConfig.hpp"

namespace net {

class ISessionHandler;

// Plan F：监听 + worker 线程；每个新连接创建 Session 并 start()。
class TcpServer {
 public:
  struct Config {
    std::string bind_address = "0.0.0.0";
    unsigned short port = 8080;
    // 0 表示 max(1, hardware_concurrency())
    std::size_t worker_threads = 0;
    TlsConfig tls;
  };

  TcpServer(Config cfg, ISessionHandler& session_handler);
  ~TcpServer();

  TcpServer(const TcpServer&) = delete;
  TcpServer& operator=(const TcpServer&) = delete;

  // 绑定、监听、投递 accept、启动 worker；重复调用无效。
  // 端口占用等失败抛出 std::runtime_error。
  void start();

  // 关闭 acceptor、撤掉 work_guard、stop io_context、join 线程；可重复调用。
  void stop();

  boost::asio::io_context& io_context() { return ioc_; }
  bool is_running() const { return running_; }

 private:
  void do_accept();
  void init_ssl_context();

  Config cfg_;
  ISessionHandler& handler_;
  boost::asio::io_context ioc_;
  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ip::tcp::socket accept_socket_;

  std::unique_ptr<boost::asio::ssl::context> ssl_ctx_;

  using work_guard_t =
      boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
  std::optional<work_guard_t> work_guard_;

  std::vector<std::thread> workers_;
  bool running_ = false;
};

}  // namespace net
