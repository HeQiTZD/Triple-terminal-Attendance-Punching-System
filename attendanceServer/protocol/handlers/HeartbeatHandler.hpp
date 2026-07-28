#pragma once

#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/ResponseBuilders.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void handle_heartbeat(
    net::ISession& session,
    const ParsedEnvelope& env,
    ConnContext& ctx,
    const std::string& route_to) {
  const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
  session.write_line(build_heartbeat_response(env.msg_id, to));
}

}  // namespace protocol::handlers