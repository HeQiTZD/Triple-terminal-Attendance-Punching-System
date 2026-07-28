#include "util/JwtUtil.hpp"

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>

#include <nlohmann/json.hpp>

#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <stdexcept>
#include <algorithm>

namespace util {

JwtUtil::JwtUtil(const JwtConfig& config) : config_(config) {
    if (config_.secret.size() < 32) {
        throw std::runtime_error("JWT secret must be at least 32 bytes");
    }
}

std::string JwtUtil::generate_access_token(const TokenPayload& payload) {
    TokenPayload access_payload = payload;
    access_payload.type = "access";
    access_payload.iat = current_timestamp();
    access_payload.exp = access_payload.iat + config_.access_token_ttl;
    if (access_payload.jti.empty()) {
        access_payload.jti = generate_jti();
    }
    return generate_token(access_payload);
}

std::string JwtUtil::generate_refresh_token(const std::string& sub, const std::string& role) {
    TokenPayload refresh_payload;
    refresh_payload.sub = sub;
    refresh_payload.role = role;
    refresh_payload.type = "refresh";
    refresh_payload.iat = current_timestamp();
    refresh_payload.exp = refresh_payload.iat + config_.refresh_token_ttl;
    refresh_payload.jti = generate_jti();
    return generate_token(refresh_payload);
}

std::optional<TokenPayload> JwtUtil::verify_access_token(const std::string& token) {
    return verify_token(token, "access");
}

std::optional<TokenPayload> JwtUtil::verify_refresh_token(const std::string& token) {
    return verify_token(token, "refresh");
}

std::string JwtUtil::generate_token(const TokenPayload& payload) {
    // 构建 Header
    nlohmann::json header;
    header["alg"] = "HS256";
    header["typ"] = "JWT";
    std::string header_str = header.dump();
    std::string encoded_header = base64_url_encode(header_str);

    // 构建 Payload
    nlohmann::json payload_json;
    payload_json["sub"] = payload.sub;
    payload_json["role"] = payload.role;
    payload_json["iat"] = payload.iat;
    payload_json["exp"] = payload.exp;
    payload_json["jti"] = payload.jti;
    payload_json["type"] = payload.type;

    if (!payload.permissions.empty()) {
        payload_json["permissions"] = payload.permissions;
    }
    if (payload.user_id > 0) {
        payload_json["user_id"] = payload.user_id;
    }
    if (!payload.employee_id.empty()) {
        payload_json["employee_id"] = payload.employee_id;
    }

    std::string payload_str = payload_json.dump();
    std::string encoded_payload = base64_url_encode(payload_str);

    // 生成签名
    std::string data = encoded_header + "." + encoded_payload;
    std::string signature = hmac_sha256(data, config_.secret);
    std::string encoded_signature = base64_url_encode(signature);

    return data + "." + encoded_signature;
}

std::optional<TokenPayload> JwtUtil::verify_token(const std::string& token, const std::string& expected_type) {
    // 分割令牌
    size_t first_dot = token.find('.');
    if (first_dot == std::string::npos) {
        return std::nullopt;
    }
    size_t second_dot = token.find('.', first_dot + 1);
    if (second_dot == std::string::npos) {
        return std::nullopt;
    }

    std::string encoded_header = token.substr(0, first_dot);
    std::string encoded_payload = token.substr(first_dot + 1, second_dot - first_dot - 1);
    std::string encoded_signature = token.substr(second_dot + 1);

    // 验证签名
    std::string data = encoded_header + "." + encoded_payload;
    std::string expected_signature = hmac_sha256(data, config_.secret);
    std::string actual_signature = base64_url_decode(encoded_signature);

    if (expected_signature != actual_signature) {
        return std::nullopt;
    }

    // 解析 Payload
    try {
        std::string payload_str = base64_url_decode(encoded_payload);
        nlohmann::json payload_json = nlohmann::json::parse(payload_str);

        TokenPayload payload;
        payload.sub = payload_json.value("sub", "");
        payload.role = payload_json.value("role", "");
        payload.iat = payload_json.value("iat", 0);
        payload.exp = payload_json.value("exp", 0);
        payload.jti = payload_json.value("jti", "");
        payload.type = payload_json.value("type", "");
        payload.user_id = payload_json.value("user_id", 0);
        payload.employee_id = payload_json.value("employee_id", "");

        if (payload_json.contains("permissions") && payload_json["permissions"].is_array()) {
            for (const auto& perm : payload_json["permissions"]) {
                if (perm.is_string()) {
                    payload.permissions.push_back(perm.get<std::string>());
                }
            }
        }

        // 验证令牌类型
        if (payload.type != expected_type) {
            return std::nullopt;
        }

        // 验证过期时间（考虑时钟偏差）
        int64_t now = current_timestamp();
        if (now > payload.exp + config_.clock_skew) {
            return std::nullopt;
        }

        // 验证签发时间（不能是未来的时间）
        if (payload.iat > now + config_.clock_skew) {
            return std::nullopt;
        }

        return payload;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::string JwtUtil::generate_jti() {
    // 生成 16 字节随机数作为 JTI
    auto bytes = generate_random_bytes(16);
    return base64_url_encode(bytes);
}

int64_t JwtUtil::current_timestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

std::string JwtUtil::base64_url_encode(const std::string& input) {
    BIO* bio = nullptr;
    BIO* b64 = nullptr;
    BUF_MEM* buffer_ptr = nullptr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // 设置 Base64 URL 安全字符
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), static_cast<int>(input.size()));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &buffer_ptr);

    std::string result(buffer_ptr->data, buffer_ptr->length);
    BIO_free_all(bio);

    // 转换为 URL 安全的 Base64
    std::replace(result.begin(), result.end(), '+', '-');
    std::replace(result.begin(), result.end(), '/', '_');

    // 移除填充字符
    size_t pad_pos = result.find('=');
    if (pad_pos != std::string::npos) {
        result = result.substr(0, pad_pos);
    }

    return result;
}

std::string JwtUtil::base64_url_decode(const std::string& input) {
    // 还原标准 Base64 字符
    std::string base64 = input;
    std::replace(base64.begin(), base64.end(), '-', '+');
    std::replace(base64.begin(), base64.end(), '_', '/');

    // 添加填充字符
    switch (base64.size() % 4) {
        case 2: base64 += "=="; break;
        case 3: base64 += "="; break;
        default: break;
    }

    BIO* bio = nullptr;
    BIO* b64 = nullptr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(base64.c_str(), static_cast<int>(base64.size()));
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    std::vector<char> buffer(base64.size());
    int length = BIO_read(bio, buffer.data(), static_cast<int>(buffer.size()));
    BIO_free_all(bio);

    if (length <= 0) {
        return "";
    }

    return std::string(buffer.data(), length);
}

std::string JwtUtil::hmac_sha256(const std::string& data, const std::string& key) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;

    HMAC(EVP_sha256(),
         key.c_str(), static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char*>(data.c_str()), data.size(),
         digest, &digest_len);

    return std::string(reinterpret_cast<char*>(digest), digest_len);
}

std::string JwtUtil::generate_random_bytes(size_t length) {
    std::vector<unsigned char> buffer(length);
    RAND_bytes(buffer.data(), static_cast<int>(length));
    return std::string(reinterpret_cast<char*>(buffer.data()), length);
}

} // namespace util
