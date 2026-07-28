#pragma once

#include <string>

namespace util {

// Produces "hex_salt$hex_hash" using SHA-256.
// salt_bytes: length of the random salt (default 16).
std::string hash_password(const std::string& plain, int salt_bytes = 16);

// Returns true if `plain` matches the stored hash produced by hash_password().
bool verify_password(const std::string& plain, const std::string& stored);

// Validates password strength: >= 8 chars, at least one letter and one digit.
// Returns empty string if valid, otherwise a human-readable error message.
std::string validate_password_strength(const std::string& plain);

}  // namespace util
