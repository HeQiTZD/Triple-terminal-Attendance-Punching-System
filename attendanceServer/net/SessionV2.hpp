#pragma once

#include "net/SessionBase.hpp"

namespace net {

class Session : public SessionBase<boost::asio::ip::tcp::socket> {
 public:
  using SessionBase::SessionBase;

  void start() override;
  boost::asio::ip::tcp::socket& socket() override { return stream_; }
  const boost::asio::ip::tcp::socket& socket() const override { return stream_; }

 protected:
  void do_read() override;
  void do_write_front() override;

 private:
  void on_read_some(const boost::system::error_code& ec, std::size_t n);
  void on_write_done(const boost::system::error_code& ec);
};

}  // namespace net