#include "heartbeatmanager.h"
#include "serverprotocol.h"

#include <QJsonDocument>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

Heartbeatmanager::Heartbeatmanager(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_timeoutTimer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &Heartbeatmanager::onTimeout);

    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this]() {
        m_waitingResponse = false;
        qDebug() << "Heartbeatmanager: 心跳超时，触发重连";
        emit heartbeattimeout();
    });
}

// ---------------------------------------------------------------------------
// Socket
// ---------------------------------------------------------------------------

void Heartbeatmanager::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
    if (socket) {
        connect(socket, &QTcpSocket::disconnected, this, [this]() {
            stop();
        });
    }
}

// ---------------------------------------------------------------------------
// start / stop
// ---------------------------------------------------------------------------

void Heartbeatmanager::start(int heartbeatSec)
{
    if (!m_socket) {
        qWarning() << "Heartbeatmanager: 未设置 socket";
        return;
    }

    if (m_timer->isActive())
        stop();

    m_heartbeatSec = qMax(heartbeatSec, 5);            // 最低 5 s
    m_timeoutThresholdMs = m_heartbeatSec * kGraceMultiplier * 1000;

    m_waitingResponse = false;
    m_timer->start(m_heartbeatSec * 1000);

    // 立即发送第一次心跳
    onTimeout();

    qDebug() << "Heartbeatmanager: 已启动, 间隔" << m_heartbeatSec
             << "s, 超时阈值" << m_timeoutThresholdMs << "ms";
}

void Heartbeatmanager::stop()
{
    m_timer->stop();
    m_timeoutTimer->stop();
    m_waitingResponse = false;

    qDebug() << "Heartbeatmanager: 已停止";
}

bool Heartbeatmanager::isRunning() const
{
    return m_timer->isActive();
}

// ---------------------------------------------------------------------------
// Dynamic interval
// ---------------------------------------------------------------------------

void Heartbeatmanager::setHeartbeatInterval(int heartbeatSec)
{
    if (heartbeatSec < 5)
        heartbeatSec = 5; // 最低 5 s

    if (heartbeatSec == m_heartbeatSec)
        return;

    m_heartbeatSec = heartbeatSec;
    m_timeoutThresholdMs = m_heartbeatSec * kGraceMultiplier * 1000;

    // 重启定时器以使用新间隔
    if (m_timer->isActive()) {
        m_timer->start(m_heartbeatSec * 1000);
    }

    qDebug() << "Heartbeatmanager: 间隔更新为" << m_heartbeatSec
             << "s, 超时阈值" << m_timeoutThresholdMs << "ms";
}

// ---------------------------------------------------------------------------
// Activity / response
// ---------------------------------------------------------------------------

void Heartbeatmanager::onAnyMessage()
{
    // 收到任意消息 → 连接存活，重置超时计时器
    if (m_timeoutTimer->isActive())
        m_timeoutTimer->start(m_timeoutThresholdMs);
}

void Heartbeatmanager::onHeartbeatResponse()
{
    m_waitingResponse = false;
    m_timeoutTimer->stop();
}

// ---------------------------------------------------------------------------
// Build
// ---------------------------------------------------------------------------

QByteArray Heartbeatmanager::buildHeartbeatData()
{
    QJsonObject message = ServerProtocol::buildHeartbeat();
    return QJsonDocument(message).toJson(QJsonDocument::Compact);
}

// ---------------------------------------------------------------------------
// Timer callback
// ---------------------------------------------------------------------------

void Heartbeatmanager::onTimeout()
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    if (m_waitingResponse) {
        qWarning() << "Heartbeatmanager: 上一次心跳未收到响应";
        emit heartbeattimeout();
        m_waitingResponse = false;
        return;
    }

    const QByteArray heartbeatData = buildHeartbeatData();
    m_waitingResponse = true;
    m_timeoutTimer->start(m_timeoutThresholdMs);

    emit sendHeartbeat(heartbeatData);
}
