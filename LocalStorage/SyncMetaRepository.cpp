#include "SyncMetaRepository.h"
#include "../Utils/DatabaseManager.h"
#include <QSqlError>
#include <QSqlQuery>

SyncMetaRepository::SyncMetaRepository(const QString &dbPath)
    : m_dbPath(dbPath)
{
}

void SyncMetaRepository::ensureRow()
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.exec("INSERT OR IGNORE INTO sync_meta (id) VALUES (1)");
}

SyncMeta SyncMetaRepository::get()
{
    SyncMeta m;
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("SELECT current_generation, staging_generation, last_sync_request_msg_id, "
                  "last_sync_ok_at, last_sync_status, face_count FROM sync_meta WHERE id = 1");

    if (query.exec() && query.next()) {
        m.currentGeneration     = query.value(0).toInt();
        m.stagingGeneration     = query.value(1).toInt();
        m.lastSyncRequestMsgId  = query.value(2).toString();
        m.lastSyncOkAt          = query.value(3).toString();
        m.lastSyncStatus        = query.value(4).toString();
        m.faceCount             = query.value(5).toInt();
    }
    return m;
}

bool SyncMetaRepository::beginStaging(int stagingGeneration, const QString &requestMsgId)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE sync_meta SET staging_generation = :staging, "
                  "last_sync_request_msg_id = :msg, last_sync_status = 'syncing' WHERE id = 1");
    query.bindValue(":staging", stagingGeneration);
    query.bindValue(":msg",     requestMsgId);

    if (!query.exec()) {
        return false;
    }
    return true;
}

bool SyncMetaRepository::commitGeneration(int &outCurrentGeneration, int faceCount)
{
    SyncMeta meta = get();
    int staging = meta.stagingGeneration;
    if (staging <= 0) {
        return false;
    }

    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery delQuery(db);
    delQuery.prepare("DELETE FROM face_feature WHERE sync_generation != :staging");
    delQuery.bindValue(":staging", staging);
    if (!delQuery.exec()) {
        return false;
    }

    QSqlQuery updQuery(db);
    updQuery.prepare("UPDATE sync_meta SET current_generation = :curr, "
                     "staging_generation = 0, face_count = :fc, "
                     "last_sync_ok_at = datetime('now'), last_sync_status = 'ok' WHERE id = 1");
    updQuery.bindValue(":curr", staging);
    updQuery.bindValue(":fc",   faceCount);

    if (!updQuery.exec()) {
        return false;
    }

    outCurrentGeneration = staging;
    return true;
}

bool SyncMetaRepository::updateStatus(const QString &status)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE sync_meta SET last_sync_status = :st WHERE id = 1");
    query.bindValue(":st", status);
    if (!query.exec()) {
        return false;
    }
    return true;
}

bool SyncMetaRepository::updateFaceCount(int count)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE sync_meta SET face_count = :fc WHERE id = 1");
    query.bindValue(":fc", count);
    if (!query.exec()) {
        return false;
    }
    return true;
}
