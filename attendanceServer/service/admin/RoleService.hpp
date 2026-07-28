#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "protocol/Envelope.hpp"

namespace net {
class ISession;
}

namespace service {

class IDbExecutor;
struct AuditContext;

// Role CRUD
void admin_role_create(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit);

void admin_role_query(const protocol::ParsedEnvelope& env,
                      const std::string& reply_to, IDbExecutor& db,
                      const std::shared_ptr<net::ISession>& session);

void admin_role_update(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit);

void admin_role_delete(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit);

// Permission queries
void admin_permission_query(const protocol::ParsedEnvelope& env,
                            const std::string& reply_to, IDbExecutor& db,
                            const std::shared_ptr<net::ISession>& session);

void admin_permission_self(
    const protocol::ParsedEnvelope& env, const std::string& reply_to,
    const std::unordered_set<std::string>& caller_permissions,
    const std::shared_ptr<net::ISession>& session);

}  // namespace service
