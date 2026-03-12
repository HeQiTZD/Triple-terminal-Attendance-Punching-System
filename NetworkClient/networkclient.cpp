#include "networkclient.h"

Networkclient *Networkclient::instance()
{
    static Networkclient* s_instance = nullptr;
    if(!s_instance){
        s_instance = new Networkclient();
    }
    return s_instance;
}

bool Networkclient::connectToServer(const QString &ip, quint16 port)
{
    return m_connection->connectToHost(ip,port);
}

void Networkclient::disconnect()
{
    return m_connection->disconnect();
}

bool Networkclient::isConnected() const
{
    return m_connection->isConnect();
}

void Networkclient::onConnectionConnected()
{
    qDebug()<<"Networkclient:连接成功，初始化模块";

    //获取socket并创建Writer/ready
    QTcpSocket *socket = m_connection->socket();
    m_writer = new Messagewriter(socket);
    m_ready = new Messagereader(socket);

    //连接ready信号
    connect(m_ready,&Messagereader::messageReceived,this,&Networkclient::onMessageReceived);
    //连接writer信号
    connect(m_writer,&Messagewriter::messageSent,this,&Networkclient::onSendError);

    //启动并设置heartbeat
    m_heartbeat->setSocket(socket);
    connect(m_heartbeat,&Heartbeatmanager::heartbeattimeout,this,&Networkclient::onHeartbeatTimeout);
    m_heartbeat->start(3000);

    //启动消息接收
    m_ready->start();

    //处理断网期间缓存消息
    processQueue();

    //更新状态并发射对外信号
    m_isOnline = true;
    emit connected();
    emit networkStateChanged(true);

}

void Networkclient::onConnectionDisconnected()
{

}

void Networkclient::onConnectionStateChanged(bool isOnline)
{

}

Networkclient::Networkclient(QObject *parent): QObject(parent)
    ,m_connection(new Connectionmanager(this))
    ,m_heartbeat(new Heartbeatmanager(this))
    ,m_writer(nullptr)
    ,m_ready(nullptr)
    ,m_queue(new Messagequeue(this))
    ,m_isOnline(false)
{
    setupConnections();
    qDebug()<<"Networkclient初始化完成";
}

void Networkclient::setupConnections()
{
   //ConnectionMessage信号->私有槽处理
    connect(m_connection,&Connectionmanager::connected,this,&Networkclient::onConnectionConnected);
   connect(m_connection,&Connectionmanager::disconnected,this,&Networkclient::onConnectionDisconnected);
    connect(m_connection,&Connectionmanager::stateChanged,this,&Networkclient::onConnectionStateChanged);
}


