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

// FaceData query/delete/register
void admin_face_query(const protocol::ParsedEnvelope& env,
                      const std::string& reply_to, IDbExecutor& db,
                      const std::shared_ptr<net::ISession>& session);

void admin_face_delete(const protocol::ParsedEnvelope& env,
                       const std::string& reply_to, IDbExecutor& db,
                       const std::shared_ptr<net::ISession>& session,
                       const AuditContext& audit);

void admin_face_register(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to,
                         IDbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit);

}  // namespace service
