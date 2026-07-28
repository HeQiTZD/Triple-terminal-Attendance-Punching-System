#pragma once

#include <string>
#include <vector>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/MessageHandlerRegistry.hpp"
#include "protocol/ProtocolUtils.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "service/EventSubscriptionRegistry.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void register_subscribe_handlers(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::EventSubscriptionRegistry& subscriptions) {
  registry.register_handler(std::string(kTypeSubscribe),
      [&subscriptions](net::ISession& session, const ParsedEnvelope& env,
                        ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "event.subscribe")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        std::vector<std::string> topics;
        if (env.data.contains("topics") && env.data["topics"].is_array()) {
          for (const auto& el : env.data["topics"]) {
            if (topics.size() >= 32) break;
            if (el.is_string()) {
              topics.push_back(el.get<std::string>());
            }
          }
        }
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        subscriptions.subscribe(&session, topics);
        session.write_line(build_report_ack(
            kTypeSubscribeResponse, env.msg_id, to, 0, "ok", nullptr));
      });

  registry.register_handler(std::string(kTypeUnsubscribe),
      [&subscriptions](net::ISession& session, const ParsedEnvelope& env,
                        ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "event.subscribe")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        subscriptions.unsubscribe_all(&session);
        session.write_line(build_report_ack(
            kTypeUnsubscribeResponse, env.msg_id, to, 0, "ok", nullptr));
      });
}

}  // namespace protocol::handlers