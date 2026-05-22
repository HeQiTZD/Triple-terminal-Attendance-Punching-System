# JWT + TLS 认证方案详细计划书

## 一、项目背景

### 1.1 现状分析

当前考勤系统采用基于预共享密钥的认证方式：
- **管理端**：username + password（SHA-256 哈希存储）
- **设备端**：deviceId + deviceKey（明文比较）

### 1.2 存在的安全风险

| 风险类型 | 严重程度 | 说明 |
|---------|---------|------|
| 明文传输 | 高 | 密码和设备密钥在网络上明文传输 |
| 密钥明文存储 | 高 | 设备密钥在配置文件中明文存储 |
| 无令牌过期 | 中 | sessionToken 永久有效，无刷新机制 |
| 重放攻击 | 中 | 缺少 nonce 和时间戳验证 |
| 会话管理缺失 | 中 | 无强制下线、会话超时等机制 |

### 1.3 改造目标

1. **安全性**：消除明文传输和存储风险
2. **标准化**：采用行业标准的 JWT 令牌机制
3. **可扩展性**：支持令牌过期、刷新、权限控制
4. **兼容性**：最小化对现有协议的改动
5. **可维护性**：清晰的架构和文档

---

## 二、技术方案设计

### 2.1 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                      TLS 层 (传输加密)                       │
├─────────────────────────────────────────────────────────────┤
│                    JWT 令牌层 (身份认证)                      │
├─────────────────────────────────────────────────────────────┤
│                  JSON 信封层 (业务消息)                       │
├─────────────────────────────────────────────────────────────┤
│                  TCP 分帧层 (消息分帧)                        │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 核心组件

#### 2.2.1 JWT 令牌结构

**Access Token（访问令牌）**：
```json
{
  "header": {
    "alg": "HS256",
    "typ": "JWT"
  },
  "payload": {
    "sub": "DEV-001",           // 主体（设备ID或用户ID）
    "role": "device",           // 角色（device/admin）
    "iat": 1736400100,          // 签发时间
    "exp": 1736403700,          // 过期时间（默认1小时）
    "jti": "unique-token-id",   // 唯一标识（防重放）
    "permissions": ["attendance.report", "sync.request"]  // 权限列表
  },
  "signature": "HMAC-SHA256(base64(header).base64(payload), secret)"
}
```

**Refresh Token（刷新令牌）**：
```json
{
  "header": {
    "alg": "HS256",
    "typ": "JWT"
  },
  "payload": {
    "sub": "DEV-001",
    "type": "refresh",
    "iat": 1736400100,
    "exp": 1737004900,          // 过期时间（默认7天）
    "jti": "unique-refresh-id"
  },
  "signature": "HMAC-SHA256(base64(header).base64(payload), secret)"
}
```

#### 2.2.2 令牌配置参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| access_token_ttl | 3600 秒 | 访问令牌有效期 |
| refresh_token_ttl | 604800 秒 | 刷新令牌有效期（7天） |
| jwt_secret | 配置文件 | JWT 签名密钥（至少32字节） |
| clock_skew | 30 秒 | 时钟偏差容忍度 |

#### 2.2.3 设备密钥哈希存储

**存储格式**：`$argon2id$v=19$m=65536,t=3,p=4$base64_salt$base64_hash`

**验证流程**：
1. 从配置文件读取设备密钥哈希
2. 使用 Argon2id 算法验证输入密钥
3. 支持旧格式（明文）向后兼容

---

## 三、协议变更

### 3.1 认证流程

#### 3.1.1 设备端认证请求

**请求**：
```json
{
  "type": "auth",
  "role": "device",
  "from": "DEV-001",
  "msgId": "auth-001",
  "ts": 1736400100000,
  "data": {
    "deviceId": "DEV-001",
    "deviceKey": "plaintext_or_hashed_key"
  }
}
```

**成功响应**：
```json
{
  "type": "auth_response",
  "role": "server",
  "from": "server",
  "to": "DEV-001",
  "inReplyTo": "auth-001",
  "ts": 1736400100050,
  "code": 0,
  "msg": "ok",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "refreshToken": "dGhpcyBpcyBhIHJlZnJlc2ggdG9rZW4...",
    "tokenType": "Bearer",
    "expiresIn": 3600,
    "heartbeatSec": 30,
    "roles": [],
    "permissions": ["attendance.report", "sync.request", "device.status.report"]
  }
}
```

**失败响应**：
```json
{
  "type": "auth_response",
  "code": 2002,
  "msg": "invalid credentials",
  "inReplyTo": "auth-001"
}
```

#### 3.1.2 管理端认证请求

**请求**：
```json
{
  "type": "auth",
  "role": "admin",
  "msgId": "auth-002",
  "data": {
    "username": "admin001",
    "password": "secure_password"
  }
}
```

**成功响应**：
```json
{
  "type": "auth_response",
  "code": 0,
  "msg": "ok",
  "inReplyTo": "auth-002",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "refreshToken": "dGhpcyBpcyBhIHJlZnJlc2ggdG9rZW4...",
    "tokenType": "Bearer",
    "expiresIn": 3600,
    "heartbeatSec": 30,
    "roles": ["admin", "super_admin"],
    "permissions": ["person.create", "person.read", "attendance.create", ...]
  }
}
```

### 3.2 令牌刷新流程

#### 3.2.1 刷新请求

**请求**：
```json
{
  "type": "token.refresh",
  "role": "device",
  "from": "DEV-001",
  "msgId": "refresh-001",
  "ts": 1736403600000,
  "data": {
    "refreshToken": "dGhpcyBpcyBhIHJlZnJlc2ggdG9rZW4..."
  }
}
```

**成功响应**：
```json
{
  "type": "token.refresh.response",
  "code": 0,
  "msg": "ok",
  "inReplyTo": "refresh-001",
  "data": {
    "accessToken": "new_access_token...",
    "refreshToken": "new_refresh_token...",
    "expiresIn": 3600
  }
}
```

**失败响应**（刷新令牌过期或无效）：
```json
{
  "type": "token.refresh.response",
  "code": 2004,
  "msg": "invalid or expired refresh token",
  "inReplyTo": "refresh-001"
}
```

### 3.3 业务消息携带令牌

**方式一：信封字段（推荐）**：
```json
{
  "type": "attendance.report",
  "role": "device",
  "from": "DEV-001",
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "msgId": "rpt-001",
  "ts": 1736400200000,
  "data": {
    "employeeId": "EMP-001",
    "checkTime": "2026-05-21 09:00:00",
    "status": "normal"
  }
}
```

**方式二：HTTP 风格（备选）**：
```json
{
  "type": "attendance.report",
  "role": "device",
  "from": "DEV-001",
  "msgId": "rpt-001",
  "ts": 1736400200000,
  "headers": {
    "Authorization": "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
  },
  "data": {...}
}
```

**推荐方式一**，原因：
- 信封字段更简洁
- 与现有协议风格一致
- 解析更简单

### 3.4 错误码扩展

| 错误码 | 常量名 | 说明 |
|--------|--------|------|
| 2001 | kCodeNotAuthenticated | 未认证（无令牌） |
| 2002 | kCodeAuthFailed | 认证失败（凭据错误） |
| 2003 | kCodeDuplicateSession | 重复会话 |
| 2004 | kCodeTokenInvalid | 令牌无效或过期 |
| 2005 | kCodeTokenExpired | 令牌已过期 |
| 2006 | kCodeRefreshTokenInvalid | 刷新令牌无效或过期 |
| 2007 | kCodeInsufficientPermissions | 权限不足 |

---

## 四、详细实现计划

### 4.1 服务端实现

#### 4.1.1 新增模块

**1. JWT 工具类 (`util/JwtUtil.hpp/cpp`)**

```cpp
namespace util {

struct JwtConfig {
    std::string secret;           // 签名密钥
    int access_token_ttl = 3600;  // 访问令牌有效期
    int refresh_token_ttl = 604800; // 刷新令牌有效期
    int clock_skew = 30;          // 时钟偏差容忍度
};

struct TokenPayload {
    std::string sub;              // 主体
    std::string role;             // 角色
    int64_t iat;                  // 签发时间
    int64_t exp;                  // 过期时间
    std::string jti;              // 唯一标识
    std::vector<std::string> permissions; // 权限列表
};

class JwtUtil {
public:
    explicit JwtUtil(const JwtConfig& config);
    
    // 生成访问令牌
    std::string generateAccessToken(const TokenPayload& payload);
    
    // 生成刷新令牌
    std::string generateRefreshToken(const std::string& sub, const std::string& role);
    
    // 验证令牌
    std::optional<TokenPayload> verifyToken(const std::string& token);
    
    // 验证刷新令牌
    std::optional<TokenPayload> verifyRefreshToken(const std::string& token);
    
private:
    JwtConfig config_;
    std::string base64Encode(const std::string& input);
    std::string base64Decode(const std::string& input);
    std::string hmacSha256(const std::string& data, const std::string& key);
};

} // namespace util
```

**2. 令牌管理器 (`service/TokenManager.hpp/cpp`)**

```cpp
namespace service {

struct TokenPair {
    std::string accessToken;
    std::string refreshToken;
    int expiresIn;
};

class TokenManager {
public:
    TokenManager(const util::JwtConfig& config, service::DbExecutor& db);
    
    // 为设备生成令牌对
    TokenPair generateDeviceTokens(const std::string& deviceId, 
                                   const std::vector<std::string>& permissions);
    
    // 为管理员生成令牌对
    TokenPair generateAdminTokens(int userId, const std::string& employeeId,
                                  const std::vector<std::string>& roles,
                                  const std::vector<std::string>& permissions);
    
    // 刷新令牌
    std::optional<TokenPair> refreshTokens(const std::string& refreshToken);
    
    // 验证访问令牌
    std::optional<util::TokenPayload> verifyAccessToken(const std::string& token);
    
    // 撤销令牌（用于强制下线）
    void revokeToken(const std::string& jti);
    
    // 检查令牌是否被撤销
    bool isTokenRevoked(const std::string& jti);
    
private:
    util::JwtUtil jwtUtil_;
    service::DbExecutor& db_;
    
    // 存储已撤销的令牌 JTI（使用 Redis 或数据库）
    void storeRevokedToken(const std::string& jti, int64_t exp);
};

} // namespace service
```

**3. 密码哈希增强 (`util/PasswordHash.hpp/cpp`)**

```cpp
namespace util {

// 新增 Argon2id 哈希
std::string hashPasswordArgon2(const std::string& plain);

// 验证密码（支持旧格式 SHA-256 和新格式 Argon2id）
bool verifyPasswordEnhanced(const std::string& plain, const std::string& stored);

// 哈希设备密钥
std::string hashDeviceKey(const std::string& key);

// 验证设备密钥
bool verifyDeviceKey(const std::string& key, const std::string& stored);

} // namespace util
```

#### 4.1.2 修改现有模块

**1. GatewaySessionHandler 修改**

```cpp
// 新增成员变量
class GatewaySessionHandler {
private:
    std::unique_ptr<service::TokenManager> tokenManager_;
    
    // 修改认证处理函数
    void handleDeviceAuth(net::Session& session, const ParsedEnvelope& env);
    void handleAdminAuth(net::Session& session, const ParsedEnvelope& env);
    
    // 新增令牌验证函数
    bool verifyTokenInEnvelope(const ParsedEnvelope& env, ConnContext& ctx);
    
    // 新增令牌刷新处理
    void handleTokenRefresh(net::Session& session, const ParsedEnvelope& env);
};
```

**2. ConnContext 结构扩展**

```cpp
struct ConnContext {
    // 现有字段...
    
    // 新增字段
    std::string accessToken;      // 当前访问令牌
    std::string refreshToken;     // 当前刷新令牌
    int64_t tokenExpiresAt;       // 令牌过期时间
    std::string jti;              // 当前令牌唯一标识
};
```

**3. ProtocolTypes 扩展**

```cpp
// 新增消息类型
constexpr char kTypeTokenRefresh[] = "token.refresh";
constexpr char kTypeTokenRefreshResponse[] = "token.refresh.response";

// 新增错误码
constexpr int kCodeTokenInvalid = 2004;
constexpr int kCodeTokenExpired = 2005;
constexpr int kCodeRefreshTokenInvalid = 2006;
constexpr int kCodeInsufficientPermissions = 2007;
```

#### 4.1.3 数据库变更

**新增表：revoked_tokens**
```sql
CREATE TABLE revoked_tokens (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    jti VARCHAR(64) NOT NULL UNIQUE,
    revoked_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP NOT NULL,
    INDEX idx_expires_at (expires_at)
);
```

**修改表：device**
```sql
ALTER TABLE device 
ADD COLUMN key_hash VARCHAR(255) COMMENT '设备密钥哈希',
ADD COLUMN key_version INT DEFAULT 1 COMMENT '密钥版本';
```

---

### 4.2 设备端实现

#### 4.2.1 新增模块

**1. JWT 令牌管理器 (`Auth/TokenManager.hpp/cpp`)**

```cpp
class TokenManager : public QObject {
    Q_OBJECT
    
public:
    struct TokenPair {
        QString accessToken;
        QString refreshToken;
        int expiresIn;
    };
    
    explicit TokenManager(QObject* parent = nullptr);
    
    // 存储令牌
    void storeTokens(const TokenPair& tokens);
    
    // 获取访问令牌
    QString accessToken() const;
    
    // 获取刷新令牌
    QString refreshToken() const;
    
    // 检查令牌是否过期
    bool isAccessTokenExpired() const;
    
    // 检查是否需要刷新（提前5分钟）
    bool needsRefresh() const;
    
    // 清除令牌
    void clearTokens();
    
signals:
    void tokenExpired();
    void tokenRefreshed();
    
private:
    QString m_accessToken;
    QString m_refreshToken;
    QDateTime m_expiresAt;
    QSettings m_settings;
    
    void loadFromSettings();
    void saveToSettings();
};
```

**2. 令牌刷新器 (`Auth/TokenRefresher.hpp/cpp`)**

```cpp
class TokenRefresher : public QObject {
    Q_OBJECT
    
public:
    explicit TokenRefresher(NetworkClient* client, TokenManager* tokenManager,
                           QObject* parent = nullptr);
    
    // 启动自动刷新
    void startAutoRefresh();
    
    // 停止自动刷新
    void stopAutoRefresh();
    
    // 手动刷新
    void refreshToken();
    
signals:
    void refreshSuccess();
    void refreshFailed(int code, const QString& message);
    
private:
    NetworkClient* m_client;
    TokenManager* m_tokenManager;
    QTimer* m_refreshTimer;
    
    void onRefreshResponse(const QJsonObject& response);
};
```

#### 4.2.2 修改现有模块

**1. NetworkClient 修改**

```cpp
class NetworkClient : public QObject {
    // 新增成员
private:
    TokenManager* m_tokenManager;
    TokenRefresher* m_tokenRefresher;
    
    // 修改认证响应处理
    void handleAuthResponse(const QJsonObject& message);
    
    // 新增：构建带令牌的消息
    QJsonObject addTokenToMessage(const QJsonObject& message);
    
    // 修改：发送消息前自动添加令牌
    bool sendJson(const QJsonObject& message);
};
```

**2. ServerProtocol 修改**

```cpp
class ServerProtocol {
public:
    // 新增消息类型
    enum class MessageType {
        // 现有类型...
        TokenRefresh,
        TokenRefreshResponse,
    };
    
    // 新增：解析令牌响应
    static TokenPair parseTokenResponse(const QJsonObject& message);
    
    // 新增：构建刷新请求
    static QJsonObject buildTokenRefreshRequest(const QString& refreshToken);
};
```

**3. ConfigManager 扩展**

```cpp
class ConfigManager {
public:
    // 新增配置项
    int getAccessTokenTtl() const;
    int getRefreshTokenTtl() const;
    bool getAutoRefreshEnabled() const;
};
```

---

### 4.3 管理端实现

#### 4.3.1 修改建议

1. **登录流程**：
   - 存储 accessToken 和 refreshToken
   - 监听令牌过期事件
   - 实现自动刷新

2. **请求拦截器**：
   - 自动在请求中添加 Authorization 头
   - 处理 401 错误，自动刷新令牌
   - 刷新失败时跳转登录页

3. **会话管理**：
   - 实现会话超时检测
   - 支持强制下线
   - 显示在线设备/用户列表

---

## 五、TLS 配置

### 5.1 服务端 TLS 配置

#### 5.1.1 证书生成

```bash
# 生成 CA 证书
openssl genrsa -out ca.key 4096
openssl req -new -x509 -days 3650 -key ca.key -out ca.crt \
    -subj "/CN=Attendance CA"

# 生成服务端证书
openssl genrsa -out server.key 2048
openssl req -new -key server.key -out server.csr \
    -subj "/CN=attendance-server"
openssl x509 -req -days 365 -in server.csr -CA ca.crt -CAkey ca.key \
    -CAcreateserial -out server.crt

# 生成设备证书（可选，用于 mTLS）
openssl genrsa -out device.key 2048
openssl req -new -key device.key -out device.csr \
    -subj "/CN=DEV-001"
openssl x509 -req -days 365 -in device.csr -CA ca.crt -CAkey ca.key \
    -CAcreateserial -out device.crt
```

#### 5.1.2 配置文件

```json
{
  "tls": {
    "enabled": true,
    "cert_file": "certs/server.crt",
    "key_file": "certs/server.key",
    "ca_file": "certs/ca.crt",
    "verify_client": false,
    "min_version": "TLSv1.2"
  }
}
```

#### 5.1.3 Boost.Asio TLS 实现

```cpp
#include <boost/asio/ssl.hpp>

class TlsSession : public std::enable_shared_from_this<TlsSession> {
public:
    TlsSession(boost::asio::ip::tcp::socket socket,
               boost::asio::ssl::context& ctx)
        : stream_(std::move(socket), ctx) {}
    
    void start() {
        stream_.async_handshake(
            boost::asio::ssl::stream_base::server,
            [self = shared_from_this()](boost::system::error_code ec) {
                if (!ec) {
                    self->startReading();
                }
            });
    }
    
private:
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream_;
};
```

### 5.2 设备端 TLS 配置

#### 5.2.1 Qt SSL 配置

```cpp
QSslConfiguration sslConfig;
sslConfig.setProtocol(QSsl::TlsV1_2OrLater);

// 加载 CA 证书（验证服务端）
QList<QSslCertificate> caCerts = QSslCertificate::fromPath("certs/ca.crt");
sslConfig.setCaCertificates(caCerts);

// 可选：加载客户端证书（用于 mTLS）
// sslConfig.setLocalCertificate(QSslCertificate::fromPath("certs/device.crt"));
// sslConfig.setPrivateKey(QSslKey::fromPath("certs/device.key"));

QSslSocket* socket = new QSslSocket;
socket->setSslConfiguration(sslConfig);
socket->connectToHostEncrypted("attendance-server", 8080);
```

---

## 六、测试计划

### 6.1 单元测试

#### 6.1.1 JWT 工具测试

```cpp
TEST(JwtUtilTest, GenerateAndVerifyToken) {
    util::JwtConfig config;
    config.secret = "test-secret-key-at-least-32-bytes-long";
    config.access_token_ttl = 3600;
    
    util::JwtUtil jwt(config);
    
    util::TokenPayload payload;
    payload.sub = "DEV-001";
    payload.role = "device";
    payload.iat = std::time(nullptr);
    payload.exp = payload.iat + config.access_token_ttl;
    payload.jti = "test-jti-001";
    payload.permissions = {"attendance.report", "sync.request"};
    
    std::string token = jwt.generateAccessToken(payload);
    ASSERT_FALSE(token.empty());
    
    auto verified = jwt.verifyToken(token);
    ASSERT_TRUE(verified.has_value());
    EXPECT_EQ(verified->sub, "DEV-001");
    EXPECT_EQ(verified->role, "device");
}

TEST(JwtUtilTest, ExpiredToken) {
    util::JwtConfig config;
    config.secret = "test-secret-key-at-least-32-bytes-long";
    
    util::JwtUtil jwt(config);
    
    util::TokenPayload payload;
    payload.sub = "DEV-001";
    payload.iat = std::time(nullptr) - 7200; // 2 hours ago
    payload.exp = std::time(nullptr) - 3600; // 1 hour ago (expired)
    
    std::string token = jwt.generateAccessToken(payload);
    auto verified = jwt.verifyToken(token);
    EXPECT_FALSE(verified.has_value());
}
```

#### 6.1.2 令牌管理器测试

```cpp
TEST(TokenManagerTest, GenerateDeviceTokens) {
    // 测试设备令牌生成
}

TEST(TokenManagerTest, RefreshTokens) {
    // 测试令牌刷新
}

TEST(TokenManagerTest, RevokeToken) {
    // 测试令牌撤销
}
```

### 6.2 集成测试

#### 6.2.1 认证流程测试

```cpp
TEST_F(AuthIntegrationTest, DeviceAuthSuccess) {
    // 1. 建立 TLS 连接
    // 2. 发送认证请求
    // 3. 验证返回的令牌
    // 4. 使用令牌发送业务消息
}

TEST_F(AuthIntegrationTest, DeviceAuthFailure) {
    // 1. 建立 TLS 连接
    // 2. 发送错误的认证请求
    // 3. 验证错误响应
}

TEST_F(AuthIntegrationTest, TokenRefresh) {
    // 1. 认证获取令牌
    // 2. 等待令牌即将过期
    // 3. 刷新令牌
    // 4. 使用新令牌发送消息
}
```

#### 6.2.2 安全测试

```cpp
TEST_F(SecurityTest, ReplayAttack) {
    // 1. 捕获有效的认证请求
    // 2. 重放该请求
    // 3. 验证服务端拒绝
}

TEST_F(SecurityTest, TamperedToken) {
    // 1. 获取有效令牌
    // 2. 篡改令牌内容
    // 3. 使用篡改后的令牌
    // 4. 验证服务端拒绝
}

TEST_F(SecurityTest, ExpiredToken) {
    // 1. 获取令牌
    // 2. 等待令牌过期
    // 3. 使用过期令牌
    // 4. 验证服务端拒绝
}
```

### 6.3 性能测试

#### 6.3.1 令牌验证性能

```cpp
TEST(PerformanceTest, TokenVerificationThroughput) {
    // 测试令牌验证吞吐量
    // 目标：> 10000 次/秒
}

TEST(PerformanceTest, TokenGenerationLatency) {
    // 测试令牌生成延迟
    // 目标：< 10ms
}
```

#### 6.3.2 连接性能

```cpp
TEST(PerformanceTest, TlsHandshakeLatency) {
    // 测试 TLS 握手延迟
    // 目标：< 100ms
}

TEST(PerformanceTest, ConcurrentConnections) {
    // 测试并发连接数
    // 目标：支持 1000+ 并发设备
}
```

### 6.4 兼容性测试

```cpp
TEST(CompatibilityTest, LegacyAuthFallback) {
    // 测试旧格式认证的向后兼容性
}

TEST(CompatibilityTest, MixedEnvironment) {
    // 测试新旧客户端混合环境
}
```

---

## 七、风险评估与缓解

### 7.1 技术风险

| 风险 | 影响 | 概率 | 缓解措施 |
|------|------|------|---------|
| JWT 库依赖问题 | 高 | 低 | 选择成熟的库（如 jwt-cpp） |
| TLS 性能影响 | 中 | 中 | 启用会话复用，优化握手 |
| 令牌存储安全 | 高 | 中 | 使用安全存储（Keychain/加密） |
| 时钟同步问题 | 中 | 低 | 增加时钟偏差容忍度（30秒） |

### 7.2 实施风险

| 风险 | 影响 | 概率 | 缓解措施 |
|------|------|------|---------|
| 升级中断服务 | 高 | 低 | 灰度发布，保留旧接口 |
| 设备固件更新困难 | 中 | 中 | 支持 OTA 更新，向后兼容 |
| 配置迁移复杂 | 中 | 低 | 提供迁移脚本和文档 |

### 7.3 缓解策略

1. **灰度发布**：
   - 阶段1：服务端同时支持新旧认证
   - 阶段2：逐步升级设备端
   - 阶段3：废弃旧认证方式

2. **向后兼容**：
   - 保留旧格式认证（带配置开关）
   - 支持旧格式密钥（明文）验证
   - 提供迁移工具

3. **监控告警**：
   - 监控认证失败率
   - 监控令牌刷新失败率
   - 监控 TLS 握手失败率

---

## 八、实施时间表

### 8.1 阶段一：基础准备（1周）

| 任务 | 负责人 | 时间 |
|------|--------|------|
| 环境搭建和依赖安装 | 开发团队 | 2天 |
| JWT 工具类实现 | 后端开发 | 2天 |
| 密码哈希增强 | 后端开发 | 1天 |
| 单元测试编写 | 测试团队 | 2天 |

### 8.2 阶段二：服务端实现（2周）

| 任务 | 负责人 | 时间 |
|------|--------|------|
| TokenManager 实现 | 后端开发 | 3天 |
| GatewaySessionHandler 修改 | 后端开发 | 4天 |
| 数据库变更 | DBA | 1天 |
| TLS 集成 | 后端开发 | 3天 |
| 集成测试 | 测试团队 | 3天 |

### 8.3 阶段三：设备端实现（1.5周）

| 任务 | 负责人 | 时间 |
|------|--------|------|
| TokenManager 实现 | 嵌入式开发 | 2天 |
| NetworkClient 修改 | 嵌入式开发 | 3天 |
| ServerProtocol 修改 | 嵌入式开发 | 2天 |
| TLS 集成 | 嵌入式开发 | 2天 |
| 集成测试 | 测试团队 | 2天 |

### 8.4 阶段四：管理端适配（1周）

| 任务 | 负责人 | 时间 |
|------|--------|------|
| 登录流程修改 | 前端开发 | 2天 |
| 请求拦截器实现 | 前端开发 | 2天 |
| 会话管理实现 | 前端开发 | 2天 |
| 集成测试 | 测试团队 | 2天 |

### 8.5 阶段五：测试与优化（1周）

| 任务 | 负责人 | 时间 |
|------|--------|------|
| 端到端测试 | 测试团队 | 3天 |
| 性能测试 | 测试团队 | 2天 |
| 安全审计 | 安全团队 | 2天 |
| 文档编写 | 技术文档 | 2天 |

### 8.6 阶段六：部署与监控（1周）

| 任务 | 负责人 | 时间 |
|------|--------|------|
| 灰度发布 | 运维团队 | 2天 |
| 监控配置 | 运维团队 | 1天 |
| 问题修复 | 开发团队 | 2天 |
| 全量发布 | 运维团队 | 2天 |

**总计时间**：约 7.5 周

---

## 九、验收标准

### 9.1 功能验收

- [ ] 设备端认证成功，返回 access_token 和 refresh_token
- [ ] 管理端认证成功，返回 access_token 和 refresh_token
- [ ] 使用 access_token 可以正常发送业务消息
- [ ] access_token 过期后，使用 refresh_token 可以刷新
- [ ] refresh_token 过期后，需要重新认证
- [ ] 令牌撤销后，无法继续使用
- [ ] TLS 连接建立成功，数据传输加密

### 9.2 安全验收

- [ ] 无明文密码和密钥传输
- [ ] 无明文密钥存储
- [ ] 防重放攻击测试通过
- [ ] 令牌篡改测试通过
- [ ] 令牌过期测试通过
- [ ] TLS 证书验证通过

### 9.3 性能验收

- [ ] 令牌验证延迟 < 10ms
- [ ] TLS 握手延迟 < 100ms
- [ ] 支持 1000+ 并发设备连接
- [ ] 认证成功率 > 99.9%

### 9.4 兼容性验收

- [ ] 旧格式认证向后兼容（可配置）
- [ ] 新旧客户端混合环境正常工作
- [ ] 灰度发布过程中无服务中断

---

## 十、附录

### 10.1 参考文档

- [RFC 7519 - JSON Web Token (JWT)](https://tools.ietf.org/html/rfc7519)
- [RFC 5246 - The Transport Layer Security (TLS) Protocol](https://tools.ietf.org/html/rfc5246)
- [OWASP JWT Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/JSON_Web_Token_for_Java_Cheat_Sheet.html)
- [Argon2 Specification](https://github.com/P-H-C/phc-winner-argon2)

### 10.2 依赖库

**服务端**：
- jwt-cpp (C++ JWT 库)
- OpenSSL (TLS 和加密)
- Boost.Asio (网络和 TLS)
- Argon2 (密码哈希)

**设备端**：
- Qt Network (TLS 和网络)
- QJsonWebToken (Qt JWT 库，可选)
- QCryptographicHash (哈希计算)

### 10.3 配置示例

**服务端配置 (`attendance.json`)**：
```json
{
  "tls": {
    "enabled": true,
    "cert_file": "certs/server.crt",
    "key_file": "certs/server.key",
    "ca_file": "certs/ca.crt",
    "verify_client": false
  },
  "jwt": {
    "secret": "your-256-bit-secret-key-here-change-in-production",
    "access_token_ttl": 3600,
    "refresh_token_ttl": 604800,
    "clock_skew": 30
  },
  "auth": {
    "legacy_auth_enabled": true,
    "password_hash_algorithm": "argon2id"
  }
}
```

**设备端配置 (`config.ini`)**：
```ini
[server]
host=attendance-server
port=8080
use_tls=true
ca_cert_path=certs/ca.crt

[auth]
auto_refresh_enabled=true
refresh_before_expiry=300

[device]
id=DEV-001
key=device-secret-key
```

---

**文档版本**：1.0  
**最后更新**：2026-05-21  
**审核人**：[待定]  
**批准人**：[待定]
