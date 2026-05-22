#include "tokenrefresher.h"
#include "../NetworkClient/networkclient.h"
#include "../NetworkClient/serverprotocol.h"

#include <QDebug>

TokenRefresher::TokenRefresher(NetworkClient *client, TokenManager *tokenManager,
                               QObject *parent)
    : QObject(parent)
    , m_client(client)
    , m_tokenManager(tokenManager)
    , m_refreshTimer(new QTimer(this))
{
    m_refreshTimer->setSingleShot(true);
    connect(m_refreshTimer, &QTimer::timeout, this, &TokenRefresher::onRefreshTimerTimeout);
    setupConnections();
}

void TokenRefresher::startAutoRefresh()
{
    // 每分钟检查一次是否需要刷新
    m_refreshTimer->start(60000);
    qDebug() << "TokenRefresher: 自动刷新已启动";
}

void TokenRefresher::stopAutoRefresh()
{
    m_refreshTimer->stop();
    qDebug() << "TokenRefresher: 自动刷新已停止";
}

void TokenRefresher::refreshToken()
{
    if (!m_client || !m_client->isConnected()) {
        qDebug() << "TokenRefresher: 未连接，无法刷新令牌";
        return;
    }

    QString refreshToken = m_tokenManager->refreshToken();
    if (refreshToken.isEmpty()) {
        qDebug() << "TokenRefresher: 刷新令牌为空";
        return;
    }

    QJsonObject msg = ServerProtocol::buildTokenRefreshRequest(refreshToken);
    m_client->sendJson(msg);
    qDebug() << "TokenRefresher: 已发送令牌刷新请求";
}

void TokenRefresher::onRefreshTimerTimeout()
{
    if (m_tokenManager->needsRefresh()) {
        refreshToken();
    }
    // 重新启动定时器
    m_refreshTimer->start(60000);
}

void TokenRefresher::onTokenRefreshResponse(const QJsonObject &response)
{
    int code = response.value("code").toInt(-1);
    if (code == -1) {
        const QJsonObject d = response.value("data").toObject();
        code = d.value("code").toInt(0);
    }

    if (code == 0) {
        // 刷新成功
        const QJsonObject data = response.value("data").toObject();
        TokenManager::TokenPair tokens;
        tokens.accessToken = data.value("accessToken").toString();
        tokens.refreshToken = data.value("refreshToken").toString();
        tokens.expiresIn = data.value("expiresIn").toInt(3600);

        m_tokenManager->storeTokens(tokens);
        qDebug() << "TokenRefresher: 令牌刷新成功";
        emit refreshSuccess();
    } else {
        // 刷新失败
        QString msg = response.value("msg").toString();
        qDebug() << "TokenRefresher: 令牌刷新失败, code=" << code << "msg=" << msg;
        emit refreshFailed(code, msg);
    }
}

void TokenRefresher::setupConnections()
{
    // 连接 NetworkClient 的信号
    connect(m_client, &NetworkClient::tokenRefreshResponse,
            this, &TokenRefresher::onTokenRefreshResponse);
}
