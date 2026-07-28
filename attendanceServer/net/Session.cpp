#include "net/SessionV2.hpp"

#include "util/DebugLog.hpp"

#include <boost/asio.hpp>

#include <iostream>

namespace net {

void Session::start() {
  if (closed_) return;
  boost::system::error_code ec;
  auto ep = stream_.remote_endpoint(ec);
  if (!ec) {
    DBG_SESSION("会话启动: " << ep.address().to_string()
                << ":" << ep.port());
  }
  do_read();
}

void Session::do_read() {
  auto self = std::static_pointer_cast<Session>(shared_from_this());
  stream_.async_read_some(
      boost::asio::buffer(read_buf_),
      boost::asio::bind_executor(
          strand_, [self](const boost::system::error_code& ec,
                         std::size_t bytes_transferred) {
            self->on_read_some(ec, bytes_transferred);
          }));
}

void Session::on_read_some(const boost::system::error_code& ec,
                           std::size_t n) {
  if (closed_) return;

  if (ec) {
    if (ec == boost::asio::error::operation_aborted) return;
    if (ec == boost::asio::error::eof ||
        ec == boost::asio::error::connection_reset) {
      clean_close();
      return;
    }
    WARN_NET("TCP 读取错误: ec=" << ec.value() << " msg=" << ec.message());
    fail("tcp_read_error");
    return;
  }

  process_received_bytes(read_buf_.data(), n);

  if (!closed_) do_read();
}

void Session::do_write_front() {
  if (closed_ || write_queue_.empty()) {
    write_in_progress_ = false;
    return;
  }

  auto self = std::static_pointer_cast<Session>(shared_from_this());
  auto& buf = write_queue_.front();
  boost::asio::async_write(
      stream_, boost::asio::buffer(buf.data(), buf.size()),
      boost::asio::bind_executor(
          strand_, [self](const boost::system::error_code& ec,
                          std::size_t /*bytes_sent*/) {
            self->on_write_done(ec);
          }));
}

void Session::on_write_done(const boost::system::error_code& ec) {
  if (closed_) {
    reset_writes();
    return;
  }

  if (ec) {
    reset_writes();
    if (ec != boost::asio::error::operation_aborted) {
      fail("tcp_write_error");
    }
    return;
  }

  write_queue_.pop_front();
  if (write_queue_.empty()) {
    write_in_progress_ = false;
    return;
  }
  do_write_front();
}

}  // namespace net
