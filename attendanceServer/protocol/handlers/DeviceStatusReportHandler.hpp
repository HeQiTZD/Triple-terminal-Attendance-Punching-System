#pragma once

#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "service/DeviceService.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void handle_device_status_report(
    net::ISession& session,
    const ParsedEnvelope& env,
    ConnContext& ctx,
    const std::string& route_to,
    service::DbExecutor& db) {
  const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
  auto self = session.shared_from_this();
  service::device_handle_status_report(env, ctx, to, db, self);
}

}  // namespace protocol::handlers