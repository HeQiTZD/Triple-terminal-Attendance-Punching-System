#pragma once

#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "service/AttendanceService.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void handle_attendance_report(
    net::ISession& session,
    const ParsedEnvelope& env,
    ConnContext& ctx,
    const std::string& route_to,
    service::DbExecutor& db,
    service::EventSubscriptionRegistry* subs) {
  const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
  auto self = session.shared_from_this();
  service::attendance_handle_report(env, ctx, to, db, self, subs);
}

}  // namespace protocol::handlers