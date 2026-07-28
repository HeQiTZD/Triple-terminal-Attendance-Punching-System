#ifndef AUTHENTICATOR_H
#define AUTHENTICATOR_H

#include <QObject>
#include <QJsonObject>

class TokenManager;
class TokenRefresher;
class MessageWriter;
class HeartbeatManager;
class ConnectionManager;

class Authenticator : public QObject
{
    Q_OBJECT

public:
    explicit Authenticator(TokenManager *tokenManager,
                          TokenRefresher *tokenRefresher,
                          QObject *parent = nullptr);

    // 由 NetworkClient::onConnectionConnected() 调用，发送首包 auth 请求
    void sendAuthRequest(const QString &deviceId, const QString &deviceKey,
                         MessageWriter *writer);

    // 由 NetworkClient::onMessageReceived() 路由过来
    void handleAuthResponse(const QJsonObject &message,
                            HeartbeatManager *heartbeat,
                            ConnectionManager *connection);

    bool isAuthenticated() const { return m_isAuthenticated; }
    void setAuthenticated(bool authenticated) { m_isAuthenticated = authenticated; }

    // 会话令牌（兼容旧版本）
    QString sessionToken() const { return m_sessionToken; }

signals:
    void authSuccess();
    void authFailed(int code, const QString &message);
    void devicePendingAuth();
    void deviceKeyUpdated(const QString &newDeviceKey);  // 密钥轮换时通知 NetworkClient 重连

private:
    TokenManager   *m_tokenManager;    // 不持有所有权
    TokenRefresher *m_tokenRefresher;  // 不持有所有权
    bool m_isAuthenticated = false;
    QString m_sessionToken;
};

#endif // AUTHENTICATOR_H
