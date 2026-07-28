#pragma once

#include <memory>
#include <string>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"

namespace net {
class ISession;
}

namespace service {

class DbExecutor;

void device_handle_status_report(const protocol::ParsedEnvelope& env,
                                 protocol::ConnContext& ctx,
                                 const std::string& reply_to,
                                 DbExecutor& db,
                                 const std::shared_ptr<net::ISession>& session);

}  // namespace service
