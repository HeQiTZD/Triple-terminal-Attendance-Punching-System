#include "messagewriter.h"

#include <QJsonDocument>

MessageWriter::MessageWriter(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
{
}

static bool socketReady(QTcpSocket *s, MessageWriter *w)
{
    if (!s) {
        emit w->sendError(QStringLiteral("socket未初始化"));
        return false;
    }
    if (s->state() != QAbstractSocket::ConnectedState) {
        emit w->sendError(QStringLiteral("socket未连接"));
        return false;
    }
    return true;
}

bool MessageWriter::send(const QJsonObject &message)
{
    if (!socketReady(m_socket, this))
        return false;

    QByteArray data = QJsonDocument(message).toJson(QJsonDocument::Compact);
    data.append('\n');

    const qint64 written = m_socket->write(data);
    if (written == -1) {
        emit sendError(QStringLiteral("写入失败：") + m_socket->errorString());
        return false;
    }

    m_socket->flush();
    emit messageSent(message);
    return true;
}

bool MessageWriter::send(const QByteArray &data)
{
    if (!socketReady(m_socket, this))
        return false;

    QByteArray frame = data;
    if (!frame.endsWith('\n'))
        frame.append('\n');

    const qint64 written = m_socket->write(frame);
    if (written == -1) {
        emit sendError(QStringLiteral("写入失败：") + m_socket->errorString());
        return false;
    }

    m_socket->flush();
    return true;
}

bool MessageWriter::sendBinaryFrame(const QByteArray &payload)
{
    if (!socketReady(m_socket, this))
        return false;

    const quint32 len = static_cast<quint32>(payload.size());

    QByteArray frame;
    frame.reserve(4 + payload.size());
    frame.append(static_cast<char>((len >> 24) & 0xFF));
    frame.append(static_cast<char>((len >> 16) & 0xFF));
    frame.append(static_cast<char>((len >> 8)  & 0xFF));
    frame.append(static_cast<char>( len        & 0xFF));
    frame.append(payload);

    const qint64 written = m_socket->write(frame);
    if (written == -1) {
        emit sendError(QStringLiteral("二进制帧写入失败：") + m_socket->errorString());
        return false;
    }

    m_socket->flush();
    return true;
}

bool MessageWriter::sendRawBytes(const QByteArray &data)
{
    if (!socketReady(m_socket, this))
        return false;

    const qint64 written = m_socket->write(data);
    if (written == -1) {
        emit sendError(QStringLiteral("原始字节写入失败：") + m_socket->errorString());
        return false;
    }

    m_socket->flush();
    return true;
}

int MessageWriter::sendBatch(const QVector<QJsonObject> &messages)
{
    int successCount = 0;
    for (const QJsonObject &msg : messages) {
        if (send(msg))
            ++successCount;
        else
            break;
    }
    return successCount;
}

bool MessageWriter::sendRawJson(const QJsonObject &msg)
{
    if (!socketReady(m_socket, this))
        return false;

    const QByteArray data = QJsonDocument(msg).toJson(QJsonDocument::Compact);

    const qint64 written = m_socket->write(data);
    if (written == -1) {
        emit sendError(QStringLiteral("写入失败：") + m_socket->errorString());
        return false;
    }

    m_socket->flush();
    return true;
}
