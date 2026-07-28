#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QVector>
#include "serverprotocol.h"

#include "connectionmanager.h"
#include "heartbeatmanager.h"
#include "messagewriter.h"
#include "messagereader.h"
#include "messagequeue.h"
#include "authenticator.h"
#include "outboxmanager.h"
#include <QDebug>

#include "../Auth/tokenmanager.h"
#include "../Auth/tokenrefresher.h"

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    static NetworkClient* instance();

    // 连接管理
    bool connectToServer(const QString &ip, quint16 port);
    void disconnect();
    bool isConnected() const;

    // 认证（委托给 Authenticator）
    bool isAuthenticated() const;
    Authenticator* authenticator() const { return m_authenticator; }

    // 业务接口
    bool syncPersonData();

    /// 考勤上报 — 写入 outbox 后异步发送，返回 client_msg_id（幂等键）
    QString uploadAttendance(const QString& employeeId, const QString& status,
                             const QDateTime& checkTime = QDateTime::currentDateTime());

    /// 带照片的考勤上报
    QString uploadAttendanceWithPhoto(const QString& employeeId, const QString& status,
                                      const QByteArray& photoJpeg,
                                      const QDateTime& checkTime = QDateTime::currentDateTime());

    bool uploadAttendanceBatch(const QVector<QJsonObject> &records);
    void reportDeviceStatus(const QJsonObject &status);

    /// 公开 outbox 处理（AttendanceReporter 触发重试）
    void retryOutbox();

    /// 发送任意 JSON 消息（SyncManager/CommandHandler 使用）
    bool sendJson(const QJsonObject &message);

    // 设备身份
    void setDeviceId(const QString& deviceId);
    QString deviceId() const { return m_deviceId; }
    void setDeviceKey(const QString& deviceKey);
    QString deviceKey() const { return m_deviceKey; }
    QString sessionToken() const;

    void setDeviceName(const QString& name) { m_deviceName = name; }
    QString deviceName() const { return m_deviceName; }

    // JWT 令牌
    TokenManager* tokenManager() const { return m_tokenManager; }

    // Outbox 管理
    OutboxManager* outboxManager() const { return m_outboxManager; }

signals:
    // 连接状态
    void connected();
    void disconnected();
    void networkStateChanged(bool isOnline);

    // 认证（转发 Authenticator 信号）
    void authSuccess();
    void authFailed(int code, const QString &message);
    void devicePendingAuth();

    // 业务数据
    void personDataReceived(const QVector<ServerProtocol::PersonData> &persons);
    void uploadFinished(bool success, const QString &message);
    void faceSyncItemReceived(const QJsonObject &header, const QByteArray &payload);

    /// 单条考勤上报结果（含 employeeId）
    void attendanceReportResult(const QString &employeeId, bool success, const QString &message);

    /// 接收到远程指令
    void deviceCommandReceived(const QJsonObject &message);

    // 同步流信号（路由到 SyncManager）
    void personSyncReceived(const QJsonObject &message);
    void faceSyncBeginReceived(const QJsonObject &message);
    void faceSyncEndReceived(const QJsonObject &message);

    // JWT 令牌刷新响应
    void tokenRefreshResponse(const QJsonObject &message);

public slots:
    void scheduleReconnect();

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
    explicit NetworkClient(QObject *parent = nullptr);
    void setupConnections();
    void processQueue();
    void loadDeviceConfig();

    void handlePersonSynResponse(const QJsonObject &message);
    void handleTokenRefreshResponse(const QJsonObject &message);
    void sendDeviceStatusReport();

    // 添加令牌到消息
    QJsonObject addTokenToMessage(const QJsonObject &message);

private:
    ConnectionManager *m_connection;
    HeartbeatManager   *m_heartbeat;
    MessageWriter      *m_writer;
    MessageReader      *m_ready;
    MessageQueue       *m_queue;

    // JWT 令牌管理
    TokenManager    *m_tokenManager;
    TokenRefresher  *m_tokenRefresher;

    // 新增：认证和 Outbox 管理
    Authenticator   *m_authenticator;
    OutboxManager   *m_outboxManager;

    QString m_deviceId    = QStringLiteral("device_001");
    QString m_deviceKey;
    QString m_deviceName;
    QString m_fwVersion   = QStringLiteral("1.0.0");

    bool m_isOnline        = false;
};

#endif // NETWORKCLIENT_H
