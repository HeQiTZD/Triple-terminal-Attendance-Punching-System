#include "service/TokenManager.hpp"
#include "service/DbExecutor.hpp"
#include "util/PasswordHash.hpp"

#include <mysqlx/xdevapi.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <future>
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace service {

TokenManager::TokenManager(const util::JwtConfig& config, DbExecutor& db)
    : jwt_util_(config), db_(db) {}

DeviceAuthResult TokenManager::authenticate_device(const std::string& device_id,
                                                    const std::string& device_key) {
    DeviceAuthResult result;

    // 验证设备密钥
    if (!verify_device_key(device_id, device_key)) {
        result.error_message = "invalid device key";
        return result;
    }

    // 生成令牌
    util::TokenPayload payload;
    payload.sub = device_id;
    payload.role = "device";
    payload.permissions = {"attendance.report", "sync.request", "device.status.report"};

    result.tokens = generate_token_pair(payload);
    result.success = true;

    return result;
}

AdminAuthResult TokenManager::authenticate_admin(const std::string& username,
                                                  const std::string& password) {
    AdminAuthResult result;

    // 验证密码
    if (!verify_admin_password(username, password, result.user_id, result.employee_id)) {
        result.error_message = "invalid credentials";
        return result;
    }

    // 加载角色和权限
    load_user_roles_and_permissions(result.user_id, result.roles, result.permissions);

    // 生成令牌
    util::TokenPayload payload;
    payload.sub = result.employee_id;
    payload.role = "admin";
    payload.user_id = result.user_id;
    payload.employee_id = result.employee_id;
    payload.permissions = result.permissions;

    result.tokens = generate_token_pair(payload);
    result.success = true;

    return result;
}

std::optional<TokenPair> TokenManager::refresh_tokens(const std::string& refresh_token) {
    // 验证刷新令牌
    auto payload = jwt_util_.verify_refresh_token(refresh_token);
    if (!payload) {
        return std::nullopt;
    }

    // 检查是否被撤销
    if (is_token_revoked(payload->jti)) {
        return std::nullopt;
    }

    // 撤销旧的刷新令牌
    revoke_token(payload->jti);

    // 生成新的令牌对
    util::TokenPayload new_payload;
    new_payload.sub = payload->sub;
    new_payload.role = payload->role;
    new_payload.user_id = payload->user_id;
    new_payload.employee_id = payload->employee_id;
    new_payload.permissions = payload->permissions;

    return generate_token_pair(new_payload);
}

std::optional<util::TokenPayload> TokenManager::verify_access_token(const std::string& token) {
    auto payload = jwt_util_.verify_access_token(token);
    if (!payload) {
        return std::nullopt;
    }

    // 检查是否被撤销
    if (is_token_revoked(payload->jti)) {
        return std::nullopt;
    }

    return payload;
}

void TokenManager::revoke_token(const std::string& jti) {
    std::lock_guard<std::mutex> lock(mu_);
    revoked_tokens_.insert(jti);
}

bool TokenManager::is_token_revoked(const std::string& jti) {
    std::lock_guard<std::mutex> lock(mu_);
    return revoked_tokens_.find(jti) != revoked_tokens_.end();
}

void TokenManager::cleanup_revoked_tokens() {
    // TODO: 实现定期清理过期的撤销令牌
    // 可以使用定时器或后台线程
}

bool TokenManager::verify_device_key(const std::string& device_id, const std::string& key) {
    try {
        auto promise = std::make_shared<std::promise<bool>>();
        auto future = promise->get_future();
        db_.dispatch(
            [promise, device_id, key](mysqlx::Session& s) {
                bool ok = false;
                s.sql("USE attendanceserver").execute();
                auto res = s.sql("SELECT key_hash FROM Device WHERE device_id = ?")
                    .bind(device_id)
                    .execute();
                auto row = res.fetchOne();
                if (row) {
                    if (row[0].isNull()) {
                        // key_hash 为 NULL，使用默认密码
                        ok = (key == "changeme");
                    } else {
                        std::string stored_key = row[0].get<std::string>();
                        if (stored_key.empty()) {
                            ok = (key == "changeme");
                        } else {
                            ok = (stored_key == key);  // 直接比较明文
                        }
                    }
                }
                promise->set_value(ok);
            },
            [promise](std::exception_ptr ep) {
                if (ep) {
                    try {
                        promise->set_exception(ep);
                    } catch (...) {}
                }
            });
        return future.get();
    } catch (const std::exception& ex) {
        std::cerr << "verify_device_key exception: " << ex.what() << std::endl;
        return false;
    }
}

bool TokenManager::verify_admin_password(const std::string& username, const std::string& password,
                                          int& user_id, std::string& employee_id) {
    try {
        auto promise = std::make_shared<std::promise<bool>>();
        auto future = promise->get_future();
        db_.dispatch(
            [promise, &user_id, &employee_id, username, password](mysqlx::Session& s) {
                s.sql("USE attendanceserver").execute();

                auto res = s.sql("SELECT id, employee_id, password FROM UserAccount WHERE employee_id = ?")
                    .bind(username)
                    .execute();
                auto row = res.fetchOne();
                if (!row) {
                    promise->set_value(false);
                    return;
                }

                user_id = row[0].get<int>();
                employee_id = row[1].get<std::string>();
                std::string stored_password = row[2].get<std::string>();

                bool ok = false;
                if (stored_password.find('$') != std::string::npos) {
                    ok = util::verify_password(password, stored_password);
                } else {
                    ok = (stored_password == password);
                }

                if (ok) {
                    s.sql("UPDATE UserAccount SET last_login_time = NOW() WHERE id = ?")
                        .bind(user_id)
                        .execute();
                }
                promise->set_value(ok);
            },
            [promise](std::exception_ptr ep) {
                if (ep) {
                    try {
                        promise->set_exception(ep);
                    } catch (...) {}
                }
            });
        return future.get();
    } catch (const std::exception& ex) {
        std::cerr << "verify_admin_password exception: " << ex.what() << std::endl;
        return false;
    }
}

void TokenManager::load_user_roles_and_permissions(int user_id,
                                                    std::vector<std::string>& roles,
                                                    std::vector<std::string>& permissions) {
    try {
        auto promise = std::make_shared<std::promise<void>>();
        auto future = promise->get_future();
        db_.dispatch(
            [promise, &roles, &permissions, user_id](mysqlx::Session& s) {
                s.sql("USE attendanceserver").execute();

                auto role_res = s.sql(
                    "SELECT r.role_key FROM Role r "
                    "INNER JOIN UserRole ur ON r.id = ur.role_id "
                    "WHERE ur.user_id = ?")
                    .bind(user_id)
                    .execute();
                for (auto row : role_res) {
                    roles.push_back(row[0].get<std::string>());
                }

                auto perm_res = s.sql(
                    "SELECT DISTINCT p.perm_key FROM Permission p "
                    "INNER JOIN RolePermission rp ON p.id = rp.permission_id "
                    "INNER JOIN UserRole ur ON rp.role_id = ur.role_id "
                    "WHERE ur.user_id = ?")
                    .bind(user_id)
                    .execute();
                for (auto row : perm_res) {
                    permissions.push_back(row[0].get<std::string>());
                }
                promise->set_value();
            },
            [promise](std::exception_ptr ep) {
                if (ep) {
                    try {
                        promise->set_exception(ep);
                    } catch (...) {}
                }
            });
        future.get();
    } catch (const std::exception& ex) {
        std::cerr << "load_user_roles_and_permissions exception: " << ex.what() << std::endl;
    }
}

TokenPair TokenManager::generate_token_pair(const util::TokenPayload& payload) {
    TokenPair pair;
    pair.access_token = jwt_util_.generate_access_token(payload);
    pair.refresh_token = jwt_util_.generate_refresh_token(payload.sub, payload.role);
    pair.expires_in = 3600; // 默认1小时
    return pair;
}

} // namespace service
