#pragma once

#include <QDateTime>
#include <QSqlDatabase>
#include <QString>

struct SyncMeta {
    int     currentGeneration = 1;
    int     stagingGeneration = 0;
    QString lastSyncRequestMsgId;
    QString lastSyncOkAt;
    QString lastSyncStatus;
    int     faceCount = 0;
};

class SyncMetaRepository {
public:
    explicit SyncMetaRepository(QSqlDatabase &db);

    void ensureRow();

    SyncMeta get();

    bool beginStaging(int stagingGeneration, const QString &requestMsgId);

    bool commitGeneration(int &outCurrentGeneration, int faceCount);

    bool updateStatus(const QString &status);

    bool updateFaceCount(int count);

private:
    QSqlDatabase &m_db;
};
