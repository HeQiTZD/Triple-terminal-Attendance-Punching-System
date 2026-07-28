#pragma once

#include <memory>
#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "service/DbExecutor.hpp"

namespace net {
class ISession;
}

namespace service {

void sync_handle_request(const protocol::ParsedEnvelope& env,
                         protocol::ConnContext& ctx, const std::string& reply_to,
                         DbExecutor& db,
                         const std::shared_ptr<net::ISession>& session);

void sync_handle_ack(const protocol::ParsedEnvelope& env,
                     protocol::ConnContext& ctx, const std::string& reply_to,
                     const std::shared_ptr<net::ISession>& session);

}  // namespace service
