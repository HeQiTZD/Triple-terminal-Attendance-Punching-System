#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QSettings>

class TokenManager : public QObject
{
    Q_OBJECT

public:
    struct TokenPair {
        QString accessToken;
        QString refreshToken;
        int expiresIn;
    };

    explicit TokenManager(QObject *parent = nullptr);
    ~TokenManager() = default;

    // 存储令牌
    void storeTokens(const TokenPair &tokens);

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

    // 从设置中加载令牌
    void loadFromSettings();

    // 保存令牌到设置
    void saveToSettings();

signals:
    void tokenExpired();
    void tokenRefreshed();

private:
    QString m_accessToken;
    QString m_refreshToken;
    QDateTime m_expiresAt;
    QSettings m_settings;
};

#endif // TOKENMANAGER_H
