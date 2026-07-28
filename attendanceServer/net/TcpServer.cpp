#include "net/TcpServer.hpp"

#include "net/SessionV2.hpp"
#include "net/TlsSessionV2.hpp"
#include "util/DebugLog.hpp"
#include "util/SystemError.hpp"

#include <boost/asio.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace net {

namespace {

[[noreturn]] void throwStartError(const char* step,
                                 const boost::system::error_code& ec,
                                 const std::string& bind_address,
                                 unsigned short port) {
  std::ostringstream os;
  os << "TcpServer::start " << step << ' ' << bind_address << ':' << port
     << " — " << util::format_boost_error(ec);
  throw std::runtime_error(os.str());
}

}  // namespace

TcpServer::TcpServer(Config cfg, ISessionHandler& session_handler)
    : cfg_(std::move(cfg)),
      handler_(session_handler),
      acceptor_(ioc_),
      accept_socket_(ioc_) {
  if (cfg_.tls.enabled) {
    init_ssl_context();
  }
}

TcpServer::~TcpServer() {
  stop();
}

void TcpServer::init_ssl_context() {
  ssl_ctx_ = std::make_unique<boost::asio::ssl::context>(
      boost::asio::ssl::context::tlsv12_server);

  // 设置证书和私钥
  ssl_ctx_->use_certificate_chain_file(cfg_.tls.cert_file);
  ssl_ctx_->use_private_key_file(cfg_.tls.key_file,
                                  boost::asio::ssl::context::pem);

  // 设置 CA 证书（用于验证客户端证书）
  if (!cfg_.tls.ca_file.empty()) {
    ssl_ctx_->load_verify_file(cfg_.tls.ca_file);
  }

  // 设置是否验证客户端证书
  if (cfg_.tls.verify_client) {
    ssl_ctx_->set_verify_mode(boost::asio::ssl::verify_peer |
                              boost::asio::ssl::verify_fail_if_no_peer_cert);
  } else {
    ssl_ctx_->set_verify_mode(boost::asio::ssl::verify_none);
  }

  // 设置密码套件
  SSL_CTX_set_cipher_list(ssl_ctx_->native_handle(),
                          "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256");

  std::cout << "TLS initialized with cert: " << cfg_.tls.cert_file << std::endl;
}

void TcpServer::start() {
  if (running_) {
    return;
  }

  if (ioc_.stopped()) {
    ioc_.restart();
  }
  accept_socket_ = boost::asio::ip::tcp::socket(ioc_);

  const boost::asio::ip::tcp::endpoint ep(
      boost::asio::ip::make_address_v4(cfg_.bind_address), cfg_.port);

  boost::system::error_code ec;
  acceptor_.open(ep.protocol(), ec);
  if (ec) {
    throwStartError("acceptor.open", ec, cfg_.bind_address, cfg_.port);
  }
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    boost::system::error_code ignored;
    acceptor_.close(ignored);
    throwStartError("reuse_address", ec, cfg_.bind_address, cfg_.port);
  }
  acceptor_.bind(ep, ec);
  if (ec) {
    boost::system::error_code ignored;
    acceptor_.close(ignored);
    throwStartError("bind", ec, cfg_.bind_address, cfg_.port);
  }
  acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    boost::system::error_code ignored;
    acceptor_.close(ignored);
    throwStartError("listen", ec, cfg_.bind_address, cfg_.port);
  }

  running_ = true;
  work_guard_.emplace(boost::asio::make_work_guard(ioc_));

  do_accept();

  handler_.on_listen_started(ioc_);

  std::size_t n = cfg_.worker_threads;
  if (n == 0) {
    n = std::max<std::size_t>(1, std::thread::hardware_concurrency());
  }
  workers_.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    workers_.emplace_back([this]() { ioc_.run(); });
  }

  std::cout << "Server started on " << cfg_.bind_address << ":" << cfg_.port;
  if (cfg_.tls.enabled) {
    std::cout << " (TLS enabled)";
  }
  std::cout << std::endl;
}

void TcpServer::stop() {
  if (!running_) {
    return;
  }
  running_ = false;

  boost::system::error_code ec;
  acceptor_.close(ec);

  work_guard_.reset();

  ioc_.stop();

  for (std::thread& w : workers_) {
    if (w.joinable()) {
      w.join();
    }
  }
  workers_.clear();
}

void TcpServer::do_accept() {
  acceptor_.async_accept(accept_socket_,
                         [this](const boost::system::error_code& ec) {
                           if (!running_) {
                             return;
                           }
                           if (!acceptor_.is_open()) {
                             return;
                           }

                           if (!ec) {
                             // 获取远端地址用于调试日志
                             boost::system::error_code ep_ec;
                             auto remote_ep = accept_socket_.remote_endpoint(ep_ec);
                             std::string remote_str = ep_ec ? "unknown"
                               : (remote_ep.address().to_string() + ":" +
                                  std::to_string(remote_ep.port()));
                             INFO_NET("新连接接入: " << remote_str
                                      << (cfg_.tls.enabled ? " (TLS)" : " (TCP)"));

                             if (cfg_.tls.enabled && ssl_ctx_) {
                               // TLS 模式
                               std::make_shared<TlsSession>(
                                   std::move(accept_socket_), *ssl_ctx_,
                                   handler_)
                                   ->start();
                             } else {
                               // 普通 TCP 模式
                               std::make_shared<Session>(
                                   std::move(accept_socket_), handler_)
                                   ->start();
                             }
                             accept_socket_ =
                                 boost::asio::ip::tcp::socket(ioc_);
                             do_accept();
                             return;
                           }

                           if (ec == boost::asio::error::operation_aborted) {
                             return;
                           }

                           do_accept();
                         });
}

}  // namespace net
