#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <ctime>

namespace util {

// JWT 配置
struct JwtConfig {
    std::string secret;               // 签名密钥（至少32字节）
    int access_token_ttl = 3600;      // 访问令牌有效期（秒）
    int refresh_token_ttl = 604800;   // 刷新令牌有效期（秒）
    int clock_skew = 30;              // 时钟偏差容忍度（秒）
};

// JWT 令牌载荷
struct TokenPayload {
    std::string sub;                  // 主体（设备ID或用户ID）
    std::string role;                 // 角色（device/admin）
    int64_t iat = 0;                  // 签发时间（Unix时间戳）
    int64_t exp = 0;                  // 过期时间（Unix时间戳）
    std::string jti;                  // 唯一标识（防重放）
    std::string type;                 // 令牌类型（access/refresh）
    std::vector<std::string> permissions; // 权限列表
    int user_id = 0;                  // 用户ID（仅管理端）
    std::string employee_id;          // 员工ID（仅管理端）
};

class JwtUtil {
public:
    explicit JwtUtil(const JwtConfig& config);
    ~JwtUtil() = default;

    // 生成访问令牌
    std::string generate_access_token(const TokenPayload& payload);

    // 生成刷新令牌
    std::string generate_refresh_token(const std::string& sub, const std::string& role);

    // 验证访问令牌
    std::optional<TokenPayload> verify_access_token(const std::string& token);

    // 验证刷新令牌
    std::optional<TokenPayload> verify_refresh_token(const std::string& token);

    // 生成唯一 JTI
    static std::string generate_jti();

    // 获取当前时间戳
    static int64_t current_timestamp();

private:
    JwtConfig config_;

    // 生成 JWT 令牌
    std::string generate_token(const TokenPayload& payload);

    // 验证 JWT 令牌
    std::optional<TokenPayload> verify_token(const std::string& token, const std::string& expected_type);

    // Base64 URL 编码
    static std::string base64_url_encode(const std::string& input);

    // Base64 URL 解码
    static std::string base64_url_decode(const std::string& input);

    // HMAC-SHA256 签名
    std::string hmac_sha256(const std::string& data, const std::string& key);

    // 生成随机字节
    static std::string generate_random_bytes(size_t length);
};

} // namespace util
