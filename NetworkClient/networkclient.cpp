#include "networkclient.h"
#include "../Config/configmanager.h"

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

Networkclient *Networkclient::instance()
{
    static Networkclient* s_instance = nullptr;
    if (!s_instance) {
        s_instance = new Networkclient();
    }
    return s_instance;
}

// ---------------------------------------------------------------------------
// Connection delegation
// ---------------------------------------------------------------------------

bool Networkclient::connectToServer(const QString &ip, quint16 port)
{
    return m_connection->connectToHost(ip, port);
}

void Networkclient::disconnect()
{
    m_connection->disconnect();
}

bool Networkclient::isConnected() const
{
    return m_connection->isConnect();
}

// ---------------------------------------------------------------------------
// Device identity
// ---------------------------------------------------------------------------

void Networkclient::setDeviceId(const QString &deviceId)
{
    if (deviceId.isEmpty()) return;
    m_deviceId = deviceId;
}

void Networkclient::setDeviceKey(const QString &deviceKey)
{
    m_deviceKey = deviceKey;
}

// ---------------------------------------------------------------------------
// Business — sync / attendance / status
// ---------------------------------------------------------------------------

bool Networkclient::syncPersonData()
{
    if (!isConnected()) {
        qWarning() << "Networkclient: 未连接，无法同步人员数据";
        return false;
    }
    if (!m_writer) {
        qWarning() << "Networkclient: writer未初始化，无法同步人员数据";
        return false;
    }

    const QJsonObject msg = ServerProtocol::buildSyncRequest(m_deviceId);
    return m_writer->send(msg);
}

bool Networkclient::uploadAttendance(const QString& employeeId,
                                     const QString& status,
                                     const QDateTime& checkTime)
{
    QJsonObject message = ServerProtocol::buildAttendanceReport(employeeId, checkTime, m_deviceId, status);

    if (isConnected() && m_writer) {
        bool ok = m_writer->send(message);
        if (!ok) {
            m_queue->enqueue(message);
            qDebug() << "发送失败，已缓存";
        }
        return ok;
    } else {
        qDebug() << "断网，打卡记录已缓存，队列大小" << m_queue->size();
        m_queue->enqueue(message);
        return true;
    }
}

bool Networkclient::uploadAttendanceBatch(const QVector<QJsonObject> &records)
{
    if (!m_queue) {
        qWarning() << "Networkclient: queue未初始化";
        return false;
    }

    if (!isConnected() || !m_writer) {
        for (const auto &msg : records)
            m_queue->enqueue(msg);
        return true;
    }

    int sent = m_writer->sendBatch(records);
    for (int i = sent; i < records.size(); i++)
        m_queue->enqueue(records[i]);

    return sent == records.size();
}

void Networkclient::reportDeviceStatus(const QJsonObject &status)
{
    QJsonObject message = ServerProtocol::buildDeviceStatusReport(
        m_deviceId,
        status.value(QStringLiteral("deviceName")).toString(m_deviceName),
        status.value(QStringLiteral("ipAddress")).toString(),
        status.value(QStringLiteral("fwVersion")).toString(m_fwVersion),
        status);

    if (isConnected() && m_writer) {
        m_writer->send(message);
    } else {
        m_queue->enqueue(message);
    }
}

// ---------------------------------------------------------------------------
// TCP connected — send auth, defer heartbeat
// ---------------------------------------------------------------------------

void Networkclient::onConnectionConnected()
{
    qDebug() << "Networkclient: TCP 已连接，发送认证";

    QTcpSocket *socket = m_connection->socket();
    m_writer = new Messagewriter(socket);
    m_ready  = new Messagereader(socket);

    connect(m_ready,  &Messagereader::messageReceived,      this, &Networkclient::onMessageReceived);
    connect(m_ready,  &Messagereader::binaryFrameReceived,   this, &Networkclient::onBinaryFrameReceived);
    connect(m_writer, &Messagewriter::messageSent,           this, &Networkclient::onSendError);

    m_ready->start();

    // 设置心跳 socket，但不启动 —— 等 auth 成功后再启动
    m_heartbeat->setSocket(socket);
    connect(m_heartbeat, &Heartbeatmanager::heartbeattimeout, this, &Networkclient::onHeartbeatTimeout);

    // 首包：auth
    m_isAuthenticated = false;
    loadDeviceConfig();
    m_writer->send(ServerProtocol::buildAuth(m_deviceId, m_deviceKey));

    m_isOnline = true;
    emit connected();
    emit networkStateChanged(true);
}

// ---------------------------------------------------------------------------
// TCP disconnected
// ---------------------------------------------------------------------------

void Networkclient::onConnectionDisconnected()
{
    qDebug() << "Networkclient: 连接断开，清理资源";

    m_heartbeat->stop();

    if (m_ready) {
        m_ready->stop();
        m_ready->disconnect(this);
    }
    if (m_writer) {
        m_writer->disconnect(this);
    }

    FaceDatabaseManager::disconnect(m_heartbeat, &Heartbeatmanager::heartbeattimeout,
                                    this, &Networkclient::onHeartbeatTimeout);

    delete m_writer;  m_writer = nullptr;
    delete m_ready;   m_ready  = nullptr;

    m_isAuthenticated = false;
    m_sessionToken.clear();
    m_isOnline = false;

    emit disconnected();
    emit networkStateChanged(false);
}

void Networkclient::onConnectionStateChanged(bool isOnline)
{
    if (m_isOnline == isOnline) return;
    m_isOnline = isOnline;
    if (isOnline) processQueue();
    emit networkStateChanged(isOnline);
}

// ---------------------------------------------------------------------------
// Message dispatch
// ---------------------------------------------------------------------------

void Networkclient::onMessageReceived(const QJsonObject &message)
{
    m_heartbeat->onAnyMessage();

    const auto t = ServerProtocol::parseType(message);
    switch (t) {
    case ServerProtocol::MessageType::AuthResponse:
        handleAuthResponse(message);
        break;
    case ServerProtocol::MessageType::HeartbeatResponse:
        qDebug() << "Networkclient: 收到服务器心跳响应";
        m_heartbeat->onHeartbeatResponse();
        break;
    case ServerProtocol::MessageType::PersonSync:
        handlePersonSynResponse(message);
        break;
    case ServerProtocol::MessageType::FaceSyncBegin:
        qDebug() << "Networkclient: 收到人脸同步开始";
        break;
    case ServerProtocol::MessageType::FaceSyncEnd:
        qDebug() << "Networkclient: 收到人脸同步结束";
        break;
    case ServerProtocol::MessageType::AttendanceReportResponse:
        handleUploadResponse(message);
        break;
    case ServerProtocol::MessageType::DeviceStatusReportResponse:
        qDebug() << "Networkclient: 收到设备状态上报响应";
        break;
    case ServerProtocol::MessageType::DeviceCommand:
        qDebug() << "Networkclient: 收到远程指令" << message;
        break;
    case ServerProtocol::MessageType::Error:
        handleServerError(message);
        break;
    default:
        qWarning() << "Networkclient: 未知服务器消息"
                   << message.value(ServerProtocol::kType).toString();
        break;
    }
}

void Networkclient::onBinaryFrameReceived(const QJsonObject &header, const QByteArray &payload)
{
    m_heartbeat->onAnyMessage();

    qDebug() << "Networkclient: 收到二进制帧, employeeId="
             << header.value(QStringLiteral("employeeId")).toString()
             << "payload size=" << payload.size();

    emit faceSyncItemReceived(header, payload);
}

// ---------------------------------------------------------------------------
// Auth response
// ---------------------------------------------------------------------------

void Networkclient::handleAuthResponse(const QJsonObject &message)
{
    // 支持 root 级和 data 嵌套两种 code 字段
    int code = message.value(QStringLiteral("code")).toInt(-1);
    if (code == -1) {
        const QJsonObject d = message.value(QStringLiteral("data")).toObject();
        code = d.value(QStringLiteral("code")).toInt(0);
    }

    if (code == ServerProtocol::kCodeOk) {
        // ---------- 认证成功 ----------
        m_isAuthenticated = true;
        m_connection->setAuthenticated(true);

        // sessionToken
        m_sessionToken = message.value(QStringLiteral("sessionToken")).toString();
        if (m_sessionToken.isEmpty()) {
            const QJsonObject d = message.value(QStringLiteral("data")).toObject();
            m_sessionToken = d.value(QStringLiteral("sessionToken")).toString();
        }

        // 心跳间隔
        int heartbeatSec = message.value(QStringLiteral("heartbeatSec")).toInt(0);
        if (heartbeatSec == 0) {
            const QJsonObject d = message.value(QStringLiteral("data")).toObject();
            heartbeatSec = d.value(QStringLiteral("heartbeatSec")).toInt(0);
        }
        if (heartbeatSec > 0)
            m_heartbeat->setHeartbeatInterval(heartbeatSec);

        // 认证成功后启动心跳
        m_heartbeat->start(heartbeatSec > 0 ? heartbeatSec : 30);

        qDebug() << "Networkclient: auth 成功, deviceId=" << m_deviceId
                 << "heartbeatSec=" << heartbeatSec;

        // 按需发送设备状态上报
        sendDeviceStatusReport();

        // 处理队列中积压的消息
        processQueue();

        // 可选：触发初始同步
        syncPersonData();

        emit authSuccess();

    } else if (code == ServerProtocol::kCodeAuthFailed) {
        // ---------- 2002：认证失败（凭据错误）----------
        m_isAuthenticated = false;
        const QString msg = message.value(QStringLiteral("msg")).toString();
        qWarning() << "Networkclient: auth 失败 (2002), 凭据无效:" << msg;
        emit authFailed(code, msg);

    } else if (code == ServerProtocol::kCodeDuplicateSession) {
        // ---------- 2003：重复会话 ----------
        m_isAuthenticated = false;
        const QString msg = message.value(QStringLiteral("msg")).toString();
        qWarning() << "Networkclient: auth 失败 (2003), 重复会话:" << msg;
        // 断开当前连接，由重连机制自动重建
        m_connection->disconnect();
        emit authFailed(code, msg);

    } else {
        // ---------- 其他错误 ----------
        m_isAuthenticated = false;
        m_connection->setAuthenticated(false);
        const QString msg = message.value(QStringLiteral("msg")).toString();
        qWarning() << "Networkclient: auth 失败, code=" << code << msg;
        emit authFailed(code, msg);
    }
}

// ---------------------------------------------------------------------------
// Device status report (called after auth)
// ---------------------------------------------------------------------------

void Networkclient::sendDeviceStatusReport()
{
    QJsonObject extra;
    extra[QStringLiteral("status")] = QStringLiteral("online");

    const QJsonObject msg = ServerProtocol::buildDeviceStatusReport(
        m_deviceId, m_deviceName, /*ipAddress=*/{}, m_fwVersion, extra);

    if (m_writer)
        m_writer->send(msg);

    qDebug() << "Networkclient: 已发送 device.status.report";
}

// ---------------------------------------------------------------------------
// Heartbeat / send slots
// ---------------------------------------------------------------------------

void Networkclient::onHeartbeatTimeout()
{
    qWarning() << "Networkclient: 心跳超时，触发重连";
    m_connection->disconnect();
}

void Networkclient::onSendError()
{
    qWarning() << "Networkclient: 消息发送错误";
}

void Networkclient::onSendHeartbeat(const QByteArray &data)
{
    if (m_writer) {
        m_writer->send(data);
    } else {
        qWarning() << "Networkclient: writer未初始化，无法发送心跳";
    }
}

// ---------------------------------------------------------------------------
// Constructor / setup
// ---------------------------------------------------------------------------

Networkclient::Networkclient(QObject *parent)
    : QObject(parent)
    , m_connection(new Connectionmanager(this))
    , m_heartbeat(new Heartbeatmanager(this))
    , m_writer(nullptr)
    , m_ready(nullptr)
    , m_queue(new Messagequeue(this))
    , m_isOnline(false)
{
    setupConnections();
    loadDeviceConfig();
    qDebug() << "Networkclient 初始化完成, deviceId=" << m_deviceId;
}

void Networkclient::setupConnections()
{
    connect(m_connection, &Connectionmanager::connected,    this, &Networkclient::onConnectionConnected);
    connect(m_connection, &Connectionmanager::disconnected, this, &Networkclient::onConnectionDisconnected);
    connect(m_connection, &Connectionmanager::stateChanged, this, &Networkclient::onConnectionStateChanged);

    connect(m_heartbeat, &Heartbeatmanager::sendHeartbeat, this, &Networkclient::onSendHeartbeat);
}

void Networkclient::loadDeviceConfig()
{
    ConfigManager *cfg = ConfigManager::instance();

    // 从配置文件读取 deviceId / deviceKey；缺省保留硬编码默认值
    const QString cfgId  = cfg->getDeviceId();
    const QString cfgKey = cfg->getDeviceKey();
    const QString cfgFw  = cfg->getFwVersion();

    if (!cfgId.isEmpty())
        m_deviceId = cfgId;
    if (!cfgKey.isEmpty())
        m_deviceKey = cfgKey;
    if (!cfgFw.isEmpty())
        m_fwVersion = cfgFw;
}

// ---------------------------------------------------------------------------
// Queue
// ---------------------------------------------------------------------------

void Networkclient::processQueue()
{
    if (m_queue->isEmpty()) return;

    if (!m_writer) {
        qWarning() << "Networkclient: writer未初始化，无法处理队列";
        return;
    }

    qDebug() << "Networkclient: 处理队列" << m_queue->size() << "条消息等待发送";

    QVector<QJsonObject> message = m_queue->dequeueAll();
    int sent = m_writer->sendBatch(message);

    if (sent < message.size()) {
        for (int i = sent; i < message.size(); ++i)
            m_queue->enqueue(message[i]);
        qWarning() << "Networkclient: 队列处理中断"
                   << (message.size() - sent) << "条重新入队";
    }
}

// ---------------------------------------------------------------------------
// Handlers
// ---------------------------------------------------------------------------

void Networkclient::handlePersonSynResponse(const QJsonObject &message)
{
    const auto items = ServerProtocol::parsePersons(message);

    QVector<ServerProtocol::PersonData> persons;
    persons.reserve(items.size());
    for (const auto& it : items) {
        ServerProtocol::PersonData p;
        p.employeeId  = it.employeeId;
        p.name        = it.name;
        p.faceFeature.clear();
        p.featureSize = 0;
        persons.push_back(p);
    }

    qDebug() << "Networkclient: 收到人员数据：" << persons.size() << "条";

    if (LocalStorage::instance()->syncPersons(persons)) {
        FaceDatabaseManager::instance()->loadFromDatabase();
        emit personDataReceived(persons);
    } else {
        qWarning() << "人员数据同步到本地数据库失败";
    }
}

void Networkclient::handleUploadResponse(const QJsonObject &message)
{
    bool success = message.value(QStringLiteral("success")).toBool();
    QString msg = message.value(QStringLiteral("message")).toString();
    emit uploadFinished(success, msg);
}

void Networkclient::handleServerError(const QJsonObject &message)
{
    const QString msg = message.value(ServerProtocol::kMessage).toString();
    qWarning() << "Networkclient: server error:" << msg;
    emit uploadFinished(false, msg);
}
