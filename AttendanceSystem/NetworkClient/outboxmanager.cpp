#include "outboxmanager.h"
#include "messagewriter.h"
#include "serverprotocol.h"
#include "../LocalStorage/localstorage.h"
#include "../Utils/Logger.h"

#include <QUuid>

OutboxManager::OutboxManager(QObject *parent)
    : QObject(parent)
    , m_retryTimer(new QTimer(this))
{
    m_retryTimer->setSingleShot(true);
    connect(m_retryTimer, &QTimer::timeout, this, &OutboxManager::onRetryTick);
}

QString OutboxManager::enqueue(const QString &employeeId, const QString &status,
                                const QDateTime &checkTime)
{
    const QString clientMsgId = ServerProtocol::generateMsgId();

    OutboxRecord record;
    record.clientMsgId = clientMsgId;
    record.employeeId  = employeeId;
    record.checkTime   = checkTime.isValid() ? checkTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
                                             : QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    record.status      = status;
    record.state       = QStringLiteral("pending");

    AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
    if (!outbox.enqueue(record)) {
        return clientMsgId;
    }

    emit pendingCountChanged(pendingCount());
    return clientMsgId;
}

QString OutboxManager::enqueueWithPhoto(const QString &employeeId, const QString &status,
                                         const QByteArray &photoJpeg, const QDateTime &checkTime)
{
    const QString clientMsgId = ServerProtocol::generateMsgId();

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
        return clientMsgId;
    }

    emit pendingCountChanged(pendingCount());
    return clientMsgId;
}

void OutboxManager::processOutbox(MessageWriter *writer, const QString &deviceId)
{
    if (!writer) {
        return;
    }

    // 缓存 writer 和 deviceId 用于定时器触发的重试
    m_cachedWriter = writer;
    m_cachedDeviceId = deviceId;

    AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();

    // 先将超过最大重试次数的记录标记为 dead
    auto allPending = outbox.fetchPending(200);
    for (const auto &r : allPending) {
        if (r.retryCount >= m_maxRetryCount && r.state != QLatin1String("dead")) {
            outbox.markDead(r.id, QStringLiteral("max retry %1 exceeded").arg(m_maxRetryCount));
            LOG_ATT(Logger::Warn, "记录标记为死信", QStringLiteral("msgId=%1 | 原因=达到最大重试次数").arg(r.clientMsgId));
        }
    }

    // 获取待发送记录（pending + failed，不含 dead）
    auto records = outbox.fetchPending(50);
    if (records.isEmpty()) {
        m_retryRound = 0;
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
            deviceId,
            r.status,
            /*awaitPhoto=*/false,
            r.clientMsgId);  // 复用原 msgId，保证幂等

        writer->send(msg);
        outbox.markState(r.id, QStringLiteral("sending"));
    }

    // 带照片记录：逐个发送完整 awaitPhoto 流程
    for (const auto &r : photoRecords) {
        // report (awaitPhoto=true)
        QJsonObject reportMsg = ServerProtocol::buildAttendanceReport(
            r.employeeId,
            QDateTime::fromString(r.checkTime, QStringLiteral("yyyy-MM-dd HH:mm:ss")),
            deviceId,
            r.status,
            true,
            r.clientMsgId);

        writer->send(reportMsg);

        // photo.header
        QJsonObject headerMsg = ServerProtocol::buildAttendancePhotoHeader(
            deviceId, r.employeeId, r.photoSize);

        writer->send(headerMsg);

        // raw photo bytes
        writer->sendRawBytes(r.photoBlob);

        outbox.markState(r.id, QStringLiteral("sending"));
    }
}

void OutboxManager::handleUploadResponse(const QJsonObject &message)
{
    // 提取 inReplyTo（对应考勤上报的 client_msg_id）
    const QString inReplyTo = message.value(QStringLiteral("inReplyTo")).toString();
    int code = message.value(QStringLiteral("code")).toInt(-1);

    if (code == -1) {
        const QJsonObject d = message.value(QStringLiteral("data")).toObject();
        code = d.value(QStringLiteral("code")).toInt(0);
    }

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
        bool success = (code == ServerProtocol::kCodeOk);
        emit uploadFinished(success, message.value(QStringLiteral("msg")).toString());
        return;
    }

    if (code == ServerProtocol::kCodeOk) {
        // ---------- 成功：删除 outbox 记录 ----------
        outbox.remove(record.id);
        LOG_ATT(Logger::Info, "考勤上报成功", QStringLiteral("employeeId=%1 | msgId=%2").arg(record.employeeId, inReplyTo));

        emit attendanceReportResult(record.employeeId, true, QString());
        emit uploadFinished(true, QStringLiteral("ok"));

        // 重置退避轮次
        m_retryRound = 0;

    } else if (code == ServerProtocol::kCodeEmployeeNotFound) {
        // ---------- 4011：员工不存在 → dead ----------
        outbox.markDead(record.id, QStringLiteral("employee not found (4011)"));
        LOG_ATT(Logger::Error, "考勤上报失败", QStringLiteral("employeeId=%1 | msgId=%2 | 原因=员工不存在").arg(record.employeeId, inReplyTo));

        emit attendanceReportResult(record.employeeId, false, QStringLiteral("employee not found"));
        emit uploadFinished(false, QStringLiteral("employee not found"));

    } else {
        // ---------- 其他错误 (6002 等)：重试 ----------
        outbox.incrementRetry(record.id,
                              QStringLiteral("code=%1 msg=%2")
                                  .arg(code)
                                  .arg(message.value(QStringLiteral("msg")).toString()));

        const int newRetryCount = record.retryCount + 1;
        if (newRetryCount >= m_maxRetryCount) {
            outbox.markDead(record.id, QStringLiteral("max retry after error"));
            LOG_ATT(Logger::Error, "考勤上报失败", QStringLiteral("employeeId=%1 | msgId=%2 | 原因=达到最大重试次数").arg(record.employeeId, inReplyTo));
        } else {
            outbox.markState(record.id, QStringLiteral("failed"),
                             QStringLiteral("code=%1").arg(code));
            LOG_ATT(Logger::Warn, "考勤上报失败", QStringLiteral("employeeId=%1 | msgId=%2 | 错误码=%3 | 重试次数=%4").arg(record.employeeId, inReplyTo).arg(code).arg(newRetryCount));
        }

        emit attendanceReportResult(record.employeeId, false,
                                    QStringLiteral("code=%1").arg(code));
        emit uploadFinished(false, message.value(QStringLiteral("msg")).toString());

        // 启动退避重试定时器
        if (!m_retryTimer->isActive()) {
            m_retryRound++;
            int delay = m_retryBackoffBaseMs * (1 << qMin(m_retryRound, 4));
            m_retryTimer->start(delay);
            LOG_ATT(Logger::Info, "计划退避重试", QStringLiteral("延迟=%1ms").arg(delay));
        }
    }

    emit pendingCountChanged(pendingCount());
}

void OutboxManager::handleServerError(const QJsonObject &message)
{
    const QString inReplyTo = message.value(QStringLiteral("inReplyTo")).toString();
    int code = message.value(QStringLiteral("code")).toInt(-1);

    LOG_NET(Logger::Warn, "服务器错误", QStringLiteral("错误码=%1").arg(code));

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
    emit pendingCountChanged(pendingCount());
}

void OutboxManager::retryAll(MessageWriter *writer, const QString &deviceId)
{
    m_retryRound = 0;
    processOutbox(writer, deviceId);
}

void OutboxManager::rollbackSendingToPending()
{
    AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
    auto pending = outbox.fetchPending(200);
    for (const auto &r : pending) {
        if (r.state == QLatin1String("sending")) {
            outbox.markState(r.id, QStringLiteral("pending"));
        }
    }

    // 停止重试定时器
    m_retryTimer->stop();
    m_retryRound = 0;
}

int OutboxManager::pendingCount() const
{
    AttendanceOutboxRepository &outbox = LocalStorage::instance()->outbox();
    return outbox.fetchPending(9999).size();
}

void OutboxManager::onRetryTick()
{
    if (m_cachedWriter && !m_cachedDeviceId.isEmpty()) {
        processOutbox(m_cachedWriter, m_cachedDeviceId);
    }
}
