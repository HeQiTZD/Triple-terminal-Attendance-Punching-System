#pragma once

#include <memory>
#include <string>
#include <vector>

#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"

namespace net {
class ISession;
}

namespace service {

class DbExecutor;
class EventSubscriptionRegistry;

void attendance_handle_report(const protocol::ParsedEnvelope& env,
                              protocol::ConnContext& ctx,
                              const std::string& reply_to,
                              DbExecutor& db,
                              const std::shared_ptr<net::ISession>& session,
                              EventSubscriptionRegistry* push_registry);

void attendance_complete_with_photo(protocol::PendingAttendance pending,
                                    const std::vector<char>& photo_bytes,
                                    DbExecutor& db,
                                    const std::shared_ptr<net::ISession>& session,
                                    EventSubscriptionRegistry* push_registry);

}  // namespace service
