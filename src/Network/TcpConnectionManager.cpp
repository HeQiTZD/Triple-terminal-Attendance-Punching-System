#include "TcpConnectionManager.h"
#include <QJsonDocument>
#include <QUuid>
#include <QDateTime>
#include <QtEndian>
#include <algorithm>

static const int kMaxJsonLineSize = 1 * 1024 * 1024;   // 1 MB
static const int kMaxBinaryPayload = 16 * 1024 * 1024;  // 16 MB
static const int kMaxReconnectDelayMs = 30000;

TcpConnectionManager::TcpConnectionManager(QObject *parent)
    : QObject(parent)
{
    m_socket = new QTcpSocket(this);

    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setSingleShot(false);
    connect(m_heartbeatTimer, &QTimer::timeout,
            this, &TcpConnectionManager::onHeartbeatTimeout);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout,
            this, &TcpConnectionManager::onReconnectTimeout);

    connect(m_socket, &QTcpSocket::connected,
            this, &TcpConnectionManager::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &TcpConnectionManager::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::readyRead,
            this, &TcpConnectionManager::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred,
            this, &TcpConnectionManager::onSocketErrorOccurred);
}

TcpConnectionManager::~TcpConnectionManager()
{
    cleanupPendingRequests();
    disconnectFromServer();
}

// ── public ──

TcpConnectionManager::ConnectionState TcpConnectionManager::connectionState() const { return m_state; }

bool TcpConnectionManager::isAuthenticated() const { return m_state == ConnectionState::Authenticated; }

QString TcpConnectionManager::sessionToken() const { return m_sessionToken; }

QStringList TcpConnectionManager::roles() const { return m_roles; }

QStringList TcpConnectionManager::permissions() const { return m_permissions; }

const TcpConnectionManager::ConnectionConfig &TcpConnectionManager::config() const { return m_config; }

void TcpConnectionManager::connectToServer(const ConnectionConfig &config)
{
    if (m_state == ConnectionState::Connecting)
        return;

    // 如果已连接，先断开
    if (m_state != ConnectionState::Disconnected) {
        disconnectFromServer();
    }

    m_config = config;
    m_sessionToken.clear();
    m_roles.clear();
    m_permissions.clear();
    m_readBuffer.clear();
    m_binaryExpectedBytes = 0;

    cancelReconnect();
    cleanupPendingRequests();

    setState(ConnectionState::Connecting);
    emit errorOccurred(QStringLiteral("Connecting to %1:%2...").arg(config.host).arg(config.port));
    m_socket->connectToHost(config.host, config.port);
}

void TcpConnectionManager::disconnectFromServer()
{
    stopHeartbeat();
    cancelReconnect();
    resetReconnectAttempts();

    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
    }

    m_sessionToken.clear();
    m_roles.clear();
    m_permissions.clear();
    m_readBuffer.clear();
    m_binaryExpectedBytes = 0;
    cleanupPendingRequests();

    if (m_state != ConnectionState::Disconnected) {
        setState(ConnectionState::Disconnected);
    }
}

QString TcpConnectionManager::sendMessage(const QJsonObject &message, ResponseCallback callback)
{
    QJsonObject msg = message;

    // 自动填充信封字段
    if (!msg.contains(::Protocol::kRole))
        msg[::Protocol::kRole] = ::Protocol::kRoleAdmin;
    if (!msg.contains(::Protocol::kFrom))
        msg[::Protocol::kFrom] = m_config.clientId;
    if (!msg.contains(::Protocol::kMsgId))
        msg[::Protocol::kMsgId] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    if (!msg.contains(::Protocol::kTs))
        msg[::Protocol::kTs] = QDateTime::currentMSecsSinceEpoch();

    QString msgId = msg[::Protocol::kMsgId].toString();

    if (callback) {
        auto *timer = new QTimer(this);
        timer->setSingleShot(true);

        PendingRequest req;
        req.callback = std::move(callback);
        req.timer = timer;
        m_pendingRequests.insert(msgId, req);

        connect(timer, &QTimer::timeout, this, [this, msgId]() {
            onRequestTimeout(msgId);
        });
        timer->start(m_config.requestTimeoutMs);
    }

    emit jsonMessageSent(sanitizeForHistory(msg));

    QJsonDocument doc(msg);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');
    m_socket->write(data);

    return msgId;
}

void TcpConnectionManager::sendBinaryFrame(const QByteArray &payload)
{
    if (payload.size() > kMaxBinaryPayload) {
        emit errorOccurred(QStringLiteral("Binary payload too large: %1 bytes").arg(payload.size()));
        return;
    }
    qint32 len = static_cast<qint32>(payload.size());
    qint32 lenBE = qToBigEndian(len);
    QByteArray frame;
    frame.append(reinterpret_cast<const char *>(&lenBE), sizeof(lenBE));
    frame.append(payload);
    m_socket->write(frame);
}

// ── private slots ──

void TcpConnectionManager::onSocketConnected()
{
    setState(ConnectionState::Connected);
    sendAuth();
}

void TcpConnectionManager::onSocketDisconnected()
{
    stopHeartbeat();
    m_readBuffer.clear();
    m_binaryExpectedBytes = 0;

    if (m_state >= ConnectionState::Connected) {
        setState(ConnectionState::Disconnected);
    }

    // 如果之前已认证过，尝试自动重连
    if (!m_sessionToken.isEmpty()) {
        scheduleReconnect();
    }
}

void TcpConnectionManager::onSocketReadyRead()
{
    m_readBuffer.append(m_socket->readAll());

    // 二进制帧接收模式
    if (m_binaryExpectedBytes > 0) {
        if (static_cast<qint64>(m_readBuffer.size()) >= m_binaryExpectedBytes) {
            QByteArray payload = m_readBuffer.left(m_binaryExpectedBytes);
            m_readBuffer.remove(0, m_binaryExpectedBytes);
            m_binaryExpectedBytes = 0;
            emit binaryFrameReceived(payload);
        }
        return;
    }

    // JSON 行分帧解析
    while (true) {
        // 检查是否遇到二进制帧头（下一个JSON消息可能带payloadLength）
        int newlineIdx = m_readBuffer.indexOf('\n');
        if (newlineIdx < 0) {
            // 缓冲区过大保护
            if (m_readBuffer.size() > kMaxJsonLineSize) {
                emit errorOccurred(QStringLiteral("JSON line exceeds max size, disconnecting"));
                disconnectFromServer();
            }
            break;
        }

        QByteArray line = m_readBuffer.left(newlineIdx);
        m_readBuffer.remove(0, newlineIdx + 1);

        if (line.isEmpty())
            continue;

        processJsonLine(line);

        // 如果刚处理的消息头指示有二进制帧跟随，退出循环等待数据
        if (m_binaryExpectedBytes > 0)
            break;
    }

    // 处理完JSON行后，检查缓冲区是否满足二进制帧
    if (m_binaryExpectedBytes > 0
        && static_cast<qint64>(m_readBuffer.size()) >= m_binaryExpectedBytes) {
        QByteArray payload = m_readBuffer.left(m_binaryExpectedBytes);
        m_readBuffer.remove(0, m_binaryExpectedBytes);
        m_binaryExpectedBytes = 0;
        emit binaryFrameReceived(payload);
    }
}

void TcpConnectionManager::onSocketErrorOccurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    emit errorOccurred(m_socket->errorString());

    if (m_state == ConnectionState::Connecting) {
        setState(ConnectionState::Disconnected);
        if (!m_sessionToken.isEmpty()) {
            scheduleReconnect();
        }
    }
}

void TcpConnectionManager::onHeartbeatTimeout()
{
    QJsonObject hb;
    hb[::Protocol::kType] = ::Protocol::kHeartbeat;
    sendMessage(hb, [this](const QJsonObject &response) {
        if (response.isEmpty()) {
            // 心跳超时——视为断线
            emit errorOccurred(QStringLiteral("Heartbeat timeout, reconnecting..."));
            m_socket->disconnectFromHost();
            return;
        }
        int code = response[::Protocol::kCode].toInt(-1);
        if (code == ::Protocol::ErrorCode::kSuccess) {
            emit heartbeatAcknowledged();
        }
    });
}

void TcpConnectionManager::onReconnectTimeout()
{
    if (m_state != ConnectionState::Disconnected)
        return;

    m_reconnectAttempts++;
    emit errorOccurred(QStringLiteral("Reconnect attempt %1/%2")
                           .arg(m_reconnectAttempts)
                           .arg(m_config.reconnectMaxAttempts));

    setState(ConnectionState::Connecting);
    m_socket->connectToHost(m_config.host, m_config.port);
}

void TcpConnectionManager::onRequestTimeout(const QString &msgId)
{
    auto it = m_pendingRequests.find(msgId);
    if (it == m_pendingRequests.end())
        return;

    ResponseCallback cb = std::move(it.value().callback);
    delete it.value().timer;
    m_pendingRequests.erase(it);

    if (cb) {
        cb(QJsonObject{}); // 空对象表示超时
    }
}

// ── private helpers ──

void TcpConnectionManager::setState(ConnectionState newState)
{
    if (m_state == newState)
        return;
    auto oldState = m_state;
    m_state = newState;

    bool wasAuth = (oldState == ConnectionState::Authenticated);
    bool isAuth  = (newState == ConnectionState::Authenticated);

    emit stateChanged(oldState, newState);
    if (wasAuth != isAuth)
        emit authenticatedChanged();
    if (newState == ConnectionState::Disconnected) {
        m_sessionToken.clear();
        m_roles.clear();
        m_permissions.clear();
        emit sessionTokenChanged();
        emit rolesChanged();
        emit permissionsChanged();
    }
}

void TcpConnectionManager::startHeartbeat()
{
    // 以 heartbeatSec 的 80% 为间隔，留出余量
    int intervalMs = static_cast<int>(m_heartbeatSec * 800);
    m_heartbeatTimer->start(intervalMs);
}

void TcpConnectionManager::stopHeartbeat()
{
    m_heartbeatTimer->stop();
}

void TcpConnectionManager::scheduleReconnect()
{
    if (m_config.reconnectMaxAttempts <= 0)
        return;

    int delay = nextReconnectDelayMs();
    m_reconnectTimer->start(delay);
}

void TcpConnectionManager::cancelReconnect()
{
    m_reconnectTimer->stop();
}

void TcpConnectionManager::resetReconnectAttempts()
{
    m_reconnectAttempts = 0;
}

void TcpConnectionManager::sendAuth()
{
    QJsonObject auth;
    auth[::Protocol::kType] = ::Protocol::kAuth;
    auth[::Protocol::kData] = QJsonObject{
        { ::Protocol::kUsername, m_config.username },
        { ::Protocol::kPassword, m_config.password }
    };

    sendMessage(auth, [this](const QJsonObject &response) {
        if (response.isEmpty()) {
            emit errorOccurred(QStringLiteral("Auth request timed out"));
            disconnectFromServer();
            return;
        }

        QString type = response[::Protocol::kType].toString();
        if (type == ::Protocol::kAuthResponse) {
            int code = response[::Protocol::kCode].toInt(-1);
            if (code == ::Protocol::ErrorCode::kSuccess) {
                QJsonObject data = response[::Protocol::kData].toObject();
                m_sessionToken = data[::Protocol::kSessionToken].toString();
                m_heartbeatSec = data[::Protocol::kHeartbeatSec].toInt(30);

                QJsonArray rolesArr = data[::Protocol::kRoles].toArray();
                m_roles.clear();
                for (const auto &v : rolesArr)
                    m_roles.append(v.toString());

                QJsonArray permsArr = data[::Protocol::kPermissions].toArray();
                m_permissions.clear();
                for (const auto &v : permsArr)
                    m_permissions.append(v.toString());

                emit sessionTokenChanged();
                emit rolesChanged();
                emit permissionsChanged();

                setState(ConnectionState::Authenticated);
                resetReconnectAttempts();
                startHeartbeat();

                emit authenticated(m_sessionToken, m_roles, m_permissions);
            } else {
                QString msg = response[::Protocol::kMsg].toString();
                emit authFailed(code, msg);
                disconnectFromServer();
            }
        } else if (type == ::Protocol::kError) {
            int code = response[::Protocol::kCode].toInt(-1);
            QString msg = response[::Protocol::kMsg].toString();
            emit authFailed(code, msg);
            disconnectFromServer();
        }
    });
}

void TcpConnectionManager::processJsonLine(const QByteArray &line)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(line, &err);
    if (err.error != QJsonParseError::NoError) {
        emit errorOccurred(QStringLiteral("JSON parse error: %1").arg(err.errorString()));
        return;
    }
    if (!doc.isObject()) {
        emit errorOccurred(QStringLiteral("Received non-object JSON"));
        return;
    }

    QJsonObject msg = doc.object();
    processReceivedMessage(msg);

    // 检查是否有二进制帧跟随
    if (msg.contains(::Protocol::kPayloadLength)) {
        bool ok = false;
        qint64 len = msg[::Protocol::kPayloadLength].toVariant().toLongLong(&ok);
        if (ok && len > 0 && len <= kMaxBinaryPayload) {
            m_binaryExpectedBytes = len + 4; // 4字节大端长度前缀
        }
    }
}

QJsonObject TcpConnectionManager::sanitizeForHistory(const QJsonObject &message)
{
    QJsonObject copy = message;
    if (copy.contains(::Protocol::kData) && copy[::Protocol::kData].isObject()) {
        QJsonObject data = copy[::Protocol::kData].toObject();
        if (data.contains(::Protocol::kPassword)) {
            data[::Protocol::kPassword] = QStringLiteral("***");
            copy[::Protocol::kData] = data;
        }
    }
    return copy;
}

void TcpConnectionManager::processReceivedMessage(const QJsonObject &message)
{
    emit jsonMessageReceived(sanitizeForHistory(message));

    // 匹配待处理的请求
    QString inReplyTo = message[::Protocol::kInReplyTo].toString();
    if (!inReplyTo.isEmpty()) {
        auto it = m_pendingRequests.find(inReplyTo);
        if (it != m_pendingRequests.end()) {
            ResponseCallback cb = std::move(it.value().callback);
            delete it.value().timer;
            m_pendingRequests.erase(it);
            if (cb) {
                cb(message);
            }
            // inReplyTo 消息可能是 push 之外的响应，push 则继续往下
            // 对于纯响应消息（非 push），处理完回调即可
        }
    }

    // 分发到通用消息信号（push、未匹配响应等）
    emit messageReceived(message);
}

void TcpConnectionManager::cleanupPendingRequests()
{
    if (m_cleaningPendingRequests || m_pendingRequests.isEmpty())
        return;

    m_cleaningPendingRequests = true;

    QList<PendingRequest> pending;
    pending.reserve(m_pendingRequests.size());
    for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); ++it) {
        pending.append(std::move(it.value()));
    }
    m_pendingRequests.clear();

    for (PendingRequest &req : pending) {
        if (req.timer) {
            req.timer->stop();
            delete req.timer;
            req.timer = nullptr;
        }
    }

    m_cleaningPendingRequests = false;

    const QJsonObject timeoutResponse;
    for (PendingRequest &req : pending) {
        if (req.callback)
            req.callback(timeoutResponse);
    }
}

int TcpConnectionManager::nextReconnectDelayMs() const
{
    int delay = m_config.reconnectBaseDelayMs * (1 << std::min(m_reconnectAttempts, 10));
    return std::min(delay, kMaxReconnectDelayMs);
}
