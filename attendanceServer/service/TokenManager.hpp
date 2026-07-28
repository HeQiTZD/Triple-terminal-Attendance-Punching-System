#pragma once

#include <string>
#include <optional>
#include <memory>
#include <mutex>
#include <unordered_set>

#include "util/JwtUtil.hpp"

namespace service {

class DbExecutor;

// 令牌对
struct TokenPair {
    std::string access_token;
    std::string refresh_token;
    int expires_in;
};

// 设备认证结果
struct DeviceAuthResult {
    bool success = false;
    std::string error_message;
    TokenPair tokens;
};

// 管理员认证结果
struct AdminAuthResult {
    bool success = false;
    std::string error_message;
    int user_id = 0;
    std::string employee_id;
    std::vector<std::string> roles;
    std::vector<std::string> permissions;
    TokenPair tokens;
};

class TokenManager {
public:
    TokenManager(const util::JwtConfig& config, DbExecutor& db);
    ~TokenManager() = default;

    // 为设备生成令牌对
    DeviceAuthResult authenticate_device(const std::string& device_id,
                                         const std::string& device_key);

    // 为管理员生成令牌对
    AdminAuthResult authenticate_admin(const std::string& username,
                                       const std::string& password);

    // 刷新令牌
    std::optional<TokenPair> refresh_tokens(const std::string& refresh_token);

    // 验证访问令牌
    std::optional<util::TokenPayload> verify_access_token(const std::string& token);

    // 撤销令牌（用于强制下线）
    void revoke_token(const std::string& jti);

    // 检查令牌是否被撤销
    bool is_token_revoked(const std::string& jti);

    // 清理过期的撤销令牌
    void cleanup_revoked_tokens();

private:
    util::JwtUtil jwt_util_;
    DbExecutor& db_;
    std::mutex mu_;
    std::unordered_set<std::string> revoked_tokens_; // 已撤销的 JTI

    // 验证设备密钥
    bool verify_device_key(const std::string& device_id, const std::string& key);

    // 验证管理员密码
    bool verify_admin_password(const std::string& username, const std::string& password,
                               int& user_id, std::string& employee_id);

    // 加载用户角色和权限
    void load_user_roles_and_permissions(int user_id,
                                         std::vector<std::string>& roles,
                                         std::vector<std::string>& permissions);

    // 生成令牌对
    TokenPair generate_token_pair(const util::TokenPayload& payload);
};

} // namespace service
