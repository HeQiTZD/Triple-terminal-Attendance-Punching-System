#pragma once

#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/MessageHandlerRegistry.hpp"
#include "protocol/ProtocolUtils.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "service/DeviceConfigService.hpp"
#include "service/MessageRouter.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void register_config_deploy_handler(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::DbExecutor& db,
    service::MessageRouter* router) {
  registry.register_handler(std::string(kTypeConfigDeploy),
      [&db, router](net::ISession& session, const ParsedEnvelope& env,
                     ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin) {
          session.write_line(build_error(kCodeForbidden, "admin role required",
                                         env.msg_id, route_to));
          return;
        }
        if (!has_permission(ctx, "config.deploy")) {
          session.write_line(build_error(kCodeForbidden, "missing permission",
                                         env.msg_id, route_to));
          return;
        }
        if (!router) {
          session.write_line(build_error(kCodeBusinessValidation,
                                         "message router unavailable",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_config_deploy(env, to, db, self, audit, *router);
      });
}

}  // namespace protocol::handlers