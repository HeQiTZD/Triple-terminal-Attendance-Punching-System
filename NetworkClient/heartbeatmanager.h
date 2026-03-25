#ifndef HEARTBEATMANAGER_H
#define HEARTBEATMANAGER_H

#include <QObject>
#include <QTimer>
#include <qtcpsocket.h>
#include "protocol.h"
#include <QJsonDocument>

class Heartbeatmanager : public QObject
{
    Q_OBJECT

public:
    explicit Heartbeatmanager(QObject* parent = nullptr);

    void setSocket(QTcpSocket *socket);

    //启动/停止心跳
    void start(int intervalMs = 3000);
    void stop();
    bool isRunning() const;//状态查询
    QByteArray buildHeartbeatData();
    
    //收到心跳响应后调用，重置等待状态
    void onHeartbeatResponse();

signals:
    void heartbeattimeout(); //心跳超时，需要重连
    void sendHeartbeat(const QByteArray &data);

private slots:
    void onTimeout();

private:
    QTcpSocket* m_socket;
    QTimer* m_timer;//心跳发送定时器
    QTimer* m_timeroutTimer;//超时检测定时器
    bool m_waitingResponse;//标记是否正在等待响应

};

#endif // HEARTBEATMANAGER_H
