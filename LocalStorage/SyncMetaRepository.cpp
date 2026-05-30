#include "SyncMetaRepository.h"

SyncMetaRepository::SyncMetaRepository(const QString &dbPath)
    : BaseRepository(dbPath)
{
}

void SyncMetaRepository::ensureRow()
{
    executeNonQuery("INSERT OR IGNORE INTO sync_meta (id) VALUES (1)");
}

SyncMeta SyncMetaRepository::get()
{
    SyncMeta m;
    executeReader(
        "SELECT current_generation, staging_generation, last_sync_request_msg_id, "
        "last_sync_ok_at, last_sync_status, face_count FROM sync_meta WHERE id = 1",
        {},
        [&](QSqlQuery &q) {
            m.currentGeneration     = q.value(0).toInt();
            m.stagingGeneration     = q.value(1).toInt();
            m.lastSyncRequestMsgId  = q.value(2).toString();
            m.lastSyncOkAt          = q.value(3).toString();
            m.lastSyncStatus        = q.value(4).toString();
            m.faceCount             = q.value(5).toInt();
        });
    return m;
}

bool SyncMetaRepository::beginStaging(int stagingGeneration, const QString &requestMsgId)
{
    return executeNonQuery(
        "UPDATE sync_meta SET staging_generation = :staging, "
        "last_sync_request_msg_id = :msg, last_sync_status = 'syncing' WHERE id = 1",
        {{":staging", stagingGeneration}, {":msg", requestMsgId}}
    );
}

bool SyncMetaRepository::commitGeneration(int &outCurrentGeneration, int faceCount)
{
    SyncMeta meta = get();
    int staging = meta.stagingGeneration;
    if (staging <= 0) {
        return false;
    }

    // 删除旧代数据
    if (!executeNonQuery(
            "DELETE FROM face_feature WHERE sync_generation != :staging",
            {{":staging", staging}})) {
        return false;
    }

    // 更新元数据
    if (!executeNonQuery(
            "UPDATE sync_meta SET current_generation = :curr, "
            "staging_generation = 0, face_count = :fc, "
            "last_sync_ok_at = datetime('now'), last_sync_status = 'ok' WHERE id = 1",
            {{":curr", staging}, {":fc", faceCount}})) {
        return false;
    }

    outCurrentGeneration = staging;
    return true;
}

bool SyncMetaRepository::updateStatus(const QString &status)
{
    return executeNonQuery(
        "UPDATE sync_meta SET last_sync_status = :st WHERE id = 1",
        {{":st", status}}
    );
}

bool SyncMetaRepository::updateFaceCount(int count)
{
    return executeNonQuery(
        "UPDATE sync_meta SET face_count = :fc WHERE id = 1",
        {{":fc", count}}
    );
}
