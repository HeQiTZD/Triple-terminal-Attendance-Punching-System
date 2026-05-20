#include "SyncManager.h"

#include <QDateTime>
#include <QDebug>

#include "../NetworkClient/serverprotocol.h"

SyncManager::SyncManager(QObject *parent)
    : QObject(parent)
{
}

// ---------------------------------------------------------------------------
// State management
// ---------------------------------------------------------------------------

void SyncManager::setState(State s)
{
    if (m_state == s) return;
    qDebug() << "SyncManager: state" << static_cast<int>(m_state) << "→" << static_cast<int>(s);
    m_state = s;
}

// ---------------------------------------------------------------------------
// requestSync — 发起同步
// ---------------------------------------------------------------------------

void SyncManager::requestSync()
{
    if (m_state != State::Idle) {
        qWarning() << "SyncManager: 同步已在进行中, 拒绝重复请求";
        return;
    }

    SyncMetaRepository &syncMeta = LocalStorage::instance()->syncMeta();
    const SyncMeta meta = syncMeta.get();

    // staging_generation = current_generation + 1
    m_stagingGeneration = meta.currentGeneration + 1;
    m_syncRequestMsgId  = ServerProtocol::generateMsgId();
    m_personCount       = 0;
    m_faceCount         = 0;
    m_faceWritten       = 0;

    // 标记 staging 开始
    if (!syncMeta.beginStaging(m_stagingGeneration, m_syncRequestMsgId)) {
        qWarning() << "SyncManager: beginStaging 失败";
        emit syncFailed(QStringLiteral("beginStaging failed"));
        return;
    }

    setState(State::SyncingPersons);
    emit syncStarted();

    // 发送 sync.request
    const QJsonObject msg = ServerProtocol::buildSyncRequest(
        LocalStorage::instance()->deviceLocal().get().deviceId);

    qDebug() << "SyncManager: 发送 sync.request, msgId=" << m_syncRequestMsgId
             << "stagingGeneration=" << m_stagingGeneration;

    emit sendMessage(msg);
}

// ---------------------------------------------------------------------------
// handlePersonSync — 接收人员数据
// ---------------------------------------------------------------------------

void SyncManager::handlePersonSync(const QJsonObject &message)
{
    if (m_state != State::SyncingPersons && m_state != State::Idle) {
        qWarning() << "SyncManager: 非预期状态收到 person.sync, state=" << static_cast<int>(m_state);
        return;
    }

    // 如果还在 Idle（未通过 requestSync 发起，而是被动收到），直接进入
    if (m_state == State::Idle) {
        m_stagingGeneration = LocalStorage::instance()->syncMeta().get().currentGeneration + 1;
        m_syncRequestMsgId = message.value(QStringLiteral("inReplyTo")).toString();
        m_faceCount = 0;
        m_faceWritten = 0;
        setState(State::SyncingPersons);
        emit syncStarted();
    }

    const auto persons = ServerProtocol::parsePersons(message);
    m_personCount += persons.size();

    qDebug() << "SyncManager: 收到 person.sync, 本批" << persons.size()
             << "条, 累计" << m_personCount << "条";

    emit personSyncProgress(m_personCount);
}

// ---------------------------------------------------------------------------
// handleFaceSyncBegin — 人脸同步开始
// ---------------------------------------------------------------------------

void SyncManager::handleFaceSyncBegin(const QJsonObject &message)
{
    Q_UNUSED(message);

    if (m_state != State::SyncingPersons) {
        qWarning() << "SyncManager: 非 SyncingPersons 状态收到 face.sync.begin";
        return;
    }

    m_faceCount = 0;
    m_faceWritten = 0;
    setState(State::SyncingFaces);

    qDebug() << "SyncManager: 人脸同步阶段开始";
}

// ---------------------------------------------------------------------------
// handleFaceItem — 接收单条人脸特征（header + binary）
// ---------------------------------------------------------------------------

void SyncManager::handleFaceItem(const QJsonObject &header, const QByteArray &payload)
{
    if (m_state != State::SyncingFaces) {
        qWarning() << "SyncManager: 非 SyncingFaces 状态收到 face.sync.item";
        return;
    }

    // 提取元数据（从 data 子对象中读取）
    const QJsonObject itemData = header.value(QStringLiteral("data")).toObject();
    if (itemData.isEmpty()) {
        qWarning() << "SyncManager: face.sync.item.header data 为空";
        return;
    }
    const QString employeeId  = itemData.value(QStringLiteral("employeeId")).toString();
    const int     featureSize = itemData.value(QStringLiteral("featureSize")).toInt();

    // 校验
    if (employeeId.isEmpty()) {
        qWarning() << "SyncManager: face.sync.item.header 缺少 employeeId, 跳过";
        return;
    }
    if (payload.size() != featureSize) {
        qWarning() << "SyncManager: feature_size 不匹配, employeeId=" << employeeId
                   << "expected" << featureSize << "got" << payload.size() << ", 跳过";
        return;
    }
    if (featureSize <= 0 || payload.isEmpty()) {
        qWarning() << "SyncManager: 空特征, employeeId=" << employeeId << ", 跳过";
        return;
    }

    // 写入 staging 代次
    FaceFeatureRepository &faceRepo = LocalStorage::instance()->faceFeatures();
    if (!faceRepo.insertOrReplace(employeeId, payload, featureSize, m_stagingGeneration)) {
        qWarning() << "SyncManager: 写入特征失败, employeeId=" << employeeId;
        // 不中断，继续处理后续条目
    }

    m_faceWritten++;
    m_faceCount++;
    emit faceSyncProgress(m_faceWritten);
}

// ---------------------------------------------------------------------------
// handleFaceSyncEnd — 人脸同步结束，提交代次
// ---------------------------------------------------------------------------

void SyncManager::handleFaceSyncEnd(const QJsonObject &message)
{
    Q_UNUSED(message);

    if (m_state != State::SyncingFaces) {
        qWarning() << "SyncManager: 非 SyncingFaces 状态收到 face.sync.end";
        return;
    }

    qDebug() << "SyncManager: 人脸同步结束, 共收到" << m_faceCount
             << "条, 成功写入" << m_faceWritten << "条";

    setState(State::Committing);
    doCommit();
}

// ---------------------------------------------------------------------------
// doCommit — 单事务代次切换
// ---------------------------------------------------------------------------

void SyncManager::doCommit()
{
    SyncMetaRepository &syncMeta = LocalStorage::instance()->syncMeta();

    // commitGeneration 内部完成: 删除旧代次 → 切换 current_generation → 更新 face_count
    int newGeneration = 0;
    if (!syncMeta.commitGeneration(newGeneration, m_faceWritten)) {
        qWarning() << "SyncManager: 代次切换失败";
        sendSyncAck(QStringLiteral("failed"), QStringLiteral("generation commit failed"));
        setState(State::Idle);
        emit syncFailed(QStringLiteral("generation commit failed"));
        return;
    }

    qDebug() << "SyncManager: 代次切换成功, new generation=" << newGeneration
             << "face count=" << m_faceWritten;

    // 发送 sync.ack (ok)
    sendSyncAck(QStringLiteral("ok"));

    // 通知人脸库重载
    emit requestFaceDbReload();

    setState(State::Idle);
    emit syncCompleted();
}

// ---------------------------------------------------------------------------
// sendSyncAck
// ---------------------------------------------------------------------------

void SyncManager::sendSyncAck(const QString &status, const QString &message)
{
    const QString deviceId = LocalStorage::instance()->deviceLocal().get().deviceId;

    ServerProtocol::Envelope env = ServerProtocol::buildEnvelope(
        ServerProtocol::kTypeSyncAck,
        deviceId,
        {},  // data filled below
        m_syncRequestMsgId);

    QJsonObject data;
    data[QStringLiteral("status")]  = status;
    if (!message.isEmpty())
        data[QStringLiteral("message")] = message;
    env.data = data;

    qDebug() << "SyncManager: 发送 sync.ack, status=" << status << "msg=" << message;
    emit sendMessage(env.toJson());
}
