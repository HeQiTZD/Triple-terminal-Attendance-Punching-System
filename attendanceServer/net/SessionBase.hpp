#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "net/Framing.hpp"
#include "net/Session.hpp"  // ISession, ISessionHandler
#include "util/DebugLog.hpp"

namespace net {

template <typename Stream>
class SessionBase : public ISession {
 public:
  SessionBase(Stream stream, ISessionHandler& handler)
      : stream_(std::move(stream)),
        strand_(boost::asio::make_strand(stream_.lowest_layer().get_executor())),
        handler_(handler) {}

  // ISession 接口
  strand_type& strand() override { return strand_; }

  void write_line(std::string text) override {
    auto self = std::static_pointer_cast<SessionBase<Stream>>(ISession::shared_from_this());
    boost::asio::post(strand_, [self, t = std::move(text)]() mutable {
      if (self->closed_) return;
      strip_trailing_crlf(t);
      t.push_back('\n');
      self->enqueue_write(std::vector<char>(t.begin(), t.end()));
    });
  }

  void write_binary(std::vector<char> payload) override {
    auto self = std::static_pointer_cast<SessionBase<Stream>>(ISession::shared_from_this());
    boost::asio::post(strand_, [self, p = std::move(payload)]() mutable {
      if (self->closed_ || p.size() > kMaxBinaryPayloadBytes) return;
      auto len32 = static_cast<std::uint32_t>(p.size());
      std::uint8_t be[4];
      encode_be32(len32, be);
      std::vector<char> packet(be, be + 4);
      packet.insert(packet.end(), p.begin(), p.end());
      self->enqueue_write(std::move(packet));
    });
  }

  void begin_expect_binary(std::uint32_t byte_count) override {
    auto self = std::static_pointer_cast<SessionBase<Stream>>(ISession::shared_from_this());
    boost::asio::post(strand_, [self, byte_count]() {
      if (self->closed_ || self->rx_mode_ != RxMode::Lines) return;
      if (byte_count > kMaxBinaryPayloadBytes) {
        self->fail("binary_expect_too_large");
        return;
      }
      self->rx_mode_ = RxMode::Binary;
      self->binary_remaining_ = byte_count;
      self->binary_accum_.clear();
      self->binary_accum_.reserve(byte_count);
      if (byte_count == 0) self->complete_binary_message();
      else self->consume_recv_into_binary();
    });
  }

  void request_close(const char* reason) override {
    auto self = std::static_pointer_cast<SessionBase<Stream>>(ISession::shared_from_this());
    boost::asio::post(strand_, [self, r = reason ? reason : "request_close"]() {
      if (!self->closed_) self->fail(r);
    });
  }

 protected:
  // 子类必须实现的虚函数
  virtual void do_read() = 0;
  virtual void do_write_front() = 0;

  // 共享的协议处理逻辑
  void process_received_bytes(const char* data, std::size_t len) {
    if (!append_bytes(recv_buffer_, data, len)) {
      fail("recv_buffer_overflow");
      return;
    }
    if (rx_mode_ == RxMode::Binary) consume_recv_into_binary();
    else process_line_frames();
  }

  void process_line_frames() {
    while (!closed_ && rx_mode_ == RxMode::Lines) {
      PopLineResult r = try_pop_line(recv_buffer_);
      if (r.status == PopLineResult::Status::None) break;
      if (r.status == PopLineResult::Status::TooLong) {
        fail("line_too_long");
        return;
      }
      if (!r.line.empty()) {
        handler_.on_line(*this, std::move(r.line));
        if (closed_) return;
      }
    }
  }

  void consume_recv_into_binary() {
    while (!closed_ && binary_remaining_ > 0 && !recv_buffer_.empty()) {
      std::size_t take = std::min(recv_buffer_.size(),
                                  static_cast<std::size_t>(binary_remaining_));
      binary_accum_.insert(binary_accum_.end(),
                           recv_buffer_.begin(),
                           recv_buffer_.begin() + take);
      recv_buffer_.erase(0, take);
      binary_remaining_ -= static_cast<std::uint32_t>(take);
    }
    if (!closed_ && binary_remaining_ == 0) complete_binary_message();
  }

  void complete_binary_message() {
    rx_mode_ = RxMode::Lines;
    binary_remaining_ = 0;
    std::vector<char> payload = std::move(binary_accum_);
    binary_accum_.clear();
    handler_.on_binary(*this, std::move(payload));
    if (!closed_ && !recv_buffer_.empty()) process_line_frames();
  }

  void enqueue_write(std::vector<char> chunk) {
    if (closed_ || chunk.empty()) return;
    bool idle = !write_in_progress_;
    write_queue_.push_back(std::move(chunk));
    if (idle) {
      write_in_progress_ = true;
      do_write_front();
    }
  }

  void fail(const char* reason) {
    if (closed_) return;
    closed_ = true;
    reset_writes();
    reset_rx_for_close();
    boost::system::error_code ec;
    stream_.lowest_layer().close(ec);
    handler_.on_error(*this, reason);
    handler_.on_close(*this);
  }

  void clean_close() {
    if (closed_) return;
    closed_ = true;
    reset_writes();
    reset_rx_for_close();
    boost::system::error_code ec;
    stream_.lowest_layer().close(ec);
    handler_.on_close(*this);
  }

  void reset_writes() {
    write_queue_.clear();
    write_in_progress_ = false;
  }

  void reset_rx_for_close() {
    rx_mode_ = RxMode::Lines;
    binary_remaining_ = 0;
    binary_accum_.clear();
    recv_buffer_.clear();
  }

  static void strip_trailing_crlf(std::string& s) {
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) {
      s.pop_back();
    }
  }

  // 成员变量
  Stream stream_;
  boost::asio::strand<typename Stream::lowest_layer_type::executor_type> strand_;
  ISessionHandler& handler_;

  std::string recv_buffer_;
  std::array<char, 8192> read_buf_{};
  std::deque<std::vector<char>> write_queue_;
  bool write_in_progress_ = false;

  enum class RxMode { Lines, Binary };
  RxMode rx_mode_ = RxMode::Lines;
  std::uint32_t binary_remaining_ = 0;
  std::vector<char> binary_accum_;
  bool closed_ = false;
};

}  // namespace net