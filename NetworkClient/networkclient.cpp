#include "networkclient.h"
#include "../Config/configmanager.h"

#include <QNetworkInterface>
#include <QUuid>

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

    // 2. 如果已认证且连接中，立即发送
    if (m_isAuthenticated && m_writer) {
        // 更新状态为 sending
        const auto stored = outbox.findByClientMsgId(clientMsgId);
        if (stored.id > 0) {
            outbox.markState(stored.id, QStringLiteral("sending"));
        }

        const QJsonObject msg = ServerProtocol::buildAttendanceReport(
            employeeId, checkTime, m_deviceId, status,
            /*awaitPhoto=*/false, clientMsgId);

        if (!m_writer->send(msg)) {
            // 发送失败，退回 pending 等待重试
            outbox.markState(stored.id, QStringLiteral("pending"));
            qWarning() << "Networkclient: 考勤发送失败，等待重试";
        }
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

    // 2. 发送 awaitPhoto 流程
    if (m_isAuthenticated && m_writer) {
        const auto stored = outbox.findByClientMsgId(clientMsgId);
        if (stored.id > 0) {
            outbox.markState(stored.id, QStringLiteral("sending"));
        }

        // 2a. attendance.report (awaitPhoto=true)
        const QJsonObject reportMsg = ServerProtocol::buildAttendanceReport(
            employeeId, checkTime, m_deviceId, status,
            /*awaitPhoto=*/true, clientMsgId);

        if (!m_writer->send(reportMsg)) {
            outbox.markState(stored.id, QStringLiteral("pending"));
            qWarning() << "Networkclient: awaitPhoto report 发送失败";
            return clientMsgId;
        }

        // 2b. attendance.photo.header
        const QJsonObject headerMsg = ServerProtocol::buildAttendancePhotoHeader(
            m_deviceId, employeeId, photoJpeg.size());

        if (!m_writer->send(headerMsg)) {
            outbox.markState(stored.id, QStringLiteral("pending"));
            qWarning() << "Networkclient: photo.header 发送失败";
            return clientMsgId;
        }

        // 2c. 原始 JPEG 字节（无前缀）
        if (!m_writer->sendRawBytes(photoJpeg)) {
            outbox.markState(stored.id, QStringLiteral("pending"));
            qWarning() << "Networkclient: 照片原始字节发送失败";
            return clientMsgId;
        }
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
        qDebug() << "Networkclient: 收到服务器心跳响应";
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

        // 处理内存队列中积压的消息
        processQueue();

        // 处理 outbox 中的待发送记录
        processOutbox();

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
    , m_outboxRetryTimer(new QTimer(this))
{
    m_outboxRetryTimer->setSingleShot(true);
    connect(m_outboxRetryTimer, &QTimer::timeout, this, &Networkclient::onOutboxRetryTick);

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
    return m_writer->send(message);
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
        if (r.retryCount >= kMaxRetryCount && r.state != QLatin1String("dead")) {
            outbox.markDead(r.id, QStringLiteral("max retry %1 exceeded").arg(kMaxRetryCount));
            qWarning() << "Networkclient: outbox 记录标记 dead, msgId=" << r.clientMsgId;
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

    if (code == ServerProtocol::kCodeOk) {
        // ---------- 成功：删除 outbox 记录 ----------
        outbox.remove(record.id);
        qDebug() << "Networkclient: 考勤上报成功, 已删除 outbox 记录, employeeId="
                 << record.employeeId;

        emit attendanceReportResult(record.employeeId, true, QString());
        emit uploadFinished(true, QStringLiteral("ok"));

        // 重置退避轮次
        m_outboxRetryRound = 0;

    } else if (code == ServerProtocol::kCodeEmployeeNotFound) {
        // ---------- 4011：员工不存在 → dead ----------
        outbox.markDead(record.id, QStringLiteral("employee not found (4011)"));
        qWarning() << "Networkclient: 员工不存在, 标记 dead, employeeId=" << record.employeeId;

        emit attendanceReportResult(record.employeeId, false, QStringLiteral("employee not found"));
        emit uploadFinished(false, QStringLiteral("employee not found"));

    } else {
        // ---------- 其他错误 (6002 等)：重试 ----------
        outbox.incrementRetry(record.id,
                              QStringLiteral("code=%1 msg=%2")
                                  .arg(code)
                                  .arg(message.value(QStringLiteral("msg")).toString()));

        const int newRetryCount = record.retryCount + 1;
        if (newRetryCount >= kMaxRetryCount) {
            outbox.markDead(record.id, QStringLiteral("max retry after error"));
            qWarning() << "Networkclient: 达到最大重试次数, 标记 dead, employeeId="
                       << record.employeeId;
        } else {
            outbox.markState(record.id, QStringLiteral("failed"),
                             QStringLiteral("code=%1").arg(code));
            qWarning() << "Networkclient: 考勤上报失败, 将重试, retry=" << newRetryCount;
        }

        emit attendanceReportResult(record.employeeId, false,
                                    QStringLiteral("code=%1").arg(code));
        emit uploadFinished(false, message.value(QStringLiteral("msg")).toString());

        // 启动退避重试定时器
        if (!m_outboxRetryTimer->isActive()) {
            m_outboxRetryRound++;
            int delay = kRetryBackoffBaseMs * (1 << qMin(m_outboxRetryRound, 4));
            m_outboxRetryTimer->start(delay);
            qDebug() << "Networkclient: outbox 退避重试 scheduled, delay=" << delay << "ms";
        }
    }
}

void Networkclient::handleServerError(const QJsonObject &message)
{
    const QString inReplyTo = message.value(QStringLiteral("inReplyTo")).toString();
    int code = message.value(QStringLiteral("code")).toInt(-1);

    qWarning() << "Networkclient: server error, code=" << code
               << "msg=" << message.value(ServerProtocol::kMessage).toString();

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
                if (record.retryCount + 1 >= kMaxRetryCount) {
                    outbox.markDead(record.id, QStringLiteral("max retry after error"));
                } else {
                    outbox.markState(record.id, QStringLiteral("failed"));
                }
            }
        }
    }

    emit uploadFinished(false, message.value(ServerProtocol::kMessage).toString());
}
