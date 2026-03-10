#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class Connectionmanager : public QObject
{
    Q_OBJECT

public:
    Connectionmanager(QObject *parent = nullptr);

    //连接控制
    bool connectToHost(const QString &ip,quint16 port);// 保存IP/端口，建立TCP连接，启动重连机制
    void disconnect();//断开连接，停止重连定时器
    bool isConnect();//是否连接

    //获取socket用于数据传输
    QTcpSocket* socket() const;

signals:
    void connected();//已连接信号
    void disconnected();//断开连接信号
    void errorOccurred(const QString &errorString);//发生错误时，发送错误信号并发送错误信息
    void stateChanged(bool isConnected);//连接状态发送改变时，发送当前连接状态

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onReconnectTimeout();

private:
    QTcpSocket *m_socket;//TCP套接字
    QTimer *m_reconnectTimer;//重连的定时器
    QString m_ip;//服务器ip
    quint16 m_port;//服务器端口
    int m_reconnectCount;//当前重连次数
    static const int MAX_RECONNECT = 5;//最大重连次数
};

#endif // CONNECTIONMANAGER_H
