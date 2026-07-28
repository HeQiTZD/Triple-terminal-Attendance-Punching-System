#pragma once

#include <memory>
#include <string>

#include "protocol/Envelope.hpp"
#include "service/AdminCrudService.hpp"

namespace net {
class ISession;
}

namespace service {

class DbExecutor;
class MessageRouter;

void admin_config_deploy(const protocol::ParsedEnvelope& env,
                         const std::string& reply_to, DbExecutor& db,
                         const std::shared_ptr<net::ISession>& session,
                         const AuditContext& audit, MessageRouter& router);

void push_pending_config_for_device(const std::string& device_id,
                                    DbExecutor& db, MessageRouter& router);

}  // namespace service
