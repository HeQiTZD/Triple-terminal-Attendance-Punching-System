#include "tokenrefresher.h"
#include "../NetworkClient/networkclient.h"
#include "../NetworkClient/serverprotocol.h"

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
    m_refreshTimer->start(60000);
}

void TokenRefresher::stopAutoRefresh()
{
    m_refreshTimer->stop();
}

void TokenRefresher::refreshToken()
{
    if (!m_client || !m_client->isConnected()) {
        return;
    }

    QString refreshToken = m_tokenManager->refreshToken();
    if (refreshToken.isEmpty()) {
        return;
    }

    QJsonObject msg = ServerProtocol::buildTokenRefreshRequest(refreshToken);
    m_client->sendJson(msg);
}

void TokenRefresher::onRefreshTimerTimeout()
{
    if (m_tokenManager->needsRefresh()) {
        refreshToken();
    }
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
        const QJsonObject data = response.value("data").toObject();
        TokenManager::TokenPair tokens;
        tokens.accessToken = data.value("accessToken").toString();
        tokens.refreshToken = data.value("refreshToken").toString();
        tokens.expiresIn = data.value("expiresIn").toInt(3600);

        m_tokenManager->storeTokens(tokens);
        emit refreshSuccess();
    } else {
        QString msg = response.value("msg").toString();
        emit refreshFailed(code, msg);
    }
}

void TokenRefresher::setupConnections()
{
    connect(m_client, &NetworkClient::tokenRefreshResponse,
            this, &TokenRefresher::onTokenRefreshResponse);
}
