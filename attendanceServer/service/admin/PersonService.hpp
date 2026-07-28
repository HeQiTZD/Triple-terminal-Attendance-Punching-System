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

// Person CRUD
void admin_person_create(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit);

void admin_person_query(const protocol::ParsedEnvelope& env,
                        const std::string& reply_to, IDbExecutor& db,
                        const std::shared_ptr<net::ISession>& session);

void admin_person_update(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit);

void admin_person_delete(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit);

}  // namespace service
