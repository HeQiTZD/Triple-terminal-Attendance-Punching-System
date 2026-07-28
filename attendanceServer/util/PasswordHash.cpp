#include "util/PasswordHash.hpp"

#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace util {

namespace {

// Minimal self-contained SHA-256 (FIPS 180-4).
// No external dependency on OpenSSL.

static constexpr uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

inline uint32_t rotr(uint32_t x, int n) {
  return (x >> n) | (x << (32 - n));
}

inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) ^ (~x & z);
}

inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) ^ (x & z) ^ (y & z);
}

inline uint32_t ep0(uint32_t x) {
  return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

inline uint32_t ep1(uint32_t x) {
  return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

inline uint32_t sig0(uint32_t x) {
  return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

inline uint32_t sig1(uint32_t x) {
  return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

struct Sha256Ctx {
  uint32_t state[8];
  uint8_t buf[64];
  uint64_t total_len;
  int buf_len;
};

void sha256_init(Sha256Ctx& ctx) {
  ctx.state[0] = 0x6a09e667;
  ctx.state[1] = 0xbb67ae85;
  ctx.state[2] = 0x3c6ef372;
  ctx.state[3] = 0xa54ff53a;
  ctx.state[4] = 0x510e527f;
  ctx.state[5] = 0x9b05688c;
  ctx.state[6] = 0x1f83d9ab;
  ctx.state[7] = 0x5be0cd19;
  ctx.total_len = 0;
  ctx.buf_len = 0;
}

void sha256_transform(Sha256Ctx& ctx, const uint8_t block[64]) {
  uint32_t w[64];
  for (int i = 0; i < 16; ++i) {
    w[i] = (uint32_t(block[i * 4]) << 24) | (uint32_t(block[i * 4 + 1]) << 16) |
           (uint32_t(block[i * 4 + 2]) << 8) | uint32_t(block[i * 4 + 3]);
  }
  for (int i = 16; i < 64; ++i) {
    w[i] = sig1(w[i - 2]) + w[i - 7] + sig0(w[i - 15]) + w[i - 16];
  }

  uint32_t a = ctx.state[0], b = ctx.state[1], c = ctx.state[2],
           d = ctx.state[3], e = ctx.state[4], f = ctx.state[5],
           g = ctx.state[6], h = ctx.state[7];

  for (int i = 0; i < 64; ++i) {
    uint32_t t1 = h + ep1(e) + ch(e, f, g) + K[i] + w[i];
    uint32_t t2 = ep0(a) + maj(a, b, c);
    h = g; g = f; f = e; e = d + t1;
    d = c; c = b; b = a; a = t1 + t2;
  }

  ctx.state[0] += a; ctx.state[1] += b; ctx.state[2] += c; ctx.state[3] += d;
  ctx.state[4] += e; ctx.state[5] += f; ctx.state[6] += g; ctx.state[7] += h;
}

void sha256_update(Sha256Ctx& ctx, const void* data, size_t len) {
  auto p = static_cast<const uint8_t*>(data);
  ctx.total_len += len;
  while (len > 0) {
    int space = 64 - ctx.buf_len;
    int take = (len < static_cast<size_t>(space)) ? static_cast<int>(len) : space;
    std::memcpy(ctx.buf + ctx.buf_len, p, take);
    ctx.buf_len += take;
    p += take;
    len -= take;
    if (ctx.buf_len == 64) {
      sha256_transform(ctx, ctx.buf);
      ctx.buf_len = 0;
    }
  }
}

std::array<uint8_t, 32> sha256_final(Sha256Ctx& ctx) {
  uint64_t bits = ctx.total_len * 8;
  uint8_t pad = 0x80;
  sha256_update(ctx, &pad, 1);
  pad = 0;
  while (ctx.buf_len != 56) {
    sha256_update(ctx, &pad, 1);
  }
  uint8_t len_be[8];
  for (int i = 7; i >= 0; --i) {
    len_be[i] = static_cast<uint8_t>(bits & 0xff);
    bits >>= 8;
  }
  sha256_update(ctx, len_be, 8);

  std::array<uint8_t, 32> digest;
  for (int i = 0; i < 8; ++i) {
    digest[i * 4]     = static_cast<uint8_t>(ctx.state[i] >> 24);
    digest[i * 4 + 1] = static_cast<uint8_t>(ctx.state[i] >> 16);
    digest[i * 4 + 2] = static_cast<uint8_t>(ctx.state[i] >> 8);
    digest[i * 4 + 3] = static_cast<uint8_t>(ctx.state[i]);
  }
  return digest;
}

// ── Utility ──

std::string to_hex(const uint8_t* data, size_t len) {
  std::ostringstream os;
  os << std::hex << std::setfill('0');
  for (size_t i = 0; i < len; ++i) {
    os << std::setw(2) << static_cast<unsigned>(data[i]);
  }
  return os.str();
}

std::vector<uint8_t> from_hex(const std::string& hex) {
  if (hex.size() % 2 != 0) {
    throw std::invalid_argument("invalid hex string length");
  }
  std::vector<uint8_t> bytes(hex.size() / 2);
  for (size_t i = 0; i < bytes.size(); ++i) {
    unsigned int byte;
    std::istringstream iss(hex.substr(i * 2, 2));
    iss >> std::hex >> byte;
    if (iss.fail()) {
      throw std::invalid_argument("invalid hex character");
    }
    bytes[i] = static_cast<uint8_t>(byte);
  }
  return bytes;
}

std::string sha256_salted(const std::vector<uint8_t>& salt,
                          const std::string& plain) {
  Sha256Ctx ctx;
  sha256_init(ctx);
  sha256_update(ctx, salt.data(), salt.size());
  sha256_update(ctx, plain.data(), plain.size());
  auto digest = sha256_final(ctx);
  return to_hex(digest.data(), digest.size());
}

std::vector<uint8_t> random_bytes(int n) {
  std::vector<uint8_t> buf(n);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<unsigned> dist(0, 255);
  for (int i = 0; i < n; ++i) {
    buf[i] = static_cast<uint8_t>(dist(gen));
  }
  return buf;
}

}  // namespace

std::string hash_password(const std::string& plain, int salt_bytes) {
  auto salt = random_bytes(salt_bytes);
  std::string salt_hex = to_hex(salt.data(), salt.size());
  std::string hash_hex = sha256_salted(salt, plain);
  return salt_hex + "$" + hash_hex;
}

bool verify_password(const std::string& plain, const std::string& stored) {
  auto pos = stored.find('$');
  if (pos == std::string::npos) {
    return false;
  }
  std::string salt_hex = stored.substr(0, pos);
  std::string expected_hash = stored.substr(pos + 1);

  auto salt = from_hex(salt_hex);
  std::string actual_hash = sha256_salted(salt, plain);
  return actual_hash == expected_hash;
}

std::string validate_password_strength(const std::string& plain) {
  if (plain.size() < 8) {
    return "password must be at least 8 characters";
  }
  bool has_letter = false, has_digit = false;
  for (char c : plain) {
    if (std::isalpha(static_cast<unsigned char>(c))) has_letter = true;
    if (std::isdigit(static_cast<unsigned char>(c))) has_digit = true;
  }
  if (!has_letter || !has_digit) {
    return "password must contain at least one letter and one digit";
  }
  return {};
}

}  // namespace util
