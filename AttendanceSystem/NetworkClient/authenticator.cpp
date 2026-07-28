#include "authenticator.h"
#include "messagewriter.h"
#include "heartbeatmanager.h"
#include "connectionmanager.h"
#include "serverprotocol.h"
#include "../Auth/tokenmanager.h"
#include "../Auth/tokenrefresher.h"
#include "../Config/configmanager.h"
#include "../Utils/Logger.h"

Authenticator::Authenticator(TokenManager *tokenManager,
                             TokenRefresher *tokenRefresher,
                             QObject *parent)
    : QObject(parent)
    , m_tokenManager(tokenManager)
    , m_tokenRefresher(tokenRefresher)
{
}

void Authenticator::sendAuthRequest(const QString &deviceId, const QString &deviceKey,
                                    MessageWriter *writer)
{
    if (!writer) {
        LOG_NET(Logger::Error, "认证请求失败", "操作=MessageWriter为空");
        return;
    }

    m_isAuthenticated = false;
    m_sessionToken.clear();

    const QJsonObject msg = ServerProtocol::buildAuth(deviceId, deviceKey);
    writer->send(msg);

    LOG_NET(Logger::Info, "发送认证请求", QStringLiteral("deviceId=%1").arg(deviceId));
}

void Authenticator::handleAuthResponse(const QJsonObject &message,
                                       HeartbeatManager *heartbeat,
                                       ConnectionManager *connection)
{
    int code = message.value(QStringLiteral("code")).toInt(-1);
    if (code == -1) {
        const QJsonObject d = message.value(QStringLiteral("data")).toObject();
        code = d.value(QStringLiteral("code")).toInt(0);
    }

    if (code == ServerProtocol::kCodeOk) {
        // ---------- 认证成功 ----------

        // 获取响应数据
        const QJsonObject data = message.value(QStringLiteral("data")).toObject();

        // sessionToken (兼容旧版本)
        m_sessionToken = message.value(QStringLiteral("sessionToken")).toString();
        if (m_sessionToken.isEmpty()) {
            m_sessionToken = data.value(QStringLiteral("sessionToken")).toString();
        }

        // JWT 令牌
        const QString accessToken = data.value(QStringLiteral("accessToken")).toString();
        const QString refreshToken = data.value(QStringLiteral("refreshToken")).toString();
        const int expiresIn = data.value(QStringLiteral("expiresIn")).toInt(3600);

        if (!accessToken.isEmpty() && !refreshToken.isEmpty()) {
            // 存储 JWT 令牌
            TokenManager::TokenPair tokens;
            tokens.accessToken = accessToken;
            tokens.refreshToken = refreshToken;
            tokens.expiresIn = expiresIn;
            m_tokenManager->storeTokens(tokens);

            // 启动自动刷新
            m_tokenRefresher->startAutoRefresh();

            LOG_NET(Logger::Info, "JWT令牌已存储", QStringLiteral("expiresIn=%1").arg(expiresIn));
        }

        // 检查是否收到新密钥
        const QString newDeviceKey = data.value(QStringLiteral("deviceKey")).toString();
        if (!newDeviceKey.isEmpty()) {
            // 通知 NetworkClient 更新密钥并重连
            emit deviceKeyUpdated(newDeviceKey);
            return;  // 不继续执行后续的正常认证成功逻辑，交给重连
        }

        // 心跳间隔
        int heartbeatSec = message.value(QStringLiteral("heartbeatSec")).toInt(0);
        if (heartbeatSec == 0) {
            heartbeatSec = data.value(QStringLiteral("heartbeatSec")).toInt(0);
        }
        if (heartbeatSec > 0)
            heartbeat->setHeartbeatInterval(heartbeatSec);

        // 认证成功后设置状态
        m_isAuthenticated = true;
        connection->setAuthenticated(true);

        // 认证成功后启动心跳
        heartbeat->start(heartbeatSec > 0 ? heartbeatSec : 30);

        LOG_NET(Logger::Info, "认证成功", QStringLiteral("心跳间隔=%1秒").arg(heartbeatSec));

        emit authSuccess();

    } else if (code == ServerProtocol::kCodeAuthFailed) {
        // ---------- 2002：认证失败（凭据错误或待审核）----------
        m_isAuthenticated = false;
        const QString msgText = message.value(QStringLiteral("msg")).toString();

        // 检查是否为设备待审核状态
        if (msgText.contains(QStringLiteral("pending authorization"))) {
            LOG_NET(Logger::Warn, "设备待审核", "状态=等待管理员审批");
            emit devicePendingAuth();
        } else {
            LOG_NET(Logger::Error, "认证失败", QStringLiteral("错误码=2002 | 详情=凭据无效: %1").arg(msgText));
            emit authFailed(code, msgText);
        }

    } else if (code == ServerProtocol::kCodeDuplicateSession) {
        // ---------- 2003：重复会话 ----------
        m_isAuthenticated = false;
        const QString msg = message.value(QStringLiteral("msg")).toString();
        LOG_NET(Logger::Warn, "认证失败", QStringLiteral("错误码=2003 | 详情=重复会话: %1").arg(msg));
        connection->disconnect();
        emit authFailed(code, msg);

    } else {
        // ---------- 其他错误 ----------
        m_isAuthenticated = false;
        connection->setAuthenticated(false);
        const QString msg = message.value(QStringLiteral("msg")).toString();
        LOG_NET(Logger::Error, "认证失败", QStringLiteral("错误码=%1 | 详情=%2").arg(code).arg(msg));
        emit authFailed(code, msg);
    }
}
