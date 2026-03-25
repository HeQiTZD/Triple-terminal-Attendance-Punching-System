#include "heartbeatmanager.h"
#include <qobject.h>

Heartbeatmanager::Heartbeatmanager(QObject *parent)
    :QObject(parent),
    m_socket(nullptr),
    m_timer(new QTimer(this)),
    m_timeroutTimer(new QTimer(this)),
    m_waitingResponse(false)
{
    //心跳发送定时器
    connect(m_timer,&QTimer::timeout,this,&Heartbeatmanager::onTimeout);

    //超时检测定时器(单次触发)
    m_timeroutTimer->setSingleShot(true);
    connect(m_timeroutTimer,&QTimer::timeout,[=](){

        m_waitingResponse = false;
        emit heartbeattimeout();
        qDebug()<<"心跳超时，触发重连";
    });
}

void Heartbeatmanager::setSocket(QTcpSocket *socket)
{
    m_socket = socket;
    if(socket){
        connect(socket,&QTcpSocket::connected,[=](){
            //socket连接成功，自启动心跳
            if(!isRunning()){
                start();//使用默认间隔
            }
        });

        connect(socket,&QTcpSocket::disconnected,[=](){
            //socket断开，停止心跳
            stop();
        });
    }
}

void Heartbeatmanager::start(int intervalMs)
{
    if(!m_socket){
        qWarning()<<"未设置m_socket";
        return;
    }

    //如果已运行，先停止
    if(m_timer->isActive()){
        stop();
    }

    m_waitingResponse = false;
    m_timer->setInterval(intervalMs);
    m_timer->start();

    //立即发送第一次心跳
    onTimeout();
}

void Heartbeatmanager::stop()
{
    m_timer->stop();
    m_timeroutTimer->stop();
    m_waitingResponse = false;
}

bool Heartbeatmanager::isRunning() const
{
    return m_timer->isActive();
}


//辅助函数：构建心跳数据包
QByteArray Heartbeatmanager::buildHeartbeatData()
{
    //使用协议层创建标准心跳消息
    QJsonObject emptyData;
    QJsonObject message = Protocol::createMessage(Protocol::HEARTBEAT,emptyData);

    //转为QByteArray
    QByteArray data = QJsonDocument(message).toJson(QJsonDocument::Compact);
    return data;
}

void Heartbeatmanager::onTimeout()
{
    if(!m_socket || m_socket->state() != QAbstractSocket::ConnectedState){
        //socket 未连接，不发送
        return;
    }

    if(m_waitingResponse){
        //上一次心跳还未收到响应，视为超时
        qWarning()<<"Heartbeatmanager: 心跳超时，上一次心跳未收到响应";
        emit heartbeattimeout();
        m_waitingResponse = false;
        return;
    }

    //构建心跳数据
    QByteArray heartbeatData = buildHeartbeatData();

    //标记为等待响应状态
    m_waitingResponse = true;

    //启动超时检测（5秒）
    m_timeroutTimer->start(5000);

    //通过信号发送出数据，由外部发送
    emit sendHeartbeat(heartbeatData);
}

void Heartbeatmanager::onHeartbeatResponse()
{
    //收到心跳响应，重置等待状态
    m_waitingResponse = false;
    m_timeroutTimer->stop();
}
