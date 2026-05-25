#include "networkclient.h"
#include "../Config/configmanager.h"
#include "../Utils/Logger.h"

#include <QElapsedTimer>
#include <QMutex>
#include <QNetworkInterface>
#include <QTime>
#include <QUuid>

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

Networkclient *Networkclient::instance()
{
    static Networkclient* s_instance = nullptr;
    static QMutex s_mutex;
    if (!s_instance) {
        QMutexLocker locker(&s_mutex);
        if (!s_instance) {
            s_instance = new Networkclient();
        }
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
// Business — sync
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

// ---------------------------------------------------------------------------
// Business — attendance report (outbox-based)
// ---------------------------------------------------------------------------

QString Networkclient::uploadAttendance(const QString& employeeId,
                                         const QString& status,
                                         const QDateTime& checkTime)
{
    const QString clientMsgId = ServerProtocol::generateMsgId();

    // 1. 写入 outbox（state=pending）
    OutboxRecord record;
    record.clientMsgId = clientMsgId;
    record.employeeId  = employeeId;
    record.checkTime   = checkTime.isValid() ? checkTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
                                             : QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    record.status      = status;
    record.state       = QStringLiteral("pending");

    AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
    if (!outbox.enqueue(record)) {
        qWarning() << "Networkclient: outbox 写入失败";
        return clientMsgId; // 仍返回 msgId，调用方可追踪
    }

    qDebug() << "Networkclient: 考勤记录已写入 outbox, msgId=" << clientMsgId;

    // 2. 如果已认证且连接中，通过 invokeMethod 将发送操作排队到网络线程
    if (m_isAuthenticated && m_writer) {
        QMetaObject::invokeMethod(this, [this, clientMsgId, employeeId, checkTime, status]() {
            AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
            const auto stored = outbox.findByClientMsgId(clientMsgId);
            if (stored.id > 0) {
                outbox.markState(stored.id, QStringLiteral("sending"));
            }

            qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                     << "[开始] 发送打卡请求"
                     << "employeeId=" << employeeId;

            const QJsonObject msg = ServerProtocol::buildAttendanceReport(
                employeeId, checkTime, m_deviceId, status,
                /*awaitPhoto=*/false, clientMsgId);

            if (!m_writer) {
                qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                         << "[失败] 网络发送"
                         << "msgId=" << clientMsgId
                         << "原因=发送失败";
                outbox.markState(stored.id, QStringLiteral("pending"));
                qWarning() << "Networkclient: 考勤发送失败，等待重试";
                return;
            }

            qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                     << "[进行中] 网络发送"
                     << "msgId=" << clientMsgId;

            bool sent = m_writer->send(msg);

            if (!sent) {
                qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                         << "[失败] 网络发送"
                         << "msgId=" << clientMsgId
                         << "原因=发送失败";
                // 发送失败，退回 pending 等待重试
                outbox.markState(stored.id, QStringLiteral("pending"));
                qWarning() << "Networkclient: 考勤发送失败，等待重试";
            } else {
                qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                         << "[完成] 网络发送"
                         << "msgId=" << clientMsgId;
            }
        }, Qt::QueuedConnection);
    }

    return clientMsgId;
}

QString Networkclient::uploadAttendanceWithPhoto(const QString& employeeId,
                                                  const QString& status,
                                                  const QByteArray& photoJpeg,
                                                  const QDateTime& checkTime)
{
    const QString clientMsgId = ServerProtocol::generateMsgId();

    // 1. 写入 outbox（含照片 BLOB）
    OutboxRecord record;
    record.clientMsgId = clientMsgId;
    record.employeeId  = employeeId;
    record.checkTime   = checkTime.isValid() ? checkTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
                                             : QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    record.status      = status;
    record.photoBlob   = photoJpeg;
    record.photoSize   = photoJpeg.size();
    record.state       = QStringLiteral("pending");

    AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
    if (!outbox.enqueue(record)) {
        qWarning() << "Networkclient: outbox 写入失败（带照片）";
        return clientMsgId;
    }

    qDebug() << "Networkclient: 考勤记录（带照片）已写入 outbox, msgId=" << clientMsgId
             << "photo size=" << photoJpeg.size();

    // 2. 通过 invokeMethod 将 awaitPhoto 发送流程排队到网络线程
    if (m_isAuthenticated && m_writer) {
        QMetaObject::invokeMethod(this, [this, clientMsgId, employeeId, checkTime, status, photoJpeg]() {
            AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
            const auto stored = outbox.findByClientMsgId(clientMsgId);
            if (stored.id > 0) {
                outbox.markState(stored.id, QStringLiteral("sending"));
            }

            if (!m_writer) return;

            // 2a. attendance.report (awaitPhoto=true)
            const QJsonObject reportMsg = ServerProtocol::buildAttendanceReport(
                employeeId, checkTime, m_deviceId, status,
                /*awaitPhoto=*/true, clientMsgId);

            if (!m_writer->send(reportMsg)) {
                outbox.markState(stored.id, QStringLiteral("pending"));
                qWarning() << "Networkclient: awaitPhoto report 发送失败";
                return;
            }

            // 2b. attendance.photo.header
            const QJsonObject headerMsg = ServerProtocol::buildAttendancePhotoHeader(
                m_deviceId, employeeId, photoJpeg.size());

            if (!m_writer->send(headerMsg)) {
                outbox.markState(stored.id, QStringLiteral("pending"));
                qWarning() << "Networkclient: photo.header 发送失败";
                return;
            }

            // 2c. 原始 JPEG 字节（无前缀）
            if (!m_writer->sendRawBytes(photoJpeg)) {
                outbox.markState(stored.id, QStringLiteral("pending"));
                qWarning() << "Networkclient: 照片原始字节发送失败";
                return;
            }
        }, Qt::QueuedConnection);
    }

    return clientMsgId;
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
    LOG_INFO("TCP 已连接，发送认证");

    QTcpSocket *socket = m_connection->socket();
    m_writer = new Messagewriter(socket);
    m_ready  = new Messagereader(socket);

    connect(m_ready,  &Messagereader::messageReceived,      this, &Networkclient::onMessageReceived);
    connect(m_ready,  &Messagereader::binaryFrameReceived,   this, &Networkclient::onBinaryFrameReceived);
    connect(m_writer, &Messagewriter::sendError,             this, &Networkclient::onSendError);

    m_ready->start();

    // 设置心跳 socket，但不启动 —— 等 auth 成功后再启动
    m_heartbeat->setSocket(socket);
    connect(m_heartbeat, &Heartbeatmanager::heartbeattimeout, this, &Networkclient::onHeartbeatTimeout, Qt::UniqueConnection);

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
    LOG_INFO("连接断开，清理资源");

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

    QObject::disconnect(m_heartbeat, &Heartbeatmanager::heartbeattimeout,
                        this, &Networkclient::onHeartbeatTimeout);

    delete m_writer;  m_writer = nullptr;
    delete m_ready;   m_ready  = nullptr;

    // 将 sending 状态的记录退回 pending，等待重连后重试
    {
        AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
        auto pending = outbox.fetchPending(200);
        for (const auto &r : pending) {
            if (r.state == QLatin1String("sending")) {
                outbox.markState(r.id, QStringLiteral("pending"));
            }
        }
    }

    // 停止 outbox 重试定时器
    if (m_outboxRetryTimer) {
        m_outboxRetryTimer->stop();
    }
    m_outboxRetryRound = 0;

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
        LOG_DEBUG("收到服务器心跳响应");
        m_heartbeat->onHeartbeatResponse();
        break;
    case ServerProtocol::MessageType::PersonSync:
        handlePersonSynResponse(message);
        emit personSyncReceived(message);
        break;
    case ServerProtocol::MessageType::FaceSyncBegin:
        qDebug() << "Networkclient: 收到人脸同步开始";
        emit faceSyncBeginReceived(message);
        break;
    case ServerProtocol::MessageType::FaceSyncEnd:
        qDebug() << "Networkclient: 收到人脸同步结束";
        emit faceSyncEndReceived(message);
        break;
    case ServerProtocol::MessageType::AttendanceReportResponse:
        handleUploadResponse(message);
        break;
    case ServerProtocol::MessageType::DeviceStatusReportResponse:
        qDebug() << "Networkclient: 收到设备状态上报响应";
        break;
    case ServerProtocol::MessageType::DeviceCommand:
        qDebug() << "Networkclient: 收到远程指令";
        emit deviceCommandReceived(message);
        break;
    case ServerProtocol::MessageType::TokenRefreshResponse:
        handleTokenRefreshResponse(message);
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

    const QJsonObject frameData = header.value(QStringLiteral("data")).toObject();
    qDebug() << "Networkclient: 收到二进制帧, employeeId="
             << frameData.value(QStringLiteral("employeeId")).toString()
             << "payload size=" << payload.size();

    emit faceSyncItemReceived(header, payload);
}

// ---------------------------------------------------------------------------
// Auth response
// ---------------------------------------------------------------------------

void Networkclient::handleAuthResponse(const QJsonObject &message)
{
    int code = message.value(QStringLiteral("code")).toInt(-1);
    if (code == -1) {
        const QJsonObject d = message.value(QStringLiteral("data")).toObject();
        code = d.value(QStringLiteral("code")).toInt(0);
    }

    if (code == ServerProtocol::kCodeOk) {
        // ---------- 认证成功 ----------

        // 获取响应数据
        const QJsonObject data = message.value(QStringLiteral("data")).toObject();

        // sessionToken (兼容旧版本)
        m_sessionToken = message.value(QStringLiteral("sessionToken")).toString();
        if (m_sessionToken.isEmpty()) {
            m_sessionToken = data.value(QStringLiteral("sessionToken")).toString();
        }

        // JWT 令牌
        const QString accessToken = data.value(QStringLiteral("accessToken")).toString();
        const QString refreshToken = data.value(QStringLiteral("refreshToken")).toString();
        const int expiresIn = data.value(QStringLiteral("expiresIn")).toInt(3600);

        if (!accessToken.isEmpty() && !refreshToken.isEmpty()) {
            // 存储 JWT 令牌
            TokenManager::TokenPair tokens;
            tokens.accessToken = accessToken;
            tokens.refreshToken = refreshToken;
            tokens.expiresIn = expiresIn;
            m_tokenManager->storeTokens(tokens);

            // 启动自动刷新
            m_tokenRefresher->startAutoRefresh();

            LOG_INFO(QStringLiteral("JWT 令牌已存储, expiresIn=%1").arg(expiresIn));
        }

        // 新增：检查是否收到新密钥
        const QString newDeviceKey = data.value(QStringLiteral("deviceKey")).toString();
        if (!newDeviceKey.isEmpty() && newDeviceKey != m_deviceKey) {
            // 更新本地密钥
            const QString oldKey = m_deviceKey;
            m_deviceKey = newDeviceKey;
            ConfigManager::instance()->setDeviceKey(newDeviceKey);
            ConfigManager::instance()->saveConfig();

            LOG_INFO(QStringLiteral("Device key updated from server: %1 old: %2 - scheduling reconnect to verify")
                         .arg(newDeviceKey, oldKey));

            // 延迟断开重连，让新密钥立即接受服务端验证
            QTimer::singleShot(300, this, &Networkclient::scheduleReconnect);
            return;  // 不继续执行后续的正常认证成功逻辑，交给重连
        }

        // 心跳间隔
        int heartbeatSec = message.value(QStringLiteral("heartbeatSec")).toInt(0);
        if (heartbeatSec == 0) {
            heartbeatSec = data.value(QStringLiteral("heartbeatSec")).toInt(0);
        }
        if (heartbeatSec > 0)
            m_heartbeat->setHeartbeatInterval(heartbeatSec);

        // 认证成功后设置状态
        m_isAuthenticated = true;
        m_connection->setAuthenticated(true);

        // 认证成功后启动心跳
        m_heartbeat->start(heartbeatSec > 0 ? heartbeatSec : 30);

        LOG_INFO(QStringLiteral("auth 成功, deviceId=%1 heartbeatSec=%2")
                     .arg(m_deviceId).arg(heartbeatSec));

        // 按需发送设备状态上报
        sendDeviceStatusReport();

        // 处理内存队列中积压的消息
        processQueue();

        // 处理 outbox 中的待发送记录
        processOutbox();

        emit authSuccess();

    } else if (code == ServerProtocol::kCodeAuthFailed) {
        // ---------- 2002：认证失败（凭据错误或待审核）----------
        m_isAuthenticated = false;
        const QString msgText = message.value(QStringLiteral("msg")).toString();

        // 新增：检查是否为设备待审核状态
        if (msgText.contains(QStringLiteral("pending authorization"))) {
            // 设备待审核提示
            qDebug() << "Device pending authorization, waiting for admin approval";
            emit devicePendingAuth();
        } else {
            LOG_ERROR(QStringLiteral("auth 失败 (2002), 凭据无效: %1").arg(msgText));
            emit authFailed(code, msgText);
        }

    } else if (code == ServerProtocol::kCodeDuplicateSession) {
        // ---------- 2003：重复会话 ----------
        m_isAuthenticated = false;
        const QString msg = message.value(QStringLiteral("msg")).toString();
        LOG_WARNING(QStringLiteral("auth 失败 (2003), 重复会话: %1").arg(msg));
        m_connection->disconnect();
        emit authFailed(code, msg);

    } else {
        // ---------- 其他错误 ----------
        m_isAuthenticated = false;
        m_connection->setAuthenticated(false);
        const QString msg = message.value(QStringLiteral("msg")).toString();
        LOG_ERROR(QStringLiteral("auth 失败, code=%1 %2").arg(code).arg(msg));
        emit authFailed(code, msg);
    }
}

// ---------------------------------------------------------------------------
// Device status report (called after auth)
// ---------------------------------------------------------------------------

void Networkclient::sendDeviceStatusReport()
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
        qDebug() << "Networkclient: IP 变更" << lastIp << "→" << ipAddress;
    }

    // 构建并发送
    QJsonObject extra;
    extra[QStringLiteral("status")] = QStringLiteral("online");

    const QString reportIp = ipAddress.isEmpty() ? lastIp : ipAddress;
    const QJsonObject msg = ServerProtocol::buildDeviceStatusReport(
        m_deviceId, m_deviceName, reportIp, m_fwVersion, extra);

    if (m_writer)
        m_writer->send(msg);

    qDebug() << "Networkclient: 已发送 device.status.report, ip=" << reportIp;
}

// ---------------------------------------------------------------------------
// Heartbeat / send slots
// ---------------------------------------------------------------------------

void Networkclient::onHeartbeatTimeout()
{
    LOG_WARNING("心跳超时，触发重连");
    m_connection->disconnect();
}

void Networkclient::onSendError()
{
    LOG_WARNING("消息发送错误");
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
    , m_tokenManager(new TokenManager(this))
    , m_tokenRefresher(nullptr)
    , m_isOnline(false)
    , m_outboxRetryTimer(new QTimer(this))
{
    m_outboxRetryTimer->setSingleShot(true);
    connect(m_outboxRetryTimer, &QTimer::timeout, this, &Networkclient::onOutboxRetryTick);

    setupConnections();
    loadDeviceConfig();

    // 初始化令牌刷新器（延迟初始化，因为需要 this 指针）
    m_tokenRefresher = new TokenRefresher(this, m_tokenManager, this);

    LOG_INFO(QStringLiteral("Networkclient 初始化完成, deviceId=%1").arg(m_deviceId));
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

    m_maxRetryCount      = cfg->getMaxRetryCount();
    m_retryBackoffBaseMs = cfg->getRetryBackoffBaseMs();
}

// ---------------------------------------------------------------------------
// Memory queue (legacy — kept for non-attendance messages)
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

void Networkclient::retryOutbox()
{
    if (m_isAuthenticated && m_writer) {
        m_outboxRetryRound = 0;
        processOutbox();
    }
}

bool Networkclient::sendJson(const QJsonObject &message)
{
    if (!m_writer) {
        qWarning() << "Networkclient::sendJson: writer 未初始化";
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

// ---------------------------------------------------------------------------
// Outbox processing — reconnect retry
// ---------------------------------------------------------------------------

void Networkclient::processOutbox()
{
    if (!m_isAuthenticated || !m_writer) {
        qDebug() << "Networkclient: processOutbox 跳过，未认证或无 writer";
        return;
    }

    AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();

    // 先将超过最大重试次数的记录标记为 dead
    auto allPending = outbox.fetchPending(200);
    for (const auto &r : allPending) {
        if (r.retryCount >= m_maxRetryCount && r.state != QLatin1String("dead")) {
            outbox.markDead(r.id, QStringLiteral("max retry %1 exceeded").arg(m_maxRetryCount));
            LOG_WARNING(QStringLiteral("outbox 记录标记 dead, msgId=%1").arg(r.clientMsgId));
        }
    }

    // 获取待发送记录（pending + failed，不含 dead）
    auto records = outbox.fetchPending(50);
    if (records.isEmpty()) {
        qDebug() << "Networkclient: outbox 无待发送记录";
        m_outboxRetryRound = 0;
        return;
    }

    // 分离带照片和不带照片的记录
    QVector<OutboxRecord> noPhotoRecords;
    QVector<OutboxRecord> photoRecords;

    for (const auto &r : records) {
        if (r.photoSize > 0 && !r.photoBlob.isEmpty())
            photoRecords.append(r);
        else
            noPhotoRecords.append(r);
    }

    // 无照片记录：批量发送
    for (const auto &r : noPhotoRecords) {
        QJsonObject msg = ServerProtocol::buildAttendanceReport(
            r.employeeId,
            QDateTime::fromString(r.checkTime, QStringLiteral("yyyy-MM-dd HH:mm:ss")),
            m_deviceId,
            r.status,
            /*awaitPhoto=*/false,
            r.clientMsgId);  // 复用原 msgId，保证幂等

        m_writer->send(msg);
        outbox.markState(r.id, QStringLiteral("sending"));
        qDebug() << "Networkclient: outbox 重试发送, msgId=" << r.clientMsgId;
    }

    // 带照片记录：逐个发送完整 awaitPhoto 流程
    for (const auto &r : photoRecords) {
        // report (awaitPhoto=true)
        QJsonObject reportMsg = ServerProtocol::buildAttendanceReport(
            r.employeeId,
            QDateTime::fromString(r.checkTime, QStringLiteral("yyyy-MM-dd HH:mm:ss")),
            m_deviceId,
            r.status,
            /*awaitPhoto=*/true,
            r.clientMsgId);

        m_writer->send(reportMsg);

        // photo.header
        QJsonObject headerMsg = ServerProtocol::buildAttendancePhotoHeader(
            m_deviceId, r.employeeId, r.photoSize);

        m_writer->send(headerMsg);

        // raw photo bytes
        m_writer->sendRawBytes(r.photoBlob);

        outbox.markState(r.id, QStringLiteral("sending"));
        qDebug() << "Networkclient: outbox 重试发送（带照片）, msgId=" << r.clientMsgId;
    }
}

void Networkclient::onOutboxRetryTick()
{
    qDebug() << "Networkclient: outbox 退避重试, round=" << m_outboxRetryRound;

    if (m_isAuthenticated && m_writer) {
        processOutbox();
    }
}

void Networkclient::scheduleReconnect()
{
    LOG_INFO("密钥已更新，正在断开并重连以验证新密钥...");
    m_connection->scheduleReconnect();
}

// ---------------------------------------------------------------------------
// Response handlers
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
    // 提取 inReplyTo（对应考勤上报的 client_msg_id）
    const QString inReplyTo = message.value(QStringLiteral("inReplyTo")).toString();
    int code = message.value(QStringLiteral("code")).toInt(-1);

    if (code == -1) {
        const QJsonObject d = message.value(QStringLiteral("data")).toObject();
        code = d.value(QStringLiteral("code")).toInt(0);
    }

    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[开始] 等待服务器响应"
             << "msgId=" << inReplyTo;

    qDebug() << "Networkclient: 收到 attendance.report.response, inReplyTo=" << inReplyTo
             << "code=" << code;

    if (inReplyTo.isEmpty()) {
        // 无 inReplyTo，无法匹配 outbox 记录，按旧逻辑处理
        bool success = (code == ServerProtocol::kCodeOk);
        QString msg = message.value(QStringLiteral("msg")).toString();
        emit uploadFinished(success, msg);
        return;
    }

    AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
    const OutboxRecord record = outbox.findByClientMsgId(inReplyTo);

    if (record.id == 0) {
        // outbox 中未找到（可能已被清理），仅发信号
        qDebug() << "Networkclient: outbox 中未找到 msgId=" << inReplyTo;
        bool success = (code == ServerProtocol::kCodeOk);
        emit uploadFinished(success, message.value(QStringLiteral("msg")).toString());
        return;
    }

    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[完成] 收到服务器响应"
             << "msgId=" << inReplyTo
             << "code=" << code;

    if (code == ServerProtocol::kCodeOk) {
        // ---------- 成功：删除 outbox 记录 ----------
        outbox.remove(record.id);
        LOG_INFO(QStringLiteral("考勤上报成功, employeeId=%1").arg(record.employeeId));

        qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                 << "[完成] 更新Outbox状态"
                 << "msgId=" << inReplyTo
                 << "新状态=confirmed";

        qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                 << "[完成] 打卡记录上传"
                 << "employeeId=" << record.employeeId
                 << "msgId=" << inReplyTo
                 << "状态=成功";

        emit attendanceReportResult(record.employeeId, true, QString());
        emit uploadFinished(true, QStringLiteral("ok"));

        // 重置退避轮次
        m_outboxRetryRound = 0;

    } else if (code == ServerProtocol::kCodeEmployeeNotFound) {
        // ---------- 4011：员工不存在 → dead ----------
        outbox.markDead(record.id, QStringLiteral("employee not found (4011)"));
        LOG_WARNING(QStringLiteral("员工不存在, 标记 dead, employeeId=%1").arg(record.employeeId));

        qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                 << "[失败] 打卡记录上传"
                 << "employeeId=" << record.employeeId
                 << "msgId=" << inReplyTo
                 << "原因=employee not found";

        emit attendanceReportResult(record.employeeId, false, QStringLiteral("employee not found"));
        emit uploadFinished(false, QStringLiteral("employee not found"));

    } else {
        // ---------- 其他错误 (6002 等)：重试 ----------
        outbox.incrementRetry(record.id,
                              QStringLiteral("code=%1 msg=%2")
                                  .arg(code)
                                  .arg(message.value(QStringLiteral("msg")).toString()));

        qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                 << "[完成] 更新Outbox状态"
                 << "msgId=" << inReplyTo
                 << "新状态=failed";

        const int newRetryCount = record.retryCount + 1;
        if (newRetryCount >= m_maxRetryCount) {
            outbox.markDead(record.id, QStringLiteral("max retry after error"));
            LOG_ERROR(QStringLiteral("达到最大重试次数, 标记 dead, employeeId=%1").arg(record.employeeId));
        } else {
            outbox.markState(record.id, QStringLiteral("failed"),
                             QStringLiteral("code=%1").arg(code));
            LOG_WARNING(QStringLiteral("考勤上报失败, 将重试, retry=%1").arg(newRetryCount));
        }

        qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                 << "[失败] 打卡记录上传"
                 << "employeeId=" << record.employeeId
                 << "msgId=" << inReplyTo
                 << "原因=" << message.value(QStringLiteral("msg")).toString();

        emit attendanceReportResult(record.employeeId, false,
                                    QStringLiteral("code=%1").arg(code));
        emit uploadFinished(false, message.value(QStringLiteral("msg")).toString());

        // 启动退避重试定时器
        if (!m_outboxRetryTimer->isActive()) {
            m_outboxRetryRound++;
            int delay = m_retryBackoffBaseMs * (1 << qMin(m_outboxRetryRound, 4));
            m_outboxRetryTimer->start(delay);
            qDebug() << "Networkclient: outbox 退避重试 scheduled, delay=" << delay << "ms";
        }
    }
}

void Networkclient::handleServerError(const QJsonObject &message)
{
    const QString inReplyTo = message.value(QStringLiteral("inReplyTo")).toString();
    int code = message.value(QStringLiteral("code")).toInt(-1);

    LOG_WARNING(QStringLiteral("server error, code=%1").arg(code));

    // 如果 error 消息关联了考勤上报，按同样的逻辑处理
    if (!inReplyTo.isEmpty()) {
        AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
        const OutboxRecord record = outbox.findByClientMsgId(inReplyTo);

        if (record.id > 0) {
            if (code == ServerProtocol::kCodeEmployeeNotFound) {
                outbox.markDead(record.id, QStringLiteral("employee not found"));
            } else {
                outbox.incrementRetry(record.id,
                                      QStringLiteral("error code=%1").arg(code));
                if (record.retryCount + 1 >= m_maxRetryCount) {
                    outbox.markDead(record.id, QStringLiteral("max retry after error"));
                } else {
                    outbox.markState(record.id, QStringLiteral("failed"));
                }
            }
        }
    }

    emit uploadFinished(false, message.value(ServerProtocol::kMessage).toString());
}

void Networkclient::handleTokenRefreshResponse(const QJsonObject &message)
{
    qDebug() << "Networkclient: 收到令牌刷新响应";
    emit tokenRefreshResponse(message);
}

QJsonObject Networkclient::addTokenToMessage(const QJsonObject &message)
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
