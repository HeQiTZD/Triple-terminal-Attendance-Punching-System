#include "SyncManager.h"

#include "../NetworkClient/serverprotocol.h"

SyncManager::SyncManager(QObject *parent)
    : QObject(parent)
{
}

void SyncManager::setState(State s)
{
    if (m_state == s) return;
    m_state = s;
}

void SyncManager::requestSync()
{
    if (m_state != State::Idle) {
        return;
    }

    SyncMetaRepository &syncMeta = LocalStorage::instance()->syncMeta();
    const SyncMeta meta = syncMeta.get();

    m_stagingGeneration = meta.currentGeneration + 1;
    m_syncRequestMsgId  = ServerProtocol::generateMsgId();
    m_personCount       = 0;
    m_faceCount         = 0;
    m_faceWritten       = 0;

    if (!syncMeta.beginStaging(m_stagingGeneration, m_syncRequestMsgId)) {
        emit syncFailed(QStringLiteral("beginStaging failed"));
        return;
    }

    setState(State::SyncingPersons);
    emit syncStarted();

    const QJsonObject msg = ServerProtocol::buildSyncRequest(
        LocalStorage::instance()->deviceLocal().get().deviceId);

    emit sendMessage(msg);
}

void SyncManager::handlePersonSync(const QJsonObject &message)
{
    if (m_state != State::SyncingPersons && m_state != State::Idle) {
        return;
    }

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

    emit personSyncProgress(m_personCount);
}

void SyncManager::handleFaceSyncBegin(const QJsonObject &message)
{
    Q_UNUSED(message);

    if (m_state != State::SyncingPersons) {
        return;
    }

    m_faceCount = 0;
    m_faceWritten = 0;
    setState(State::SyncingFaces);
}

void SyncManager::handleFaceItem(const QJsonObject &header, const QByteArray &payload)
{
    if (m_state != State::SyncingFaces) {
        return;
    }

    const QJsonObject itemData = header.value(QStringLiteral("data")).toObject();
    if (itemData.isEmpty()) {
        return;
    }
    const QString employeeId  = itemData.value(QStringLiteral("employeeId")).toString();
    const int     featureSize = itemData.value(QStringLiteral("featureSize")).toInt();

    if (employeeId.isEmpty()) {
        return;
    }
    if (payload.size() != featureSize) {
        return;
    }
    if (featureSize <= 0 || payload.isEmpty()) {
        return;
    }

    FaceFeatureRepository &faceRepo = LocalStorage::instance()->faceFeatures();
    faceRepo.insertOrReplace(employeeId, payload, featureSize, m_stagingGeneration);

    m_faceWritten++;
    m_faceCount++;
    emit faceSyncProgress(m_faceWritten);
}

void SyncManager::handleFaceSyncEnd(const QJsonObject &message)
{
    Q_UNUSED(message);

    if (m_state != State::SyncingFaces) {
        return;
    }

    setState(State::Committing);
    doCommit();
}

void SyncManager::doCommit()
{
    SyncMetaRepository &syncMeta = LocalStorage::instance()->syncMeta();

    int newGeneration = 0;
    if (!syncMeta.commitGeneration(newGeneration, m_faceWritten)) {
        sendSyncAck(QStringLiteral("failed"), QStringLiteral("generation commit failed"));
        setState(State::Idle);
        emit syncFailed(QStringLiteral("generation commit failed"));
        return;
    }

    sendSyncAck(QStringLiteral("ok"));

    emit requestFaceDbReload();

    setState(State::Idle);
    emit syncCompleted();
}

void SyncManager::sendSyncAck(const QString &status, const QString &message)
{
    const QString deviceId = LocalStorage::instance()->deviceLocal().get().deviceId;

    ServerProtocol::Envelope env = ServerProtocol::buildEnvelope(
        ServerProtocol::kTypeSyncAck,
        deviceId,
        {},
        m_syncRequestMsgId);

    QJsonObject data;
    data[QStringLiteral("status")]  = status;
    if (!message.isEmpty())
        data[QStringLiteral("message")] = message;
    env.data = data;

    emit sendMessage(env.toJson());
}
