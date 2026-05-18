#include "SessionManager.h"
#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"
#include <QUuid>

namespace {

QStringList parseStringOrKeyedArray(const QJsonArray &arr, const QString &objectKeyField)
{
    QStringList result;
    for (const auto &v : arr) {
        if (v.isString()) {
            const QString s = v.toString();
            if (!s.isEmpty())
                result.append(s);
            continue;
        }
        if (!v.isObject())
            continue;
        const QJsonObject o = v.toObject();
        QString key = o.value(objectKeyField).toString();
        if (key.isEmpty())
            key = o.value(QStringLiteral("key")).toString();
        if (!key.isEmpty())
            result.append(key);
    }
    return result;
}

} // namespace

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
{
}

void SessionManager::setTcpManager(TcpConnectionManager *tcp)
{
    if (m_tcp == tcp)
        return;

    if (m_tcp) {
        QObject::disconnect(m_tcp, nullptr, this, nullptr);
    }

    m_tcp = tcp;

    if (m_tcp) {
        // stateChanged uses the ConnectionState enum, convert to int for the slot
        connect(m_tcp, &TcpConnectionManager::stateChanged,
                this, [this](TcpConnectionManager::ConnectionState oldSt,
                             TcpConnectionManager::ConnectionState newSt) {
            onTcpStateChanged(static_cast<int>(oldSt), static_cast<int>(newSt));
        });
        connect(m_tcp, &TcpConnectionManager::authenticated,
                this, &SessionManager::onTcpAuthenticated);
        connect(m_tcp, &TcpConnectionManager::authFailed,
                this, &SessionManager::onTcpAuthFailed);
        connect(m_tcp, &TcpConnectionManager::messageReceived,
                this, &SessionManager::onTcpMessageReceived);
        connect(m_tcp, &TcpConnectionManager::errorOccurred,
                this, &SessionManager::onTcpError);
    }
}

bool SessionManager::isLoggedIn() const { return m_isLoggedIn; }

QString SessionManager::sessionToken() const { return m_sessionToken; }

QStringList SessionManager::roles() const { return m_roles; }

QStringList SessionManager::permissions() const { return m_permissions; }

QString SessionManager::currentUsername() const { return m_currentUsername; }

int SessionManager::connectionState() const
{
    if (!m_tcp)
        return 0;
    return static_cast<int>(m_tcp->connectionState());
}

void SessionManager::login(const QString &host, int port,
                           const QString &username, const QString &password,
                           const QString &clientId)
{
    if (!m_tcp) {
        emit errorOccurred(QStringLiteral("TCP manager not initialized"));
        return;
    }

    ++m_authEpoch;

    // 每次登录使用新连接与会话标识，避免服务端按固定 from 复用旧会话权限
    if (m_tcp->connectionState() != TcpConnectionManager::ConnectionState::Disconnected)
        m_tcp->disconnectFromServer();
    clearSession();

    m_currentUsername = username;

    TcpConnectionManager::ConnectionConfig config;
    config.host = host;
    config.port = static_cast<quint16>(port);
    config.clientId = clientId.isEmpty()
        ? QStringLiteral("admin_%1_%2")
              .arg(username, QUuid::createUuid().toString(QUuid::WithoutBraces))
        : clientId;
    config.username = username;
    config.password = password;

    m_tcp->connectToServer(config);
}

void SessionManager::logout()
{
    ++m_authEpoch;

    if (m_tcp) {
        m_tcp->disconnectFromServer();
    }
    clearSession();
    emit loggedOut();
}

bool SessionManager::hasPermission(const QString &permKey) const
{
    return m_permissions.contains(permKey);
}

bool SessionManager::hasRole(const QString &roleKey) const
{
    return m_roles.contains(roleKey);
}

void SessionManager::refreshPermissions()
{
    if (!m_tcp || !m_isLoggedIn) {
        emit errorOccurred(QStringLiteral("Not logged in, cannot refresh permissions"));
        return;
    }

    const int epoch = m_authEpoch;

    QJsonObject msg;
    msg[Protocol::kType] = Protocol::kPermissionSelf;

    m_tcp->sendMessage(msg, [this, epoch](const QJsonObject &response) {
        if (epoch != m_authEpoch || !m_isLoggedIn)
            return;
        if (response.isEmpty()) {
            emit errorOccurred(QStringLiteral("Permission refresh timed out"));
            return;
        }
        processPermissionSelfResponse(response);
    });
}

void SessionManager::clearSession()
{
    m_isLoggedIn = false;
    m_sessionToken.clear();
    m_roles.clear();
    m_permissions.clear();
    m_currentUsername.clear();

    emit sessionTokenChanged();
    emit rolesChanged();
    emit permissionsChanged();
    emit currentUsernameChanged();
    emit loggedInChanged();
}

void SessionManager::processPermissionSelfResponse(const QJsonObject &response)
{
    if (!m_isLoggedIn)
        return;

    int code = response[Protocol::kCode].toInt(-1);
    if (code != Protocol::ErrorCode::kSuccess)
        return;

    QJsonObject data = response[Protocol::kData].toObject();
    const QStringList updatedPerms = parseStringOrKeyedArray(
        data[Protocol::kPermissions].toArray(), Protocol::kPermKey);

    if (updatedPerms != m_permissions) {
        m_permissions = updatedPerms;
        emit permissionsChanged();
        emit permissionsRefreshed(m_permissions);
    }
}

void SessionManager::onTcpStateChanged(int oldState, int newState)
{
    Q_UNUSED(oldState)
    emit connectionStateChanged();

    // ConnectionState::Disconnected == 0
    if (newState == 0 && m_isLoggedIn) {
        clearSession();
        emit loggedOut();
    }
}

void SessionManager::onTcpAuthenticated(const QString &token,
                                         const QStringList &roleList,
                                         const QStringList &permList)
{
    if (!m_currentUsername.isEmpty() && m_tcp
        && m_tcp->config().username != m_currentUsername) {
        return;
    }

    m_sessionToken = token;
    m_roles = roleList;
    m_permissions = permList;

    bool wasLoggedIn = m_isLoggedIn;
    m_isLoggedIn = true;

    emit sessionTokenChanged();
    emit rolesChanged();
    emit permissionsChanged();
    if (!wasLoggedIn)
        emit loggedInChanged();

    emit loggedIn(m_sessionToken, m_roles, m_permissions);
    refreshPermissions();
}

void SessionManager::onTcpAuthFailed(int code, const QString &msg)
{
    m_isLoggedIn = false;
    emit loggedInChanged();
    emit loginFailed(code, msg);
}

void SessionManager::onTcpMessageReceived(const QJsonObject &message)
{
    QString type = message[Protocol::kType].toString();

    // 处理权限热更新推送（服务端可能在角色变更后主动推送）
    if (type == Protocol::responseType(Protocol::kPermissionSelf)) {
        processPermissionSelfResponse(message);
    }
}

void SessionManager::onTcpError(const QString &error)
{
    emit errorOccurred(error);
}
