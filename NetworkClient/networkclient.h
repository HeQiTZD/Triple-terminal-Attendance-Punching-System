#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include "protocol.h"
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

    //连接管理
    bool connectToServer(const QString &ip,quint16 port);
    void disconnect();
    bool isConnected() const;

    //业务接口
    bool syncPersonData();//同步人员数据
    bool uploadAttendance(const QString& employeeId, const QString& status, const QDateTime& checkTime = QDateTime::currentDateTime());
    bool uploadAttendanceBatch(const QVector<QJsonObject> &records); // already-built attendance_record messages
    void reportDeviceStatus(const QJsonObject &status);

    void setDeviceId(const QString& deviceId);
    QString deviceId() const { return m_deviceId; }


signals:
    //连接状态
    void connected();
    void disconnected();
    void networkStateChanged(bool isOnline);

    //业务数据
    void personDataReceived(const QVector<Protocol::PersonData> &persons);
    void uploadFinished(bool success,const QString &message);

private slots:
    //内部处理槽 - 接收子模块信号，处理逻辑后发送新信号
    void onConnectionConnected();//处理连接成功
    void onConnectionDisconnected();//处理断开连接
    void onConnectionStateChanged(bool isOnline);//处理连接状态变化

    void onMessageReceived(const QJsonObject &message);//处理接收消息
    void onHeartbeatTimeout();//处理心跳超时
    void onSendError();//处理发送错误
    void onSendHeartbeat(const QByteArray &data);//处理心跳发送

private:
    explicit Networkclient(QObject *parent = nullptr);
    void setupConnections();//连接各模块信号槽
    void processQueue();//处理队列中的消息

    //辅助方法
    void handlePersonSynResponse(const QJsonObject &message);
    void handleUploadResponse(const QJsonObject &message);
    void handleServerError(const QJsonObject& message);

private:
    Connectionmanager *m_connection;
    Heartbeatmanager *m_heartbeat;
    Messagewriter *m_writer;
    Messagereader *m_ready;
    Messagequeue *m_queue;

    QString m_deviceId = "device_001";
    bool m_isAuthenticated = false;
    bool m_isOnline = false;//当前网络状态
};
#endif // NETWORKCLIENT_H
