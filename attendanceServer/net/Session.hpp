#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace net {

class ISession;

// 网络层回调：不包含 JSON 解析（plan C3）。
class ISessionHandler {
 public:
  virtual ~ISessionHandler() = default;

  // 收到一条完整文本行（已去掉 LF / 行尾 CR）；空行不回调（plan「非空行」）。
  virtual void on_line(ISession& session, std::string line) = 0;

  // 收到一整段二进制帧（长度由先前 begin_expect_binary 声明）。
  virtual void on_binary(ISession& session, std::vector<char> payload) = 0;

  // recv 缓冲区超限、单行过长、TCP 读失败等非 EOF 错误。
  virtual void on_error(ISession& session, const char* reason) = 0;

  // 连接已关闭（含对端 EOF / reset）；每个 Session 至多一次。
  virtual void on_close(ISession& session) = 0;

  // TcpServer::start() 在绑定监听并开始 accept 之后调用一次（worker 线程启动前）。
  virtual void on_listen_started(boost::asio::io_context& /*ioc*/) {}
};

// Session 和 TlsSession 的公共接口
class ISession : public std::enable_shared_from_this<ISession> {
 public:
  virtual ~ISession() = default;

  virtual void start() = 0;

  virtual boost::asio::ip::tcp::socket& socket() = 0;
  virtual const boost::asio::ip::tcp::socket& socket() const = 0;

  virtual void write_line(std::string text) = 0;
  virtual void write_binary(std::vector<char> payload) = 0;
  virtual void begin_expect_binary(std::uint32_t byte_count) = 0;
  virtual void request_close(const char* reason) = 0;

  using strand_type =
      boost::asio::strand<boost::asio::ip::tcp::socket::executor_type>;
  virtual strand_type& strand() = 0;
};

}  // namespace net