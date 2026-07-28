#include "connectionmanager.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_reconnectTimer(new QTimer(this))
{
    connect(m_socket, &QTcpSocket::connected,       this, &ConnectionManager::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected,    this, &ConnectionManager::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred,   this, &ConnectionManager::onSocketError);

    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &ConnectionManager::onReconnectTimeout);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool ConnectionManager::connectToHost(const QString &ip, quint16 port)
{
    m_ip   = ip;
    m_port = port;
    m_reconnectCount   = 0;
    m_manualDisconnect = false;

    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();

    setState(ConnectionState::Connecting);
    m_socket->connectToHost(m_ip, m_port);

    return m_socket->state() == QAbstractSocket::ConnectingState
        || m_socket->state() == QAbstractSocket::ConnectedState;
}

void ConnectionManager::disconnect()
{
    m_manualDisconnect = true;
    m_reconnectTimer->stop();
    m_reconnectCount = kMaxReconnect;          // 阻止自动重连
    m_socket->disconnectFromHost();
    setState(ConnectionState::Disconnected);
}

void ConnectionManager::scheduleReconnect()
{
    m_manualDisconnect = false;
    m_reconnectCount   = 0;
    m_reconnectTimer->stop();
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    } else {
        // Socket already disconnected — start reconnect with minimal delay
        m_reconnectTimer->start(kBaseReconnectMs);
    }
}

bool ConnectionManager::isConnect() const
{
    return m_state == ConnectionState::Connected
        || m_state == ConnectionState::Authenticated;
}

ConnectionManager::ConnectionState ConnectionManager::state() const
{
    return m_state;
}

void ConnectionManager::setAuthenticated(bool authenticated)
{
    if (authenticated && m_state == ConnectionState::Connected) {
        setState(ConnectionState::Authenticated);
    } else if (!authenticated && m_state == ConnectionState::Authenticated) {
        setState(ConnectionState::Connected);
    }
}

QTcpSocket* ConnectionManager::socket() const
{
    return m_socket;
}

// ---------------------------------------------------------------------------
// State machine
// ---------------------------------------------------------------------------

void ConnectionManager::setState(ConnectionState newState)
{
    if (m_state == newState)
        return;

    const ConnectionState old = m_state;
    m_state = newState;

    emit connectionStateChanged(old, newState);

    // 向后兼容信号
    if (newState == ConnectionState::Connected || newState == ConnectionState::Authenticated) {
        if (old == ConnectionState::Connecting || old == ConnectionState::Disconnected)
            emit connected();
        emit stateChanged(true);
    } else if (newState == ConnectionState::Disconnected) {
        if (old != ConnectionState::Disconnected)
            emit disconnected();
        emit stateChanged(false);
    }
}

// ---------------------------------------------------------------------------
// Socket callbacks
// ---------------------------------------------------------------------------

void ConnectionManager::onSocketConnected()
{
    m_reconnectCount = 0;
    setState(ConnectionState::Connected);
}

void ConnectionManager::onSocketDisconnected()
{
    // 已经在 setAuthenticated 或 主动 disconnect 中设置过状态就不再重复
    if (m_state != ConnectionState::Disconnected)
        setState(ConnectionState::Disconnected);

    // 指数退避重连：1 s × 2^n，最大 30 s，最多 5 次
    if (!m_manualDisconnect && m_reconnectCount < kMaxReconnect) {
        const int delayMs = qMin(kBaseReconnectMs * (1 << m_reconnectCount), kMaxReconnectMs);
        m_reconnectTimer->start(delayMs);
    }
}

void ConnectionManager::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorStr;
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        errorStr = QStringLiteral("连接被拒绝，服务器未启动");     break;
    case QAbstractSocket::RemoteHostClosedError:
        errorStr = QStringLiteral("远程主机关闭连接");             break;
    case QAbstractSocket::HostNotFoundError:
        errorStr = QStringLiteral("主机未找到，请检查IP地址");      break;
    case QAbstractSocket::SocketTimeoutError:
        errorStr = QStringLiteral("连接超时");                     break;
    case QAbstractSocket::NetworkError:
        errorStr = QStringLiteral("网络错误");                     break;
    default:
        errorStr = QStringLiteral("未知错误：%1").arg(static_cast<int>(error));
    }

    emit errorOccurred(errorStr);
}

void ConnectionManager::onReconnectTimeout()
{
    m_reconnectCount++;
    setState(ConnectionState::Connecting);
    m_socket->connectToHost(m_ip, m_port);
}
