#include "networkclient.h"
#include "../Config/configmanager.h"
#include "../Utils/Logger.h"
#include "../LocalStorage/localstorage.h"
#include "../FaceRecognition/facedatabasemanager.h"

#include <QMutex>
#include <QNetworkInterface>
#include <QTime>
#include <QUuid>

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

NetworkClient *NetworkClient::instance()
{
    // C++11 保证 local static 对象的线程安全初始化，
    // 不需要 DCLP（Double-Checked Locking Pattern）
    static NetworkClient s_instance;
    return &s_instance;
}

// ---------------------------------------------------------------------------
// Connection delegation
// ---------------------------------------------------------------------------

bool NetworkClient::connectToServer(const QString &ip, quint16 port)
{
    return m_connection->connectToHost(ip, port);
}

void NetworkClient::disconnect()
{
    m_connection->disconnect();
}

bool NetworkClient::isConnected() const
{
    return m_connection->isConnect();
}

// ---------------------------------------------------------------------------
// Auth delegation
// ---------------------------------------------------------------------------

bool NetworkClient::isAuthenticated() const
{
    return m_authenticator->isAuthenticated();
}

QString NetworkClient::sessionToken() const
{
    return m_authenticator->sessionToken();
}

// ---------------------------------------------------------------------------
// Device identity
// ---------------------------------------------------------------------------

void NetworkClient::setDeviceId(const QString &deviceId)
{
    if (deviceId.isEmpty()) return;
    m_deviceId = deviceId;
}

void NetworkClient::setDeviceKey(const QString &deviceKey)
{
    m_deviceKey = deviceKey;
}

// ---------------------------------------------------------------------------
// Business — sync
// ---------------------------------------------------------------------------

bool NetworkClient::syncPersonData()
{
    if (!isConnected() || !m_writer) {
        return false;
    }

    const QJsonObject msg = ServerProtocol::buildSyncRequest(m_deviceId);
    return m_writer->send(msg);
}

// ---------------------------------------------------------------------------
// Business — attendance report (outbox-based)
// ---------------------------------------------------------------------------

QString NetworkClient::uploadAttendance(const QString& employeeId,
                                         const QString& status,
                                         const QDateTime& checkTime)
{
    const QString clientMsgId = m_outboxManager->enqueue(employeeId, status, checkTime);

    // 如果已认证且有 writer，立即触发发送
    if (m_authenticator->isAuthenticated() && m_writer) {
        QMetaObject::invokeMethod(this, [this, clientMsgId]() {
            AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
            const auto stored = outbox.findByClientMsgId(clientMsgId);
            if (stored.id > 0) {
                outbox.markState(stored.id, QStringLiteral("sending"));
            }
            m_outboxManager->processOutbox(m_writer, m_deviceId);
        }, Qt::QueuedConnection);
    }

    return clientMsgId;
}

QString NetworkClient::uploadAttendanceWithPhoto(const QString& employeeId,
                                                  const QString& status,
                                                  const QByteArray& photoJpeg,
                                                  const QDateTime& checkTime)
{
    const QString clientMsgId = m_outboxManager->enqueueWithPhoto(employeeId, status, photoJpeg, checkTime);

    // 如果已认证且有 writer，立即触发发送
    if (m_authenticator->isAuthenticated() && m_writer) {
        QMetaObject::invokeMethod(this, [this, clientMsgId]() {
            AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
            const auto stored = outbox.findByClientMsgId(clientMsgId);
            if (stored.id > 0) {
                outbox.markState(stored.id, QStringLiteral("sending"));
            }
            m_outboxManager->processOutbox(m_writer, m_deviceId);
        }, Qt::QueuedConnection);
    }

    return clientMsgId;
}

bool NetworkClient::uploadAttendanceBatch(const QVector<QJsonObject> &records)
{
    if (!m_queue) {
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

void NetworkClient::reportDeviceStatus(const QJsonObject &status)
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

void NetworkClient::onConnectionConnected()
{
    LOG_NET(Logger::Info, "TCP连接成功", "操作=发送认证");

    QTcpSocket *socket = m_connection->socket();
    m_writer = new MessageWriter(socket);
    m_ready  = new MessageReader(socket);

    connect(m_ready,  &MessageReader::messageReceived,      this, &NetworkClient::onMessageReceived);
    connect(m_ready,  &MessageReader::binaryFrameReceived,   this, &NetworkClient::onBinaryFrameReceived);
    connect(m_writer, &MessageWriter::sendError,             this, &NetworkClient::onSendError);

    m_ready->start();

    // 设置心跳 socket，但不启动 —— 等 auth 成功后再启动
    m_heartbeat->setSocket(socket);
    connect(m_heartbeat, &HeartbeatManager::heartbeatTimeout, this, &NetworkClient::onHeartbeatTimeout, Qt::UniqueConnection);

    // 首包：auth（委托给 Authenticator）
    loadDeviceConfig();
    m_authenticator->sendAuthRequest(m_deviceId, m_deviceKey, m_writer);

    m_isOnline = true;
    emit connected();
    emit networkStateChanged(true);
}

// ---------------------------------------------------------------------------
// TCP disconnected
// ---------------------------------------------------------------------------

void NetworkClient::onConnectionDisconnected()
{
    LOG_NET(Logger::Info, "连接断开", "操作=清理资源");

    m_heartbeat->stop();

    // 停止令牌刷新器
    if (m_tokenRefresher) {
        m_tokenRefresher->stopAutoRefresh();
    }

    if (m_ready) {
        m_ready->stop();
        m_ready->disconnect(this);
    }
    if (m_writer) {
        m_writer->disconnect(this);
    }

    QObject::disconnect(m_heartbeat, &HeartbeatManager::heartbeatTimeout,
                        this, &NetworkClient::onHeartbeatTimeout);

    delete m_writer;  m_writer = nullptr;
    delete m_ready;   m_ready  = nullptr;

    // 将 sending 状态的记录退回 pending，等待重连后重试（委托给 OutboxManager）
    m_outboxManager->rollbackSendingToPending();

    m_authenticator->setAuthenticated(false);
    m_isOnline = false;

    emit disconnected();
    emit networkStateChanged(false);
}

void NetworkClient::onConnectionStateChanged(bool isOnline)
{
    if (m_isOnline == isOnline) return;
    m_isOnline = isOnline;
    if (isOnline) processQueue();
    emit networkStateChanged(isOnline);
}

// ---------------------------------------------------------------------------
// Message dispatch
// ---------------------------------------------------------------------------

void NetworkClient::onMessageReceived(const QJsonObject &message)
{
    m_heartbeat->onAnyMessage();

    const auto t = ServerProtocol::parseType(message);
    switch (t) {
    case ServerProtocol::MessageType::AuthResponse:
        // 路由到 Authenticator
        m_authenticator->handleAuthResponse(message, m_heartbeat, m_connection);
        break;
    case ServerProtocol::MessageType::HeartbeatResponse:
        m_heartbeat->onHeartbeatResponse();
        break;
    case ServerProtocol::MessageType::PersonSync:
        handlePersonSynResponse(message);
        emit personSyncReceived(message);
        break;
    case ServerProtocol::MessageType::FaceSyncBegin:
        LOG_SYNC(Logger::Info, "人脸同步开始");
        emit faceSyncBeginReceived(message);
        break;
    case ServerProtocol::MessageType::FaceSyncEnd:
        LOG_SYNC(Logger::Info, "人脸同步结束");
        emit faceSyncEndReceived(message);
        break;
    case ServerProtocol::MessageType::AttendanceReportResponse:
        // 路由到 OutboxManager
        m_outboxManager->handleUploadResponse(message);
        break;
    case ServerProtocol::MessageType::DeviceStatusReportResponse:
        break;
    case ServerProtocol::MessageType::DeviceCommand:
        emit deviceCommandReceived(message);
        break;
    case ServerProtocol::MessageType::TokenRefreshResponse:
        handleTokenRefreshResponse(message);
        break;
    case ServerProtocol::MessageType::Error:
        // 路由到 OutboxManager
        m_outboxManager->handleServerError(message);
        break;
    default:
        break;
    }
}

void NetworkClient::onBinaryFrameReceived(const QJsonObject &header, const QByteArray &payload)
{
    m_heartbeat->onAnyMessage();
    emit faceSyncItemReceived(header, payload);
}

// ---------------------------------------------------------------------------
// Device status report (called after auth)
// ---------------------------------------------------------------------------

void NetworkClient::sendDeviceStatusReport()
{
    // 获取本机 IPv4 地址
    QString ipAddress;
    const auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto &iface : interfaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsUp) &&
            !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            const auto entries = iface.addressEntries();
            for (const auto &entry : entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    ipAddress = entry.ip().toString();
                    break;
                }
            }
        }
        if (!ipAddress.isEmpty()) break;
    }

    // 检测 IP 是否变更
    DeviceLocalRepository &devRepo = LocalStorage::instance()->deviceLocal();
    const QString lastIp = devRepo.get().ipAddress;

    if (!ipAddress.isEmpty() && ipAddress != lastIp) {
        devRepo.updateIpAddress(ipAddress);
    }

    // 构建并发送
    QJsonObject extra;
    extra[QStringLiteral("status")] = QStringLiteral("online");

    const QString reportIp = ipAddress.isEmpty() ? lastIp : ipAddress;
    const QJsonObject msg = ServerProtocol::buildDeviceStatusReport(
        m_deviceId, m_deviceName, reportIp, m_fwVersion, extra);

    if (m_writer)
        m_writer->send(msg);
}

// ---------------------------------------------------------------------------
// Heartbeat / send slots
// ---------------------------------------------------------------------------

void NetworkClient::onHeartbeatTimeout()
{
    LOG_NET(Logger::Warn, "心跳超时", "操作=触发重连");
    m_connection->disconnect();
}

void NetworkClient::onSendError()
{
    LOG_NET(Logger::Warn, "消息发送失败");
}

void NetworkClient::onSendHeartbeat(const QByteArray &data)
{
    if (m_writer) {
        m_writer->send(data);
    }
}

// ---------------------------------------------------------------------------
// Constructor / setup
// ---------------------------------------------------------------------------

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_connection(new ConnectionManager(this))
    , m_heartbeat(new HeartbeatManager(this))
    , m_writer(nullptr)
    , m_ready(nullptr)
    , m_queue(new MessageQueue(this))
    , m_tokenManager(new TokenManager(this))
    , m_tokenRefresher(nullptr)
    , m_authenticator(nullptr)
    , m_outboxManager(new OutboxManager(this))
    , m_isOnline(false)
{
    setupConnections();
    loadDeviceConfig();

    // 初始化令牌刷新器
    m_tokenRefresher = new TokenRefresher(this, m_tokenManager, this);

    // 初始化认证器（引用 TokenManager 和 TokenRefresher）
    m_authenticator = new Authenticator(m_tokenManager, m_tokenRefresher, this);

    // 连接 Authenticator 信号
    connect(m_authenticator, &Authenticator::authSuccess, this, [this]() {
        // 认证成功后：发送设备状态上报、处理队列、处理 outbox
        sendDeviceStatusReport();
        processQueue();
        m_outboxManager->processOutbox(m_writer, m_deviceId);
        emit authSuccess();
    });
    connect(m_authenticator, &Authenticator::authFailed, this, &NetworkClient::authFailed);
    connect(m_authenticator, &Authenticator::devicePendingAuth, this, &NetworkClient::devicePendingAuth);
    connect(m_authenticator, &Authenticator::deviceKeyUpdated, this, [this](const QString &newDeviceKey) {
        // 更新本地密钥
        const QString oldKey = m_deviceKey;
        m_deviceKey = newDeviceKey;
        ConfigManager::instance()->setDeviceKey(newDeviceKey);
        ConfigManager::instance()->saveConfig();

        LOG_NET(Logger::Info, "设备密钥已更新", QStringLiteral("新密钥=%1 | 旧密钥=%2 | 操作=计划重连").arg(newDeviceKey, oldKey));

        // 延迟断开重连
        QTimer::singleShot(300, this, &NetworkClient::scheduleReconnect);
    });

    // 连接 OutboxManager 信号
    connect(m_outboxManager, &OutboxManager::attendanceReportResult, this, &NetworkClient::attendanceReportResult);
    connect(m_outboxManager, &OutboxManager::uploadFinished, this, &NetworkClient::uploadFinished);

    LOG_NET(Logger::Info, "网络客户端初始化完成", QStringLiteral("deviceId=%1").arg(m_deviceId));
}

void NetworkClient::setupConnections()
{
    connect(m_connection, &ConnectionManager::connected,    this, &NetworkClient::onConnectionConnected);
    connect(m_connection, &ConnectionManager::disconnected, this, &NetworkClient::onConnectionDisconnected);
    connect(m_connection, &ConnectionManager::stateChanged, this, &NetworkClient::onConnectionStateChanged);

    connect(m_heartbeat, &HeartbeatManager::sendHeartbeat, this, &NetworkClient::onSendHeartbeat);
}

void NetworkClient::loadDeviceConfig()
{
    ConfigManager *cfg = ConfigManager::instance();

    const QString cfgId  = cfg->getDeviceId();
    const QString cfgKey = cfg->getDeviceKey();
    const QString cfgFw  = cfg->getFwVersion();
    const QString cfgName = cfg->getDeviceName();

    if (!cfgId.isEmpty())
        m_deviceId = cfgId;
    if (!cfgKey.isEmpty())
        m_deviceKey = cfgKey;
    if (!cfgFw.isEmpty())
        m_fwVersion = cfgFw;
    if (!cfgName.isEmpty())
        m_deviceName = cfgName;

    m_outboxManager->setMaxRetryCount(cfg->getMaxRetryCount());
    m_outboxManager->setRetryBackoffBaseMs(cfg->getRetryBackoffBaseMs());
}

// ---------------------------------------------------------------------------
// Memory queue (legacy — kept for non-attendance messages)
// ---------------------------------------------------------------------------

void NetworkClient::processQueue()
{
    if (m_queue->isEmpty()) return;

    if (!m_writer) {
        return;
    }

    QVector<QJsonObject> message = m_queue->dequeueAll();
    int sent = m_writer->sendBatch(message);

    if (sent < message.size()) {
        for (int i = sent; i < message.size(); ++i)
            m_queue->enqueue(message[i]);
    }
}

void NetworkClient::retryOutbox()
{
    if (m_authenticator->isAuthenticated() && m_writer) {
        m_outboxManager->retryAll(m_writer, m_deviceId);
    }
}

bool NetworkClient::sendJson(const QJsonObject &message)
{
    if (!m_writer) {
        return false;
    }
    // 自动添加 JWT 令牌（认证消息除外）
    QJsonObject msg = message;
    QString type = msg.value(QStringLiteral("type")).toString();
    if (type != QStringLiteral("auth") && type != QStringLiteral("token.refresh")) {
        msg = addTokenToMessage(msg);
    }
    return m_writer->send(msg);
}

void NetworkClient::scheduleReconnect()
{
    LOG_NET(Logger::Info, "密钥已更新", "操作=断开重连以验证新密钥");
    m_connection->scheduleReconnect();
}

// ---------------------------------------------------------------------------
// Response handlers
// ---------------------------------------------------------------------------

void NetworkClient::handlePersonSynResponse(const QJsonObject &message)
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

    LOG_SYNC(Logger::Info, "收到人员数据", QStringLiteral("数量=%1条").arg(persons.size()));

    if (LocalStorage::instance()->syncPersons(persons)) {
        FaceDatabaseManager::instance()->loadFromDatabase();
        emit personDataReceived(persons);
    } else {
        LOG_SYNC(Logger::Error, "人员数据同步失败", "详情=写入本地数据库失败");
    }
}

void NetworkClient::handleTokenRefreshResponse(const QJsonObject &message)
{
    emit tokenRefreshResponse(message);
}

QJsonObject NetworkClient::addTokenToMessage(const QJsonObject &message)
{
    // 如果有 JWT 访问令牌，添加到消息中
    QString token = m_tokenManager->accessToken();
    if (!token.isEmpty()) {
        QJsonObject modified = message;
        modified[QStringLiteral("token")] = token;
        return modified;
    }
    return message;
}
