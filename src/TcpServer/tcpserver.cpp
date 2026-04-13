#include "tcpserver.h"
#include <qabstractsocket.h>
#include <qhostaddress.h>
#include <qjsondocument.h>
#include <qlogging.h>
#include <qtcpserver.h>
#include <qtcpsocket.h>
#include <qtimer.h>

TcpServer::TcpServer(QObject *parent):QObject(parent),m_server(new QTcpServer(this)),m_heartbeatChecker(new QTimer(this)) {
    connect(m_server,&QTcpServer::newConnection,this,&TcpServer::onNewConnection);

    //心跳检测定时器（每30秒检查一次）
    m_heartbeatChecker->setInterval(30000);
    connect(m_heartbeatChecker,&QTimer::timeout,this,&TcpServer::onHeartbeatTimeout);
}

TcpServer::~TcpServer()
{
   
}

bool TcpServer::startServer(quint16 port)
{
    //isListening()。如果为true:服务器已成功调用 listen()，正在指定端口监听客户端连接请求
     if(m_server->isListening()){
        return true;
    }

    /*
        启动服务器监听
        • QHostAddress::Any：监听本机所有网络接口（IPv4/IPv6），确保外部设备可访问
        • port：自定义监听端口（需确保未被系统占用）
    */
    if(!m_server->listen(QHostAddress::Any,port)){
        emit errorOccurred(m_server->errorString());
        return false;
    }

    m_heartbeatChecker->start();
    emit isRunningChanged();
    qDebug()<<"TCP Server started on port:" << port;
    return true;
}

void TcpServer::stopServer()
{
    //停止心跳
    m_heartbeatChecker->stop();

    //遍历客户端容器，socket->close()强制关闭每个客户端的 Socket 连接
    for(const auto &clients:m_clients){
        clients.socket->close();
    }

    //清空容器
    m_clients.clear();
    m_deviceMap.clear();

    m_server->close();
    emit isRunningChanged();
    emit clientCountChanged();
}

bool TcpServer::isRunning() const
{
    return m_server->isListening();
}

int TcpServer::clientCount() const
{
    return m_clients.size();
}

//根据设备 ID 向指定 TCP 客户端发送数据
bool TcpServer::sendToClient(const QString &deviceId, const QJsonObject &data)
{
    //检查 m_deviceMap 这个键值对容器中，是否存在目标 deviceId（设备唯一标识）
    if(!m_deviceMap.contains(deviceId)){
        return false;
    }

    //从容器中取出该设备对应的 QTcpSocket 连接句柄（用于 TCP 通信）
    QTcpSocket *socket = m_deviceMap[deviceId];
    sendMessage(socket,data);
    return true;
}

void TcpServer::brodcastsToAll(const QJsonObject &data)
{
    //遍历客户端容器，将消息广播给所有客户端
    for(const auto &clients:m_clients){
        sendMessage(clients.socket,data);
    }
}

void TcpServer::onNewConnection()
{
    while(m_server->hasPendingConnections()){
        QTcpSocket* socket = m_server->nextPendingConnection();

        ClientInfo info;
        info.socket = socket;
        info.ipAddress = socket->peerAddress().toString();
        info.isAuthenticated = false;

        //创建客户端心跳定时器
        info.heartbeatTimer = new QTimer(this);
        info.heartbeatTimer->setSingleShot(true);
        info.heartbeatTimer->setInterval(60000);

        //加入客户端容器
        m_clients.insert(socket,info);

        //监听该客户端的连接状态和信息传输
        connect(socket,&QTcpSocket::disconnected,this,&TcpServer::onSocketDisconnected);
        connect(socket,&QTcpSocket::readyRead,this,&TcpServer::onSocketReadyRead);

        emit clientCountChanged();
        qDebug()<<"New client connected from:" << info.ipAddress;
    }
}

void TcpServer::onSocketDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());//sender() 获取触发当前槽函数的信号发送者
    if(socket){
        removeClient(socket);
    }
}

void TcpServer::onSocketReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());//使用qobject_cast<>将发送对象安全转换为QTcpSocket
    //检查套接字对象是否有效，并在客户端列表中
    if(!socket || !m_clients.contains(socket)){
        return;
    }

    ClientInfo &info = m_clients[socket];//获取socket对应的客户端结构体引用
    info.buffer+=socket->readAll(); //将新接收的所有数据追加到客户端的缓冲区中buffer(QByteArray)

    while(true){
        const int newlineIndex = info.buffer.indexOf('\n');//用indexof()找到“\n”的位置
        if(newlineIndex < 0) break;//如何为不存在，则返回

        const QByteArray line = info.buffer.left(newlineIndex).trimmed();//使用left()截取\n前的子串，然后使用trimmed()移除字符串两端的空白字符（空格、制表符、换行符等）
        info.buffer.remove(0,newlineIndex+1);//移除已处理部分

        if(line.isEmpty()) continue;//为空，则跳过，进行下一次处理

        QJsonParseError parseErr;
        const QJsonDocument doc = QJsonDocument::fromJson(line,&parseErr);//解析数据，并捕获错误
        if(doc.isNull() || !doc.isObject()){
            // 解析失败：可选择忽略，或给客户端回 error
            qDebug() << "Invalid JSON line:" << parseErr.errorString();
            continue;
        }

        processMessage(socket, doc.object());
    }
}

void TcpServer::onHeartbeatTimeout()
{
    //检查所有客户端的心跳
    QDateTime now = QDateTime::currentDateTime();
    QList<QTcpSocket*> toRemove;

    for(const auto &client:m_clients){
        if(!client.heartbeatTimer->isActive()){
            // 心跳超时，断开连接
            toRemove.append(client.socket);
        }
    }

    for(QTcpSocket *socket:toRemove){
        qDebug() << "Client heartbeat timeout, disconnecting";
        socket->close();
    }
}

void TcpServer::processMessage(QTcpSocket *socket, const QJsonObject &message)
{
    QString type = message.value("type").toString();
    ClientInfo &info = m_clients[socket];

    if(type == "auth"){
        //设备认证
        info.deviceId = message.value("deviceId").toString();
        info.isAuthenticated = true;
        m_deviceMap.insert(info.deviceId,socket);
        updateHeartbeat(socket);

        emit clientConnected(info.deviceId,info.ipAddress);

        //发送认证成功响应
        QJsonObject response;
        response["type"] = "auth_response";
        response["status"] = "success";
        sendMessage(socket,response);
    }else if(type == "heartbeat"){
        updateHeartbeat(socket);

        //发送心跳响应
        QJsonObject response;
        response["type"] = "heartbeat_response";
        sendMessage(socket,response);
    }else if(type == "attendance_record"){
        if(!info.isAuthenticated) return;
        updateHeartbeat(socket);

        emit attendanceRecordReceived(message);
    }else if(type == "device_status"){
        if(!info.isAuthenticated) return;
        updateHeartbeat(socket);
        emit deviceStatusReceived(info.deviceId,message);
    }
}

void TcpServer::sendMessage(QTcpSocket *socket, const QJsonObject &message)
{
    //QAbstractSocket::ConnectedState 是 Qt 套接字的标准状态枚举，表示TCP 连接已建立，可正常收发数据
    if(!socket || socket->state() != QAbstractSocket::ConnectedState){
        return;
    }

    /*
        核心原理
        JSON 序列化流程：QJsonObject（构造数据）→ QJsonDocument（封装文档）→ toJson()（序列化为字节流）→ QTcpSocket::write()（网络发送）。
        粘包处理：追加\n是轻量级方案，适合小数据量；大数据量建议使用长度前缀法（先发送 4 字节数据长度，再发送 JSON 内容），更稳定可靠。
    */
    QJsonDocument doc(message);
    /*
        doc.toJson(format: QJsonDocument::Compact)：
        把QJsonDocument转换为 UTF-8 编码的QByteArray（二进制字节数组）。
        QJsonDocument::Compact 表示紧凑格式：输出无缩进、无换行的最小化 JSON 字符串（如{"key":"value"}），适合网络传输以节省带宽。
        对应另一种格式 QJsonDocument::Indented：带缩进、换行的格式化 JSON，适合调试查看。
    */
    socket->write(doc.toJson(QJsonDocument::Compact)+"\n");
}

void TcpServer::removeClient(QTcpSocket *socket)
{
    if(!m_clients.contains(socket)){
        return;
    }

    // 从客户端容器中取出并移除该客户端信息
    ClientInfo info = m_clients.take(socket);//根据键 socket 找到对应的值，将其从容器中移除并返回

    if(!info.deviceId.isEmpty()){
        m_deviceMap.remove(info.deviceId);
        emit clientDisconnected(info.deviceId);
    }

    info.heartbeatTimer->stop();
    info.heartbeatTimer->deleteLater();

    socket->deleteLater();
    emit clientCountChanged();
}

void TcpServer::updateHeartbeat(QTcpSocket *socket)
{
    if(m_clients.contains(socket)){
        m_clients[socket].heartbeatTimer->stop();
        m_clients[socket].heartbeatTimer->start();
    }
}
