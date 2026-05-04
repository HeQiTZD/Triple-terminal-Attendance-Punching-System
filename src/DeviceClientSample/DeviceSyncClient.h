#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QJsonObject>

// Minimal reference client-side receiver for:
// - JSON control messages split by '\n'
// - Binary frames: 4-byte big-endian length prefix + payload
//
// It demonstrates how to parse:
// - person_sync (JSON only)
// - face_sync_begin / face_sync_item_header / face_sync_end (JSON + binary frame)
//
// You should integrate the handlers into your actual attendance device app and
// replace the "apply..." methods with real local DB writes.
class DeviceSyncClient : public QObject
{
    Q_OBJECT
public:
    explicit DeviceSyncClient(QObject* parent = nullptr);

    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();

signals:
    void logLine(const QString& line);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError err);

private:
    enum class RxState {
        ReadJsonLine,
        ReadBinaryLen,
        ReadBinaryPayload,
    };

    void processJson(const QJsonObject& obj);
    void applyPersonSync(const QJsonObject& obj);
    void applyFaceItem(const QString& employeeId, const QByteArray& featureVector);

    QTcpSocket m_socket;
    QByteArray m_buffer;
    RxState m_state = RxState::ReadJsonLine;

    // face_sync_item_header context
    QString m_pendingEmployeeId;
    quint32 m_pendingPayloadLen = 0;
};

