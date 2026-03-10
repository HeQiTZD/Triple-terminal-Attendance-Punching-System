#include "connectionmanager.h"

Connectionmanager::Connectionmanager(QObject *parent)
    :QObject(parent),
     m_socket(new QTcpSocket(this)),
     m_reconnectTimer(new QTimer(this)),
     m_reconnectCount(0)
{
    //连接QTcpSocket信号到内部槽
    connect(m_socket,&QTcpSocket::connected,this,&Connectionmanager::onSocketConnected);
    connect(m_socket,&QTcpSocket::disconnected,this,&Connectionmanager::onSocketDisconnected);
    connect(m_socket,&QTcpSocket::errorOccurred,this,&Connectionmanager::onSocketError);
    //重连定时器信号
    connect(m_reconnectTimer,&QTimer::timeout,this,&Connectionmanager::onReconnectTimeout);

    //设置定时器为单次触发
    m_reconnectTimer->setSingleShot(true);
}

bool Connectionmanager::connectToHost(const QString &ip, quint16 port)
{
    m_ip = ip;
    m_port = port;
    m_reconnectCount = 0;

    //如果已连接。先断开
    if(m_socket->state() == QAbstractSocket::ConnectedState){
        m_socket->disconnectFromHost();
    }

    //发起连接
    m_socket->connectToHost(m_ip,m_port);

    //等待连接结果
    return m_socket->state() == QAbstractSocket::ConnectingState || m_socket->state() == QAbstractSocket::ConnectedState;
}

void Connectionmanager::disconnect()
{
    m_reconnectTimer->stop();
    m_reconnectCount = MAX_RECONNECT;//标记为主动断开
    m_socket->disconnectFromHost();
}

bool Connectionmanager::isConnect()
{
    return m_socket->state();
}

QTcpSocket *Connectionmanager::socket() const
{
    return m_socket;
}

void Connectionmanager::onSocketConnected()
{
    //连接成功，重置重连次数
    m_reconnectCount = 0;

    //通知上层
    emit connected();
    emit stateChanged(true);
}

void Connectionmanager::onSocketDisconnected()
{
    emit disconnected();
    emit stateChanged(false);

    //判断是否重连
    //条件：重连次数未达到最大次数且不是主动断开
    if(m_reconnectCount < MAX_RECONNECT){
        int delayMa = qMin(1000 * (1 << m_reconnectCount),30000);//最大30秒

        m_reconnectTimer->start(delayMa);
    }
}

void Connectionmanager::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorStr;
    switch(error){
    case QAbstractSocket::ConnectionRefusedError:
        errorStr = "连接被拒绝，服务器未启动";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorStr = "远程主机关闭连接";
            break;
    case QAbstractSocket::HostNotFoundError:
        errorStr = "主机未找到，请检查IP地址";
        break;
    case QAbstractSocket::SocketTimeoutError:
        errorStr = "连接超时";
        break;
    case QAbstractSocket::NetworkError:
        errorStr = "网络错误";
        break;
    default:
        errorStr = QString("未知错误：%1").arg(error);
    }

    emit this->errorOccurred(errorStr);
}

void Connectionmanager::onReconnectTimeout()
{
    m_reconnectCount++;
    m_socket->connectToHost(m_ip,m_port);
}
