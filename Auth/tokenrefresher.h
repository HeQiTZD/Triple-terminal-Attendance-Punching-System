#ifndef TOKENREFRESHER_H
#define TOKENREFRESHER_H

#include <QObject>
#include <QTimer>

#include "tokenmanager.h"

class Networkclient;

class TokenRefresher : public QObject
{
    Q_OBJECT

public:
    explicit TokenRefresher(Networkclient *client, TokenManager *tokenManager,
                           QObject *parent = nullptr);
    ~TokenRefresher() = default;

    // 启动自动刷新
    void startAutoRefresh();

    // 停止自动刷新
    void stopAutoRefresh();

    // 手动刷新
    void refreshToken();

signals:
    void refreshSuccess();
    void refreshFailed(int code, const QString &message);

private slots:
    void onRefreshTimerTimeout();
    void onTokenRefreshResponse(const QJsonObject &response);

private:
    Networkclient *m_client;
    TokenManager *m_tokenManager;
    QTimer *m_refreshTimer;

    void setupConnections();
};

#endif // TOKENREFRESHER_H
