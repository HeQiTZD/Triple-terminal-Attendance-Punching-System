#include "messagereader.h"

#include <QJsonDocument>
#include <QJsonParseError>

MessageReader::MessageReader(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
{
}

void MessageReader::start()
{
    if (!m_socket) {
        return;
    }

    connect(m_socket, &QTcpSocket::readyRead, this, &MessageReader::onReadyRead);
}

void MessageReader::stop()
{
    disconnect(m_socket, &QTcpSocket::readyRead, this, &MessageReader::onReadyRead);

    m_buffer.clear();
    m_mode = Mode::Line;
    m_pendingBinaryHeader = QJsonObject();
}

void MessageReader::onReadyRead()
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray newData = m_socket->readAll();
    m_buffer.append(newData);

    if (m_buffer.size() > kMaxBufferSize) {
        emit parseError(QStringLiteral("接收缓冲区溢出"));
        m_buffer.clear();
        m_mode = Mode::Line;
        return;
    }

    while (true) {
        if (m_mode == Mode::Line) {
            QJsonObject message;
            if (!tryParseLine(&message))
                break;

            const QString type = message.value(QStringLiteral("type")).toString();
            if (type == QLatin1StringView("face.sync.item.header")) {
                m_pendingBinaryHeader = message;
                m_mode = Mode::Binary;
            } else {
                emit messageReceived(message);
            }
        } else {
            if (!tryParseBinaryFrame())
                break;
        }
    }
}

bool MessageReader::tryParseLine(QJsonObject *outMessage)
{
    const int nl = m_buffer.indexOf('\n');
    if (nl == -1)
        return false;

    QByteArray line = m_buffer.left(nl);
    m_buffer.remove(0, nl + 1);

    if (line.isEmpty())
        return tryParseLine(outMessage);

    QJsonParseError parseErr;
    QJsonDocument doc = QJsonDocument::fromJson(line, &parseErr);

    if (parseErr.error != QJsonParseError::NoError) {
        const QString errStr = QStringLiteral("JSON解析失败：%1 (位置 %2)")
                                   .arg(parseErr.errorString())
                                   .arg(parseErr.offset);
        emit parseError(errStr);
        return false;
    }

    if (!doc.isObject()) {
        return false;
    }

    *outMessage = doc.object();
    return true;
}

bool MessageReader::tryParseBinaryFrame()
{
    if (m_buffer.size() < kLengthPrefixLen)
        return false;

    const auto *raw = reinterpret_cast<const quint8 *>(m_buffer.constData());
    const quint32 payloadLen = (static_cast<quint32>(raw[0]) << 24)
                             | (static_cast<quint32>(raw[1]) << 16)
                             | (static_cast<quint32>(raw[2]) << 8)
                             | (static_cast<quint32>(raw[3]));

    if (payloadLen > static_cast<quint32>(kMaxBufferSize)) {
        emit parseError(QStringLiteral("二进制载荷超出上限"));
        m_buffer.clear();
        m_mode = Mode::Line;
        return false;
    }

    const int totalNeeded = kLengthPrefixLen + static_cast<int>(payloadLen);
    if (m_buffer.size() < totalNeeded)
        return false;

    const QByteArray payload = m_buffer.mid(kLengthPrefixLen, static_cast<int>(payloadLen));
    m_buffer.remove(0, totalNeeded);

    emit binaryFrameReceived(m_pendingBinaryHeader, payload);

    m_pendingBinaryHeader = QJsonObject();
    m_mode = Mode::Line;
    return true;
}
