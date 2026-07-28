#pragma once

#include <functional>
#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/MessageHandlerRegistry.hpp"
#include "protocol/ProtocolUtils.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "service/AdminCrudService.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

// Person CRUD 处理器
inline void register_person_handlers(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::DbExecutor& db) {
  registry.register_handler(std::string(kTypePersonCreate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "person.create")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_person_create(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypePersonQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "person.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_person_query(env, to, db, self);
      });

  registry.register_handler(std::string(kTypePersonUpdate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "person.update")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_person_update(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypePersonDelete),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "person.delete")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_person_delete(env, to, db, self, audit);
      });
}

// Attendance CRUD 处理器
inline void register_attendance_handlers(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::DbExecutor& db) {
  registry.register_handler(std::string(kTypeAttendanceCreate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "attendance.create")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_attendance_create(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeAttendanceQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "attendance.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_attendance_query(env, to, db, self);
      });

  registry.register_handler(std::string(kTypeAttendanceUpdate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "attendance.update")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_attendance_update(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeAttendanceDelete),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "attendance.delete")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_attendance_delete(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeAttendanceArchiveQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "attendance.archive.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_attendance_archive_query(env, to, db, self);
      });

  registry.register_handler(std::string(kTypeAttendanceArchiveDelete),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "attendance.archive.delete")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_attendance_archive_delete(env, to, db, self, audit);
      });
}

// Device CRUD 处理器
inline void register_device_handlers(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::DbExecutor& db) {
  registry.register_handler(std::string(kTypeDeviceCreate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "device.create")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_device_create(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeDeviceQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "device.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_device_query(env, to, db, self);
      });

  registry.register_handler(std::string(kTypeDeviceUpdate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "device.update")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_device_update(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeDeviceDelete),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "device.delete")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_device_delete(env, to, db, self, audit);
      });
}

// User CRUD 处理器
inline void register_user_handlers(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry,
    service::DbExecutor& db) {
  registry.register_handler(std::string(kTypeUserCreate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "user.create")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_user_create(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeUserQuery),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "user.read")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::admin_user_query(env, to, db, self);
      });

  registry.register_handler(std::string(kTypeUserUpdate),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "user.update")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_user_update(env, to, db, self, audit);
      });

  registry.register_handler(std::string(kTypeUserDelete),
      [&db](net::ISession& session, const ParsedEnvelope& env,
            ConnContext& ctx, const std::string& route_to) {
        if (ctx.endpoint_role != EndpointRole::Admin ||
            !has_permission(ctx, "user.delete")) {
          session.write_line(build_error(kCodeForbidden, "forbidden",
                                         env.msg_id, route_to));
          return;
        }
        auto self = session.shared_from_this();
        const std::string to = ctx.bound_from.empty() ? route_to : ctx.bound_from;
        service::AuditContext audit{ctx.user_id, ctx.employee_id,
                                    remote_ip_string(session)};
        service::admin_user_delete(env, to, db, self, audit);
      });
}

}  // namespace protocol::handlers