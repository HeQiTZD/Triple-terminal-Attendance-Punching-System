#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class Connectionmanager : public QObject
{
    Q_OBJECT

public:
    enum class ConnectionState {
        Disconnected,
        Connecting,
        Connected,
        Authenticated
    };
    Q_ENUM(ConnectionState)

    explicit Connectionmanager(QObject *parent = nullptr);

    // 连接控制
    bool connectToHost(const QString &ip, quint16 port);
    void disconnect();
    bool isConnect() const;
    ConnectionState state() const;

    // 由上层（Networkclient）在 auth 成功后调用
    void setAuthenticated(bool authenticated);

    // 获取 socket 用于数据传输
    QTcpSocket* socket() const;

signals:
    // 保留原信号 —— 向后兼容
    void connected();
    void disconnected();
    void stateChanged(bool isConnected);

    // 新信号 —— 完整状态变更
    void connectionStateChanged(ConnectionState oldState, ConnectionState newState);

    void errorOccurred(const QString &errorString);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onReconnectTimeout();

private:
    void setState(ConnectionState newState);

    QTcpSocket *m_socket;
    QTimer     *m_reconnectTimer;
    QString     m_ip;
    quint16     m_port = 0;
    int         m_reconnectCount = 0;
    bool        m_manualDisconnect = false;

    ConnectionState m_state = ConnectionState::Disconnected;

    static constexpr int kMaxReconnect    = 5;
    static constexpr int kMaxReconnectMs  = 30000;   // 30 s
    static constexpr int kBaseReconnectMs = 1000;    // 1 s
};

#endif // CONNECTIONMANAGER_H
