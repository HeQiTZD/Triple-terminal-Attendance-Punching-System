#pragma once

#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/MessageHandlerRegistry.hpp"
#include "protocol/ProtocolUtils.hpp"
#include "protocol/ResponseBuilders.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void register_face_handlers(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::DbExecutor& db) {
  registry.register_handler(std::string(kTypeFaceQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "face.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_face_query(env, to, db, self);
      });

  registry.register_handler(std::string(kTypeFaceDelete),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "face.delete")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_face_delete(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeFaceRegister),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "face.register")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_face_register(env, to, db, self, audit);
      });
}

}  // namespace protocol::handlers