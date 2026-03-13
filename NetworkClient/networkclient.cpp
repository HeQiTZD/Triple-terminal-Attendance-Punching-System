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

//同步人员数据
bool Networkclient::syncPersonData()
{
    if(!isConnected()){
        qWarning()<<"Networkclient：未连接，无法同步人员数据";
        return false;
    }

    QJsonObject requesData;
    requesData["device_id"] = "device_001";

    //Qt::ISODate 是 Qt 框架中定义的 日期时间格式枚举值 ，用于将 QDateTime 转换为 ISO 8601 标准格式 的字符串。
    requesData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    return m_writer->send(requesData);
}

//上传打卡记录
bool Networkclient::uploadAttendance(const Protocol::AttendanceRecord &record)
{
    QJsonObject message = Protocol::createMessage(Protocol::UPLOAD_ATTENDANCE,record.toJson());

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
bool Networkclient::uploadAttendanceBatch(const QVector<Protocol::AttendanceRecord> &rocord)
{
    //构建消息列表
    QVector<QJsonObject> message;
    for(const auto &record : rocord){
        message.append(Protocol::createMessage(Protocol::UPLOAD_ATTENDANCE,record.toJson()));
    }

    if(isConnected() && m_writer)
    {
        //网络断开，全部入队
        for(const auto &msg : message){
            m_queue->enqueue(msg);
        }
        return true;
    }

    //网络正常，批量发送
    int sent = m_writer->sendBatch(message);

    //未发送的入队
    for(int i=sent;i<message.size();i++){
        m_queue->enqueue(message[i]);
    }

    return sent == message.size();
}

//上报设备状态
void Networkclient::reportDeviceStatus(const QJsonObject &status)
{
    QJsonObject message = Protocol::createMessage(Protocol::DEVICE_STATUS,status);

    if(isConnected() && m_writer){
        m_writer->send(message);
    }else{
        m_queue->enqueue(message);
    }
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

//连接断开处理
void Networkclient::onConnectionDisconnected()
{
    qDebug()<<"Networkclient:连接断开，清理资源";

    m_heartbeat->stop();

    if(m_ready){
        m_ready->stop();
    }

    //清理writer/ready(下次连接重新创建)
    delete m_writer;
    m_writer = nullptr;

    delete m_ready;
    m_ready = nullptr;

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
    //解析消息类型
    Protocol::MessageType type = Protocol::parseMessageType(message);

    switch (type) {
    case Protocol::SYNC_PERSON_RESPONSE:
        handlePersonSynResponse(message);
        break;
    case Protocol::UPLOAD_RESPONSE:
        handleUploadResponse(message);
        break;
    case Protocol::HEARTBEAT:
        qDebug()<<"Networkclient:收到服务器心跳";
        break;
    default:
        qWarning()<<"Networkclient：未知消息"<<Protocol::messageTypeToString(type);
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
    //解析人员数据
    QVector<Protocol::PersonData> persons;
    QJsonArray personArray = message["person"].toArray();//message["person"].toArray();获取person对应的值，QJsonValue类型
    //toArray()转化为QJsonArray（JSON数组）

    for(const auto &value:personArray){
        QJsonObject obj = value.toObject();

        //Protocol::PersonData::fromJson(obj) - 静态工厂方法
        //这是一个 自定义的静态成员函数 ，用于从 JSON 对象创建 PersonData 实例。
        persons.append(Protocol::PersonData::fromJson(obj));
    }

    qDebug()<<"Networkclient：收到人员数据："<<persons.size()<<"条";

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


