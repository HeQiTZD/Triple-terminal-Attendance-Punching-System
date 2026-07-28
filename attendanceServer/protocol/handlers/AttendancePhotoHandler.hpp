#pragma once

#include <cstdint>
#include <string>

#include "net/Session.hpp"
#include "protocol/ConnContext.hpp"
#include "protocol/Envelope.hpp"
#include "protocol/MessageHandlerRegistry.hpp"
#include "protocol/ProtocolUtils.hpp"
#include "protocol/ResponseBuilders.hpp"
#include "util/DebugLog.hpp"

namespace net {
class ISession;
}

namespace protocol::handlers {

inline void register_attendance_photo_handler(
    MessageHandlerRegistryT<net::ISession, ParsedEnvelope, ConnContext>& registry) {
  registry.register_handler(std::string(kTypeAttendancePhotoHeader),
      [](net::ISession& session, const ParsedEnvelope& env,
         ConnContext& ctx, const std::string& route_to) {
        std::optional<std::uint64_t> plen = read_payload_length(env.data);
        if (!plen || *plen > net::kMaxBinaryPayloadBytes) {
          WARN_NET("照片头部验证失败: payloadLength无效或过大"
                   << "\n  - payloadLength: " << (plen ? std::to_string(*plen) : "null")
                   << "\n  - 最大允许: " << net::kMaxBinaryPayloadBytes << " bytes"
                   << "\n  - msgId: " << env.msg_id);
          session.write_line(build_error(kCodePayloadTooLarge,
                                         "invalid payloadLength", env.msg_id,
                                         route_to));
          return;
        }
        if (!ctx.pending_attendance) {
          WARN_NET("照片接收失败: 无待处理考勤记录"
                   << "\n  - 当前状态: pending_attendance=null"
                   << "\n  - 请求类型: attendance.photo.header"
                   << "\n  - msgId: " << env.msg_id);
          session.write_line(build_error(
              kCodeBusinessValidation, "no pending attendance for photo",
              env.msg_id, route_to));
          return;
        }
        INFO_NET("照片头部已接收: payloadLength=" << *plen << " bytes");
        ctx.pending_binary = PendingBinaryKind::AttendancePhoto;
        ctx.expected_binary_length = static_cast<std::uint32_t>(*plen);
        const std::uint32_t n32 = static_cast<std::uint32_t>(*plen);
        session.begin_expect_binary(n32);
      });
}

}  // namespace protocol::handlers