#pragma once

#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/MessageHandlerRegistry.hpp"
#include "protocol/ProtocolUtils.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "service/MessageRouter.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void register_device_command_handlers(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::MessageRouter* router) {
  registry.register_handler(std::string(kTypeDeviceCommandAck),
      [router](net::ISession& session, const ParsedEnvelope& env,
               ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role == EndpointRole::Device && router) {
          router->handle_device_command_ack(env);
        }
      });

  registry.register_handler(std::string(kTypeDeviceCommand),
      [router](net::ISession& session, const ParsedEnvelope& env,
               ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin) {
          session.write_line(build_error(kCodeForbidden, "admin role required",
                                         env.msg_id, route_to));
          return;
        }
        if (!has_permission(ctx, "device.command")) {
          session.write_line(build_error(kCodeForbidden, "missing permission",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string op_id = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        if (router) {
          router->forward_device_command(self, env, op_id);
        }
      });
}

}  // namespace protocol::handlers