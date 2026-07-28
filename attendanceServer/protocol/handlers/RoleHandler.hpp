#pragma once

#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/MessageHandlerRegistry.hpp"
#include "protocol/ProtocolUtils.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "service/AdminCrudService.hpp"
#include "service/admin/RoleService.hpp"
#include "service/admin/UserService.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void register_role_handlers(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::IDbExecutor& db,
    service::PermissionReloadCallback on_role_changed = nullptr) {
  // Role CRUD
  registry.register_handler(std::string(kTypeRoleCreate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "role.create")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_role_create(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeRoleQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "role.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_role_query(env, to, db, self);
      });

  registry.register_handler(std::string(kTypeRoleUpdate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "role.update")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_role_update(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeRoleDelete),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "role.delete")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_role_delete(env, to, db, self, audit);
      });

  // UserRole 操作
  registry.register_handler(std::string(kTypeUserRoleAssign),
      [&db, on_role_changed](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "user.role.assign")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_user_role_assign(env, to, db, self, audit, on_role_changed);
      });

  registry.register_handler(std::string(kTypeUserRoleRevoke),
      [&db, on_role_changed](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "user.role.revoke")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_user_role_revoke(env, to, db, self, ctx.user_id, audit, on_role_changed);
      });

  registry.register_handler(std::string(kTypeUserRoleQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "user.role.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_user_role_query(env, to, db, self);
      });

  // Permission 查询
  registry.register_handler(std::string(kTypePermissionQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "permission.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_permission_query(env, to, db, self);
      });

  registry.register_handler(std::string(kTypePermissionSelf),
      [](net::ISession& session, const ParsedEnvelope& env,
         ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        auto perms_copy = ctx.permissions;
        service::admin_permission_self(env, to, perms_copy, self);
      });
}

}  // namespace protocol::handlers