#pragma once

#include <memory>
#include <string>

#include "protocol/Envelope.hpp"

namespace net {
class ISession;
}

namespace service {

class IDbExecutor;
struct AuditContext;
using PermissionReloadCallback = std::function<void(int user_id)>;

// UserAccount CRUD
void admin_user_create(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit);

void admin_user_query(const protocol::ParsedEnvelope& env,
                      const std::string& reply_to, IDbExecutor& db,
                      const std::shared_ptr<net::ISession>& session);

void admin_user_update(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit);

void admin_user_delete(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit);

// User-Role management
void admin_user_role_assign(const protocol::ParsedEnvelope& env,
                            const std::string& reply_to, IDbExecutor& db,
                            const std::shared_ptr<net::ISession>& session,
                            const AuditContext& audit,
                            PermissionReloadCallback on_role_changed);

void admin_user_role_revoke(const protocol::ParsedEnvelope& env,
                            const std::string& reply_to, IDbExecutor& db,
                            const std::shared_ptr<net::ISession>& session,
                            int caller_user_id,
                            const AuditContext& audit,
                            PermissionReloadCallback on_role_changed);

void admin_user_role_query(const protocol::ParsedEnvelope& env,
                           const std::string& reply_to, IDbExecutor& db,
                           const std::shared_ptr<net::ISession>& session);

}  // namespace service
