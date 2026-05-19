#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include "serverprotocol.h"

#include "connectionmanager.h"
#include "heartbeatmanager.h"
#include "messagewriter.h"
#include "messagereader.h"
#include "messagequeue.h"
#include <QDebug>

#include "../LocalStorage/localstorage.h"
#include "../FaceRecognition/facedatabasemanager.h"

class Networkclient : public QObject
{
    Q_OBJECT

public:
    static Networkclient* instance();

    // 连接管理
    bool connectToServer(const QString &ip, quint16 port);
    void disconnect();
    bool isConnected() const;
    bool isAuthenticated() const { return m_isAuthenticated; }

    // 业务接口
    bool syncPersonData();
    bool uploadAttendance(const QString& employeeId, const QString& status,
                          const QDateTime& checkTime = QDateTime::currentDateTime());
    bool uploadAttendanceBatch(const QVector<QJsonObject> &records);
    void reportDeviceStatus(const QJsonObject &status);

    // 设备身份
    void setDeviceId(const QString& deviceId);
    QString deviceId() const { return m_deviceId; }
    void setDeviceKey(const QString& deviceKey);
    QString deviceKey() const { return m_deviceKey; }
    QString sessionToken() const { return m_sessionToken; }

    void setDeviceName(const QString& name) { m_deviceName = name; }
    QString deviceName() const { return m_deviceName; }

signals:
    // 连接状态
    void connected();
    void disconnected();
    void networkStateChanged(bool isOnline);

    // 认证
    void authSuccess();
    void authFailed(int code, const QString &message);

    // 业务数据
    void personDataReceived(const QVector<ServerProtocol::PersonData> &persons);
    void uploadFinished(bool success, const QString &message);
    void faceSyncItemReceived(const QJsonObject &header, const QByteArray &payload);

private slots:
    void onConnectionConnected();
    void onConnectionDisconnected();
    void onConnectionStateChanged(bool isOnline);

    void onMessageReceived(const QJsonObject &message);
    void onBinaryFrameReceived(const QJsonObject &header, const QByteArray &payload);
    void onHeartbeatTimeout();
    void onSendError();
    void onSendHeartbeat(const QByteArray &data);

private:
    explicit Networkclient(QObject *parent = nullptr);
    void setupConnections();
    void processQueue();
    void loadDeviceConfig();

    void handleAuthResponse(const QJsonObject &message);
    void handlePersonSynResponse(const QJsonObject &message);
    void handleUploadResponse(const QJsonObject &message);
    void handleServerError(const QJsonObject& message);
    void sendDeviceStatusReport();

private:
    Connectionmanager *m_connection;
    Heartbeatmanager   *m_heartbeat;
    Messagewriter      *m_writer;
    Messagereader      *m_ready;
    Messagequeue       *m_queue;

    QString m_deviceId    = QStringLiteral("device_001");
    QString m_deviceKey;
    QString m_sessionToken;
    QString m_deviceName;
    QString m_fwVersion   = QStringLiteral("1.0.0");

    bool m_isAuthenticated = false;
    bool m_isOnline        = false;
};

#endif // NETWORKCLIENT_H
