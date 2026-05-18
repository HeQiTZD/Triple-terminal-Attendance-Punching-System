#ifndef TCPCONNECTIONMANAGER_H
#define TCPCONNECTIONMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QTimer>
#include <QTcpSocket>
#include <QMap>
#include <QJsonArray>
#include <functional>
#include "../Protocol/protocol.h"
class TcpConnectionManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ConnectionState connectionState READ connectionState NOTIFY stateChanged)
    Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY authenticatedChanged)
    Q_PROPERTY(QString sessionToken READ sessionToken NOTIFY sessionTokenChanged)
    Q_PROPERTY(QStringList roles READ roles NOTIFY rolesChanged)
    Q_PROPERTY(QStringList permissions READ permissions NOTIFY permissionsChanged)

public:
    enum class ConnectionState {
        Disconnected,
        Connecting,
        Connected,
        Authenticated
    };
    Q_ENUM(ConnectionState)

    struct ConnectionConfig {
        QString host           = QStringLiteral("127.0.0.1");
        quint16 port           = 9527;
        QString clientId       = QStringLiteral("admin_001");
        QString username;
        QString password;
        int heartbeatSec       = 30;
        int reconnectMaxAttempts = 10;
        int reconnectBaseDelayMs  = 1000;
        int requestTimeoutMs      = 10000;
    };

    using ResponseCallback = std::function<void(const QJsonObject &response)>;

    explicit TcpConnectionManager(QObject *parent = nullptr);
    ~TcpConnectionManager() override;

    ConnectionState connectionState() const;
    bool isAuthenticated() const;
    QString sessionToken() const;
    QStringList roles() const;
    QStringList permissions() const;
    const ConnectionConfig &config() const;

public slots:
    void connectToServer(const TcpConnectionManager::ConnectionConfig &config);
    void disconnectFromServer();
    QString sendMessage(const QJsonObject &message, ResponseCallback callback = nullptr);
    void sendBinaryFrame(const QByteArray &payload);

signals:
    void stateChanged(TcpConnectionManager::ConnectionState oldState,
                      TcpConnectionManager::ConnectionState newState);
    void authenticatedChanged();
    void sessionTokenChanged();
    void rolesChanged();
    void permissionsChanged();

    void authenticated(const QString &sessionToken,
                       const QStringList &roles,
                       const QStringList &permissions);
    void authFailed(int code, const QString &msg);
    void messageReceived(const QJsonObject &message);
    /** 每条 JSON 行发出/收到时触发，供 QML History 记录（密码已脱敏） */
    void jsonMessageSent(const QJsonObject &message);
    void jsonMessageReceived(const QJsonObject &message);
    void binaryFrameReceived(const QByteArray &payload);
    void errorOccurred(const QString &error);
    void heartbeatAcknowledged();

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketErrorOccurred(QAbstractSocket::SocketError error);
    void onHeartbeatTimeout();
    void onReconnectTimeout();
    void onRequestTimeout(const QString &msgId);

private:
    void setState(ConnectionState newState);
    void startHeartbeat();
    void stopHeartbeat();
    void scheduleReconnect();
    void cancelReconnect();
    void resetReconnectAttempts();
    void sendAuth();
    void processJsonLine(const QByteArray &line);
    void processReceivedMessage(const QJsonObject &message);
    void cleanupPendingRequests();
    static QJsonObject sanitizeForHistory(const QJsonObject &message);
    int nextReconnectDelayMs() const;

    struct PendingRequest {
        ResponseCallback callback;
        QTimer *timer;
    };

    QTcpSocket *m_socket              = nullptr;
    QTimer *m_heartbeatTimer          = nullptr;
    QTimer *m_reconnectTimer          = nullptr;
    QByteArray m_readBuffer;
    QMap<QString, PendingRequest> m_pendingRequests;
    ConnectionState m_state           = ConnectionState::Disconnected;
    ConnectionConfig m_config;
    QString m_sessionToken;
    QStringList m_roles;
    QStringList m_permissions;
    int m_heartbeatSec                = 30;
    int m_reconnectAttempts           = 0;
    qint64 m_binaryExpectedBytes      = 0;
    bool m_cleaningPendingRequests    = false;
    int m_authGeneration              = 0;
};

#endif // TCPCONNECTIONMANAGER_H
