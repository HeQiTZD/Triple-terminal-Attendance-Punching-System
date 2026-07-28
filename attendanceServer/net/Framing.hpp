#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace net {

// 与通信协议.md / cpp-server-dev.md 对齐的可调上限。
inline constexpr std::size_t kMaxJsonLineBytes = 1024 * 1024;
inline constexpr std::size_t kMaxBinaryPayloadBytes = 16 * 1024 * 1024;
// 尚未收到换行符时 recv_buffer 总长度上限（须 ≥ 单行上限，避免恶意半包撑爆内存）。
inline constexpr std::size_t kMaxRecvBufferBytes = kMaxJsonLineBytes + 4096;

// 大端 32 位长度前缀（二进制帧）；始终返回 true，签名预留将来错误语义。
bool encode_be32(std::uint32_t value, std::uint8_t out[4]);
bool decode_be32(const std::uint8_t in[4], std::uint32_t& value);

// 将一段接收字节追加到缓冲区；超过 kMaxRecvBufferBytes 则不修改 buffer 并返回 false。
bool append_bytes(std::string& recv_buffer, const void* data, std::size_t len);

// 从 buffer 中尝试取出第一条完整文本行（以 LF 结尾）。
// - None：尚无 LF，buffer 不变。
// - Ok：已取出一行（不含行尾的 LF）；若为 CRLF，会去掉行尾单个 CR。
// - TooLong：第一个 LF 前行字节数超过 kMaxJsonLineBytes，已从 buffer 丢弃该行（含 LF），line 为空。
struct PopLineResult {
  enum class Status { None, Ok, TooLong };
  Status status = Status::None;
  std::string line;
};

PopLineResult try_pop_line(std::string& buffer);

}  // namespace net
