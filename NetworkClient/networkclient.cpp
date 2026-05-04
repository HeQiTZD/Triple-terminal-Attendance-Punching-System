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

void Networkclient::setDeviceId(const QString &deviceId)
{
    if (deviceId.isEmpty()) return;
    m_deviceId = deviceId;
}

//同步人员数据
bool Networkclient::syncPersonData()
{
    if(!isConnected()){
        qWarning()<<"Networkclient：未连接，无法同步人员数据";
        return false;
    }

    if(!m_writer){
        qWarning()<<"Networkclient：writer未初始化，无法同步人员数据";
        return false;
    }

    // AttendanceServer expects: {"type":"sync_request","deviceId":"..."}
    const QJsonObject msg = ServerProtocol::buildSyncRequest(m_deviceId);
    return m_writer->send(msg);
}

//上传打卡记录
bool Networkclient::uploadAttendance(const QString& employeeId, const QString& status, const QDateTime& checkTime)
{
    // AttendanceServer expects:
    // {"type":"attendance_record","employeeId":"...","checkTime":"ISO","deviceId":"...","status":"ok"}
    QJsonObject message = ServerProtocol::buildAttendanceRecord(employeeId, checkTime, m_deviceId, status);

    if(isConnected() && m_writer)
    {
        //网络正常，直接发送
        bool ok = m_writer->send(message);
        if(!ok){
            //发送失败，入队缓存
            m_queue->enqueue(message);
            qDebug()<<"发送失败，已缓存";
        }
        return ok;
    }else{
        //断网，进入缓存
        qDebug()<<"断网，打卡记录已缓存，队列大小"<<m_queue->size();
        m_queue->enqueue(message);
        return true;//业务层视为成功
    }
}

//批量上传
bool Networkclient::uploadAttendanceBatch(const QVector<QJsonObject> &records)
{
    if(!m_queue){
        qWarning()<<"Networkclient：queue未初始化";
        return false;
    }

    if(!isConnected() || !m_writer){
        //网络断开，全部入队
        for(const auto &msg : records){
            m_queue->enqueue(msg);
        }
        return true;
    }

    //网络正常，批量发送
    int sent = m_writer->sendBatch(records);

    //未发送的入队
    for(int i=sent;i<records.size();i++){
        m_queue->enqueue(records[i]);
    }

    return sent == records.size();
}

//上报设备状态
void Networkclient::reportDeviceStatus(const QJsonObject &status)
{
    // `status` carries optional fields like deviceName/ipAddress; we must still set
    // type/deviceId/status at root level for AttendanceServer.
    QJsonObject message = ServerProtocol::buildDeviceStatus(m_deviceId, status.value("status").toString("online"), status);

    if(isConnected() && m_writer){
        m_writer->send(message);
    }else{
        m_queue->enqueue(message);
    }
}

//连接成功处理
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

    //先启动消息接收，确保能收到服务器响应
    m_ready->start();

    // 必须先认证，否则 AttendanceServer 不会处理多数业务消息
    m_isAuthenticated = false;
    m_writer->send(ServerProtocol::buildAuth(m_deviceId));

    //再启动心跳（确保reader已就绪，能接收心跳响应）
    m_heartbeat->setSocket(socket);
    connect(m_heartbeat,&Heartbeatmanager::heartbeattimeout,this,&Networkclient::onHeartbeatTimeout);
    m_heartbeat->start(3000);

    //处理断网期间缓存消息
    processQueue();

    //更新状态并发射对外信号
    m_isOnline = true;
    emit connected();
    emit networkStateChanged(true);

}

//连接断开处理
void Networkclient::onConnectionDisconnected()
{
    qDebug()<<"Networkclient:连接断开，清理资源";

    //停止心跳
    m_heartbeat->stop();

    //停止消息接收并断开信号
    if(m_ready){
        m_ready->stop();
        //断开m_ready与当前对象的所有信号连接
        m_ready->disconnect(this);
    }
    
    //断开writer信号
    if(m_writer){
        m_writer->disconnect(this);
    }
    
    //断开心跳信号（保留与ConnectionManager的连接，但断开与当前对象的连接）
    FaceDatabaseManager::disconnect(m_heartbeat, &Heartbeatmanager::heartbeattimeout, this, &Networkclient::onHeartbeatTimeout);

    //清理writer/ready(下次连接重新创建)
    delete m_writer;
    m_writer = nullptr;

    delete m_ready;
    m_ready = nullptr;

    m_isAuthenticated = false;
    m_isOnline = false;
    emit disconnected();
    emit networkStateChanged(false);
}

//状态变化处理
void Networkclient::onConnectionStateChanged(bool isOnline)
{
    //状态未改变则忽略
    if(m_isOnline == isOnline){
        return;
    }

    m_isOnline = isOnline;

    //网络恢复时处理队列
    if(isOnline){
        processQueue();
    }

    //对外发出信号
    emit networkStateChanged(isOnline);
}

//消息接收处理
void Networkclient::onMessageReceived(const QJsonObject &message)
{
    const auto t = ServerProtocol::parseType(message);
    switch (t) {
    case ServerProtocol::MessageType::AuthResponse: {
        const QString st = message.value(ServerProtocol::kStatus).toString();
        if (st == "success") {
            m_isAuthenticated = true;
            qDebug() << "Networkclient: auth success, deviceId=" << m_deviceId;
            // Optional: trigger initial sync right after auth
            syncPersonData();
        } else {
            m_isAuthenticated = false;
            qWarning() << "Networkclient: auth failed" << message;
        }
        break;
    }
    case ServerProtocol::MessageType::HeartbeatResponse:
        qDebug()<<"Networkclient:收到服务器心跳响应";
        m_heartbeat->onHeartbeatResponse();
        break;
    case ServerProtocol::MessageType::PersonSync:
        handlePersonSynResponse(message);
        break;
    case ServerProtocol::MessageType::Error:
        handleServerError(message);
        break;
    default:
        qWarning() << "Networkclient：未知服务器消息" << message.value(ServerProtocol::kType).toString();
        break;
    }
}

//心跳超时处理
void Networkclient::onHeartbeatTimeout()
{
    qWarning()<<"Networkclient：心跳超时需要重连";

    //断开连接，触发重联机制
    m_connection->disconnect();
    //ConnectionManager会自动重连
}

//发送错误处理
void Networkclient::onSendError()
{
    qWarning()<<"Networkclient：消息发送错误";
}

//心跳发送处理
void Networkclient::onSendHeartbeat(const QByteArray &data)
{
    if(m_writer){
        m_writer->send(data);
    }else{
        qWarning()<<"Networkclient：writer未初始化，无法发送心跳";
    }
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
    
    //连接心跳管理器的心跳发送信号
    connect(m_heartbeat,&Heartbeatmanager::sendHeartbeat,this,&Networkclient::onSendHeartbeat);
}

void Networkclient::processQueue()
{
    if(m_queue->isEmpty()){
        return;
    }

    if(!m_writer){
        qWarning()<<"Networkclient：writer未初始化，无法处理队列";
        return;
    }

    qDebug()<<"Networkclient：处理队列"<<m_queue->size()<<"条消息等待发送";

    //批量取出
    QVector<QJsonObject> message =m_queue->dequeueAll();

    //批量发送
    int sent = m_writer->sendBatch(message);

    //发送失败的重新入队
    if(sent<message.size()){
        for(int i=sent;i<message.size();++i){
            m_queue->enqueue(message[i]);
        }
        qWarning()<<"Networkclient：队列处理中断"<<(message.size()-sent)<<"条重新入队";
    }
}

void Networkclient::handlePersonSynResponse(const QJsonObject &message)
{
    // AttendanceServer sends: {"type":"person_sync","persons":[{id,name,employeeId,department,position},...]}
    const auto items = ServerProtocol::parsePersons(message);

    QVector<Protocol::PersonData> persons;
    persons.reserve(items.size());
    for (const auto& it : items) {
        Protocol::PersonData p;
        p.employeeId = it.employeeId;
        p.name = it.name;
        p.faceFeature.clear(); // not provided by server yet
        p.featureSize = 0;
        persons.push_back(p);
    }

    qDebug()<<"Networkclient：收到人员数据："<<persons.size()<<"条";

    //增加，调用LocalStorage进行同步
    bool synSuccess = LocalStorage::instance()->syncPersons(persons);

    if(synSuccess){
        //同步成功，重新加载内存特征库
        FaceDatabaseManager::instance()->loadFromDatabase();
        emit personDataReceived(persons);
    }else{
        //同步失败，记录错误，不发射成信号
        qWarning()<<"人员数据同步到本地数据库失败";
    }

    emit personDataReceived(persons);
}

//处理上传响应
void Networkclient::handleUploadResponse(const QJsonObject &message)
{
    bool success = message["success"].toBool();
    QString msg = message["message"].toString();

    //对外发射信号
    emit uploadFinished(success,msg);
}

void Networkclient::handleServerError(const QJsonObject &message)
{
    const QString msg = message.value(ServerProtocol::kMessage).toString();
    qWarning() << "Networkclient: server error:" << msg;
    emit uploadFinished(false, msg);
}


