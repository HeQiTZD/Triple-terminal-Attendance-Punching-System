#include "heartbeatmanager.h"
#include "serverprotocol.h"

#include <QJsonDocument>

Heartbeatmanager::Heartbeatmanager(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_timeoutTimer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &Heartbeatmanager::onTimeout);

    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this]() {
        m_waitingResponse = false;
        emit heartbeattimeout();
    });
}

void Heartbeatmanager::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
    if (socket) {
        connect(socket, &QTcpSocket::disconnected, this, [this]() {
            stop();
        });
    }
}

void Heartbeatmanager::start(int heartbeatSec)
{
    if (!m_socket) {
        return;
    }

    if (m_timer->isActive())
        stop();

    m_heartbeatSec = qMax(heartbeatSec, 5);
    m_timeoutThresholdMs = m_heartbeatSec * kGraceMultiplier * 1000;

    m_waitingResponse = false;
    m_timer->start(m_heartbeatSec * 1000);

    onTimeout();
}

void Heartbeatmanager::stop()
{
    m_timer->stop();
    m_timeoutTimer->stop();
    m_waitingResponse = false;
}

bool Heartbeatmanager::isRunning() const
{
    return m_timer->isActive();
}

void Heartbeatmanager::setHeartbeatInterval(int heartbeatSec)
{
    if (heartbeatSec < 5)
        heartbeatSec = 5;

    if (heartbeatSec == m_heartbeatSec)
        return;

    m_heartbeatSec = heartbeatSec;
    m_timeoutThresholdMs = m_heartbeatSec * kGraceMultiplier * 1000;

    if (m_timer->isActive()) {
        m_timer->start(m_heartbeatSec * 1000);
    }
}

void Heartbeatmanager::onAnyMessage()
{
    if (m_timeoutTimer->isActive())
        m_timeoutTimer->start(m_timeoutThresholdMs);
}

void Heartbeatmanager::onHeartbeatResponse()
{
    m_waitingResponse = false;
    m_timeoutTimer->stop();
}

QByteArray Heartbeatmanager::buildHeartbeatData()
{
    QJsonObject message = ServerProtocol::buildHeartbeat();
    return QJsonDocument(message).toJson(QJsonDocument::Compact);
}

void Heartbeatmanager::onTimeout()
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    if (m_waitingResponse) {
        emit heartbeattimeout();
        m_waitingResponse = false;
        return;
    }

    const QByteArray heartbeatData = buildHeartbeatData();
    m_waitingResponse = true;
    m_timeoutTimer->start(m_timeoutThresholdMs);

    emit sendHeartbeat(heartbeatData);
}
