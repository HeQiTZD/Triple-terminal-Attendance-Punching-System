#pragma once

#include "net/SessionBase.hpp"

#include <boost/asio/ssl.hpp>

namespace net {

class TlsSession : public SessionBase<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> {
 public:
  using base_type = SessionBase<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>;
  using base_type::base_type;

  TlsSession(boost::asio::ip::tcp::socket socket,
              boost::asio::ssl::context& ssl_ctx,
              ISessionHandler& handler)
      : base_type(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(
                      std::move(socket), ssl_ctx),
                  handler) {}

  void start() override;
  boost::asio::ip::tcp::socket& socket() override { return stream_.next_layer(); }
  const boost::asio::ip::tcp::socket& socket() const override { return stream_.next_layer(); }

 protected:
  void do_read() override;
  void do_write_front() override;

 private:
  void on_handshake(const boost::system::error_code& ec);
  void on_read_some(const boost::system::error_code& ec, std::size_t n);
  void on_write_done(const boost::system::error_code& ec);
};

}  // namespace net
