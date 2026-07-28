#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <nlohmann/json_fwd.hpp>

#include "protocol/Envelope.hpp"
#include "service/IDeviceRegistry.hpp"  // 修改：依赖接口

namespace net {
class ISession;
}

namespace service {

// 管理端 device.command → 设备 → ack 回管理端（通信协议 §6）；默认超时 10s。
class MessageRouter {
 public:
  using AckHandler =
      std::function<void(const std::string&, const protocol::ParsedEnvelope&)>;
  using TimeoutHandler = std::function<void(const std::string&)>;

  struct ForwardResult {
    bool delivered = false;
    std::string forward_id;
  };

  explicit MessageRouter(boost::asio::io_context& ioc,
                         IDeviceRegistry& devices);  // 修改：依赖接口
  ~MessageRouter();

  MessageRouter(const MessageRouter&) = delete;
  MessageRouter& operator=(const MessageRouter&) = delete;

  void cancel_pending_for_session(const net::ISession* raw);

  void forward_device_command(const std::shared_ptr<net::ISession>& admin_sess,
                              const protocol::ParsedEnvelope& env,
                              const std::string& operator_id);

  ForwardResult forward_server_device_command(const std::string& device_id,
                                              const nlohmann::json& data,
                                              const std::string& msg_id_seed,
                                              AckHandler on_ack,
                                              TimeoutHandler on_timeout);

  void handle_device_command_ack(const protocol::ParsedEnvelope& env);

 private:
  struct Pending {
    std::unique_ptr<boost::asio::steady_timer> timer;
    std::weak_ptr<net::ISession> admin;
    bool has_admin = false;
    std::string admin_msg_id;
    std::string operator_id;
    AckHandler on_ack;
    TimeoutHandler on_timeout;
  };

  std::string make_forward_id(const std::string& admin_msg_id);

  boost::asio::io_context& ioc_;
  IDeviceRegistry& devices_;  // 修改：依赖接口
  std::mutex mu_;
  std::unordered_map<std::string, std::unique_ptr<Pending>> pending_;
  std::atomic<std::uint64_t> seq_{0};
};

}  // namespace service
