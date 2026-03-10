#ifndef HEARTBEATMANAGER_H
#define HEARTBEATMANAGER_H

#include <QObject>
#include <QTimer>
#include <qtcpsocket.h>

class Heartbeatmanager : public QObject
{
    Q_OBJECT

public:
    explicit Heartbeatmanager(QObject* parent = nullptr);

    void setSocket(QTcpSocket *socket);

    //启动/停止心跳
    void statr(int intervalMs = 3000);
    void stop();
    bool isRunning() const;

signals:
    void heartbeattimeout(); //心跳超时，需要重连
    void sendHeartbeat(const QByteArray &data);

private slots:
    void onTimeout();

private:
    QTcpSocket* m_socket;
    QTimer* m_timer;
    QTimer* m_timeroutTimer;//检测相应超时
    bool m_waitingResponse;

};

#endif // HEARTBEATMANAGER_H
