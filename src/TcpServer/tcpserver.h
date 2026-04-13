#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QHash>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>

class TcpServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(int clientCount READ clientCount NOTIFY clientCountChanged)

public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

    //启动TCP服务，默认端口8080
    bool startServer(quint16 pot = 8080);
    //停止TCP服务
    void stopServer();

    //获取服务状态
    bool isRunning() const;
    //在线客户端数量
    int clientCount() const;

    //向指定客户端发送数据
    bool sendToClient(const QString &deviceId,const QJsonObject &data);
    //广播给所有客户端
    void brodcastsToAll(const QJsonObject &data);

signals:
    //服务状态变化
    void isRunningChanged();
    //客户端数量变化
    void clientCountChanged();

    //客户端连接
    void clientConnected(const QString &deviceId,const QString &ipAddress);
    //客户端断开
    void clientDisconnected(const QString &deviceId);
    //收到考勤记录
    void attendanceRecordReceived(const QJsonObject &record);
    //设备状态上报
    void deviceStatusReceived(const QString &deviceId,const QJsonObject &status);
    //错误信号
    void errorOccurred(const QString &errorString);

private slots:
    // 有新客户端连接
    void onNewConnection();
    // 客户端断开连接
    void onSocketDisconnected();
    // 收到客户端数据
    void onSocketReadyRead();
    // 心跳超时（客户端掉线）
    void onHeartbeatTimeout();

private:
    //客户端信息结构体
    struct ClientInfo{
        QTcpSocket* socket;// 客户端套接字
        QString deviceId;// 设备唯一ID（如考勤机编号）
        QString ipAddress; // 客户端IP
        QTimer* heartbeatTimer;// 心跳定时器
        bool isAuthenticated;// 是否认证通过
        QByteArray buffer; // 用于按行分帧（json + '\n'）
    };

    QTcpServer *m_server;// TCP服务端对象
    QHash<QTcpSocket*,ClientInfo> m_clients;// 套接字 => 客户端信息
    QHash<QString,QTcpSocket*> m_deviceMap;// 设备ID => 套接字（快速查找）
    QTimer *m_heartbeatChecker;// 全局心跳检测定时器

    //解析客户端发来的 JSON 数据
    void processMessage(QTcpSocket *socket,const QJsonObject  &message);
    //给指定客户端发 JSON 数据
    void sendMessage(QTcpSocket *socket,const QJsonObject &message);
    //移除断开的客户端
    void removeClient(QTcpSocket *socket);
    //更新心跳时间（客户端活着就刷新）
    void updateHeartbeat(QTcpSocket *socket);
};

#endif // TCPSERVER_H
