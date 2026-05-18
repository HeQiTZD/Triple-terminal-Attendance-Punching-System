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
    Q_PROPERTY(QStringList roles READ roles NOTIFY rolesChanged)
    Q_PROPERTY(QStringList permissions READ permissions NOTIFY permissionsChanged)
    Q_PROPERTY(QString currentUsername READ currentUsername NOTIFY currentUsernameChanged)
    Q_PROPERTY(int connectionState READ connectionState NOTIFY connectionStateChanged)

public:
    explicit SessionManager(QObject *parent = nullptr);

    void setTcpManager(TcpConnectionManager *tcp);

    bool isLoggedIn() const;
    QString sessionToken() const;
    QStringList roles() const;
    QStringList permissions() const;
    QString currentUsername() const;
    int connectionState() const;

    Q_INVOKABLE void login(const QString &host, int port,
                           const QString &username, const QString &password,
                           const QString &clientId = QString());
    Q_INVOKABLE void logout();
    Q_INVOKABLE bool hasPermission(const QString &permKey) const;
    Q_INVOKABLE bool hasRole(const QString &roleKey) const;
    Q_INVOKABLE void refreshPermissions();

signals:
    void loggedInChanged();
    void sessionTokenChanged();
    void rolesChanged();
    void permissionsChanged();
    void currentUsernameChanged();
    void connectionStateChanged();

    void loggedIn(const QString &sessionToken,
                  const QStringList &roles,
                  const QStringList &permissions);
    void loggedOut();
    void loginFailed(int code, const QString &message);
    void permissionsRefreshed(const QStringList &permissions);
    void errorOccurred(const QString &error);

private slots:
    void onTcpStateChanged(int oldState, int newState);
    void onTcpAuthenticated(const QString &token,
                            const QStringList &roleList,
                            const QStringList &permList);
    void onTcpAuthFailed(int code, const QString &msg);
    void onTcpMessageReceived(const QJsonObject &message);
    void onTcpError(const QString &error);

private:
    void clearSession();
    void processPermissionSelfResponse(const QJsonObject &response);

    TcpConnectionManager *m_tcp = nullptr;
    bool m_isLoggedIn = false;
    QString m_sessionToken;
    QStringList m_roles;
    QStringList m_permissions;
    QString m_currentUsername;
    int m_authEpoch = 0;
};

#endif // SESSIONMANAGER_H
