#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <boost/asio/io_context.hpp>

#include "net/Session.hpp"
#include "protocol/ConnContext.hpp"
#include "protocol/GatewayConfig.hpp"
#include "protocol/MessageHandlerRegistry.hpp"
#include "service/AdminRegistry.hpp"
#include "service/DbExecutor.hpp"
#include "service/DeviceRegistry.hpp"
#include "service/EventSubscriptionRegistry.hpp"
#include "service/MessageRouter.hpp"
#include "service/TokenManager.hpp"

namespace protocol {

class GatewaySessionHandler : public net::ISessionHandler {
 public:
  GatewaySessionHandler(service::DeviceRegistry& registry,
                        service::DbExecutor& db_executor, GatewayConfig cfg,
                        service::AdminRegistry& admin_registry,
                        service::EventSubscriptionRegistry& subscriptions);

  void on_line(net::ISession& session, std::string line) override;
  void on_binary(net::ISession& session, std::vector<char> payload) override;
  void on_error(net::ISession& session, const char* reason) override;
  void on_close(net::ISession& session) override;
  void on_listen_started(boost::asio::io_context& ioc) override;

  // Reload RBAC permissions for an online admin (called after role changes).
  void reload_permissions(const std::string& operator_id);

 private:
  void erase_connection(net::ISession* s);
  void arm_heartbeat(net::ISession& session, ConnContext& ctx);
  void cancel_heartbeat(ConnContext& ctx) const;
  bool device_key_matches(const std::string& device_id,
                          const std::string& key) const;

  // JWT 认证相关
  void handleDeviceAuth(net::ISession& session, const ParsedEnvelope& env);
  void handleAdminAuth(net::ISession& session, const ParsedEnvelope& env);
  void handleTokenRefresh(net::ISession& session, const ParsedEnvelope& env);
  bool verifyTokenInEnvelope(const ParsedEnvelope& env, ConnContext& ctx);

  // 消息处理器注册
  void register_handlers();

  service::DeviceRegistry& registry_;
  service::DbExecutor& db_;
  service::AdminRegistry& admin_registry_;
  service::EventSubscriptionRegistry& subscriptions_;
  GatewayConfig cfg_;

  std::unique_ptr<service::MessageRouter> router_;
  std::unique_ptr<service::TokenManager> tokenManager_;

  // 消息处理器注册表
  using HandlerRegistry = MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>;
  HandlerRegistry handler_registry_;

  std::mutex mu_;
  std::unordered_map<net::ISession*, ConnContext> connections_;
};

}  // namespace protocol
