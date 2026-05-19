#ifndef HEARTBEATMANAGER_H
#define HEARTBEATMANAGER_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>

class Heartbeatmanager : public QObject
{
    Q_OBJECT

public:
    explicit Heartbeatmanager(QObject *parent = nullptr);

    void setSocket(QTcpSocket *socket);

    /// 启动心跳（heartbeatSec 从 auth_response 获取，默认 30 s）
    void start(int heartbeatSec = 30);
    void stop();
    bool isRunning() const;

    /// 动态更新心跳间隔（auth 成功后由 Networkclient 调用）
    void setHeartbeatInterval(int heartbeatSec);

    /// 收到任意消息时调用 —— 重置超时计时器
    void onAnyMessage();

    /// 收到心跳响应时调用
    void onHeartbeatResponse();

    QByteArray buildHeartbeatData();

signals:
    void heartbeattimeout();
    void sendHeartbeat(const QByteArray &data);

private slots:
    void onTimeout();

private:
    QTcpSocket *m_socket    = nullptr;
    QTimer     *m_timer     = nullptr;  // 心跳发送定时器（周期性）
    QTimer     *m_timeoutTimer = nullptr; // 超时检测定时器（单次）
    bool        m_waitingResponse = false;

    int m_heartbeatSec      = 30;       // 心跳间隔（秒）
    int m_timeoutThresholdMs = 90000;   // 超时阈值 = heartbeatSec × graceMultiplier

    static constexpr int kGraceMultiplier = 3;
};

#endif // HEARTBEATMANAGER_H
