#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QStringList>
#include <QTimer>

class TcpConnectionManager;

class SessionManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString sessionToken READ sessionToken NOTIFY sessionTokenChanged)
    Q_PROPERTY(QString accessToken READ accessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(QString refreshToken READ refreshToken NOTIFY refreshTokenChanged)
    Q_PROPERTY(QStringList roles READ roles NOTIFY rolesChanged)
    Q_PROPERTY(QStringList permissions READ permissions NOTIFY permissionsChanged)
    Q_PROPERTY(QString currentUsername READ currentUsername NOTIFY currentUsernameChanged)
    Q_PROPERTY(int connectionState READ connectionState NOTIFY connectionStateChanged)

public:
    explicit SessionManager(QObject *parent = nullptr);

    void setTcpManager(TcpConnectionManager *tcp);

    bool isLoggedIn() const;
    QString sessionToken() const;
    QString accessToken() const;
    QString refreshToken() const;
    QStringList roles() const;
    QStringList permissions() const;
    QString currentUsername() const;
    int connectionState() const;

    Q_INVOKABLE void login(const QString &host, int port,
                           const QString &username, const QString &password,
                           const QString &clientId = QString());
    Q_INVOKABLE void logout();
    Q_INVOKABLE void devLogin();
    Q_INVOKABLE bool hasPermission(const QString &permKey) const;
    Q_INVOKABLE bool hasRole(const QString &roleKey) const;
    Q_INVOKABLE void refreshPermissions();

signals:
    void loggedInChanged();
    void sessionTokenChanged();
    void accessTokenChanged();
    void refreshTokenChanged();
    void rolesChanged();
    void permissionsChanged();
    void currentUsernameChanged();
    void connectionStateChanged();

    void loggedIn(const QString &sessionToken,
                  const QStringList &roles,
                  const QStringList &permissions);
    void loggedOut();
    void loginFailed(int code, const QString &message);
    void tokenRefreshed(const QString &accessToken, const QString &refreshToken);
    void tokenRefreshFailed(int code, const QString &message);
    void permissionsRefreshed(const QStringList &permissions);
    void errorOccurred(const QString &error);

public slots:
    void refreshTokens();

private slots:
    void onTcpStateChanged(int oldState, int newState);
    void onTcpAuthenticated(const QString &token,
                            const QStringList &roleList,
                            const QStringList &permList);
    void onTcpAuthFailed(int code, const QString &msg);
    void onTcpTokenRefreshed(const QString &accessToken, const QString &refreshToken);
    void onTcpTokenRefreshFailed(int code, const QString &msg);
    void onTcpMessageReceived(const QJsonObject &message);
    void onTcpError(const QString &error);

private:
    void clearSession();
    void processPermissionSelfResponse(const QJsonObject &response);

    TcpConnectionManager *m_tcp = nullptr;
    bool m_isLoggedIn = false;
    bool m_isDevSession = false;
    QString m_sessionToken;
    QString m_accessToken;
    QString m_refreshToken;
    QStringList m_roles;
    QStringList m_permissions;
    QString m_currentUsername;
    int m_authEpoch = 0;
};

#endif // SESSIONMANAGER_H
