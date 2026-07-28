#pragma once

#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "service/SyncService.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void handle_sync_request(
    net::ISession& session,
    const ParsedEnvelope& env,
    ConnContext& ctx,
    const std::string& route_to,
    service::DbExecutor& db) {
  const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
  auto self = session.shared_from_this();
  service::sync_handle_request(env, ctx, to, db, self);
}

inline void handle_sync_ack(
    net::ISession& session,
    const ParsedEnvelope& env,
    ConnContext& ctx,
    const std::string& route_to) {
  const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
  auto self = session.shared_from_this();
  service::sync_handle_ack(env, ctx, to, self);
}

}  // namespace protocol::handlers