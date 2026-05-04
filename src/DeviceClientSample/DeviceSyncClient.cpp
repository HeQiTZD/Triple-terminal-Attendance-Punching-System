#include "DeviceSyncClient.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QDataStream>

DeviceSyncClient::DeviceSyncClient(QObject* parent) : QObject(parent)
{
    connect(&m_socket, &QTcpSocket::readyRead, this, &DeviceSyncClient::onReadyRead);
    connect(&m_socket, &QTcpSocket::connected, this, &DeviceSyncClient::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &DeviceSyncClient::onDisconnected);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &DeviceSyncClient::onError);
}

void DeviceSyncClient::connectToServer(const QString& host, quint16 port)
{
    m_socket.connectToHost(host, port);
}

void DeviceSyncClient::disconnectFromServer()
{
    m_socket.disconnectFromHost();
}

void DeviceSyncClient::onConnected()
{
    emit logLine(QStringLiteral("connected"));
}

void DeviceSyncClient::onDisconnected()
{
    emit logLine(QStringLiteral("disconnected"));
    m_buffer.clear();
    m_state = RxState::ReadJsonLine;
    m_pendingEmployeeId.clear();
    m_pendingPayloadLen = 0;
}

void DeviceSyncClient::onError(QAbstractSocket::SocketError)
{
    emit logLine(QStringLiteral("socket error: %1").arg(m_socket.errorString()));
}

void DeviceSyncClient::onReadyRead()
{
    m_buffer.append(m_socket.readAll());

    while (true) {
        if (m_state == RxState::ReadJsonLine) {
            const int idx = m_buffer.indexOf('\n');
            if (idx < 0) return;

            const QByteArray line = m_buffer.left(idx).trimmed();
            m_buffer.remove(0, idx + 1);
            if (line.isEmpty()) continue;

            QJsonParseError err;
            const QJsonDocument doc = QJsonDocument::fromJson(line, &err);
            if (doc.isNull() || !doc.isObject()) {
                emit logLine(QStringLiteral("invalid json: %1").arg(err.errorString()));
                continue;
            }
            processJson(doc.object());
            continue;
        }

        if (m_state == RxState::ReadBinaryLen) {
            if (m_buffer.size() < 4) return;
            QByteArray lenBytes = m_buffer.left(4);
            m_buffer.remove(0, 4);

            QDataStream ds(lenBytes);
            ds.setByteOrder(QDataStream::BigEndian);
            quint32 n = 0;
            ds >> n;
            m_pendingPayloadLen = n;
            m_state = RxState::ReadBinaryPayload;
            continue;
        }

        if (m_state == RxState::ReadBinaryPayload) {
            if (m_pendingPayloadLen == 0) {
                // Defensive: treat empty payload as done.
                m_state = RxState::ReadJsonLine;
                m_pendingEmployeeId.clear();
                continue;
            }
            if (static_cast<quint32>(m_buffer.size()) < m_pendingPayloadLen) return;

            const QByteArray payload = m_buffer.left(static_cast<int>(m_pendingPayloadLen));
            m_buffer.remove(0, static_cast<int>(m_pendingPayloadLen));

            applyFaceItem(m_pendingEmployeeId, payload);
            m_pendingEmployeeId.clear();
            m_pendingPayloadLen = 0;
            m_state = RxState::ReadJsonLine;
            continue;
        }
    }
}

void DeviceSyncClient::processJson(const QJsonObject& obj)
{
    const QString type = obj.value(QStringLiteral("type")).toString();

    if (type == QStringLiteral("person_sync")) {
        applyPersonSync(obj);
        return;
    }

    if (type == QStringLiteral("face_sync_begin")) {
        emit logLine(QStringLiteral("face sync begin"));
        return;
    }

    if (type == QStringLiteral("face_sync_item_header")) {
        const QString employeeId = obj.value(QStringLiteral("employeeId")).toString();
        const int payloadLen = obj.value(QStringLiteral("payloadLength")).toInt();

        if (employeeId.isEmpty() || payloadLen <= 0) {
            emit logLine(QStringLiteral("bad face header"));
            return;
        }

        m_pendingEmployeeId = employeeId;
        m_pendingPayloadLen = static_cast<quint32>(payloadLen);
        // Next bytes in the stream are: 4-byte length prefix + payload
        m_state = RxState::ReadBinaryLen;
        return;
    }

    if (type == QStringLiteral("face_sync_end")) {
        emit logLine(QStringLiteral("face sync end"));
        return;
    }

    // Other control messages ignored in this sample.
}

void DeviceSyncClient::applyPersonSync(const QJsonObject& obj)
{
    // Replace with: begin transaction -> truncate/merge -> upsert persons -> commit
    const auto persons = obj.value(QStringLiteral("persons")).toArray();
    emit logLine(QStringLiteral("person_sync count=%1").arg(persons.size()));
}

void DeviceSyncClient::applyFaceItem(const QString& employeeId, const QByteArray& featureVector)
{
    // Replace with: upsert into local face_data(employee_id, feature_vector, feature_size, updated_at)
    emit logLine(QStringLiteral("face item employeeId=%1 bytes=%2").arg(employeeId).arg(featureVector.size()));
}

