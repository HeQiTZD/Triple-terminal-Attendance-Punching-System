#include "net/Framing.hpp"

namespace net {

bool encode_be32(std::uint32_t value, std::uint8_t out[4]) {
  out[0] = static_cast<std::uint8_t>((value >> 24) & 0xff);
  out[1] = static_cast<std::uint8_t>((value >> 16) & 0xff);
  out[2] = static_cast<std::uint8_t>((value >> 8) & 0xff);
  out[3] = static_cast<std::uint8_t>(value & 0xff);
  return true;
}

bool decode_be32(const std::uint8_t in[4], std::uint32_t& value) {
  value = (static_cast<std::uint32_t>(in[0]) << 24) |
          (static_cast<std::uint32_t>(in[1]) << 16) |
          (static_cast<std::uint32_t>(in[2]) << 8) |
          static_cast<std::uint32_t>(in[3]);
  return true;
}

bool append_bytes(std::string& recv_buffer, const void* data, std::size_t len) {
  if (len == 0) {
    return true;
  }
  if (recv_buffer.size() > kMaxRecvBufferBytes - len) {
    return false;
  }
  recv_buffer.append(static_cast<const char*>(data), len);
  return true;
}

PopLineResult try_pop_line(std::string& buffer) {
  const std::size_t npos = buffer.find('\n');
  if (npos == std::string::npos) {
    return {PopLineResult::Status::None, {}};
  }

  if (npos > kMaxJsonLineBytes) {
    buffer.erase(0, npos + 1);
    return {PopLineResult::Status::TooLong, {}};
  }

  std::string line = buffer.substr(0, npos);
  buffer.erase(0, npos + 1);

  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }

  return {PopLineResult::Status::Ok, std::move(line)};
}

}  // namespace net
