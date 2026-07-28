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

// Device CRUD
void admin_device_create(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit);

void admin_device_query(const protocol::ParsedEnvelope& env,
                        const std::string& reply_to, IDbExecutor& db,
                        const std::shared_ptr<net::ISession>& session);

void admin_device_update(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit);

void admin_device_delete(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit);

}  // namespace service
