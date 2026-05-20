#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QString>
#include <QVector>

struct OutboxRecord {
    int     id = 0;
    QString clientMsgId;
    QString employeeId;
    QString checkTime;
    QString status;
    QByteArray photoBlob;
    int     photoSize = 0;
    int     retryCount = 0;
    QString lastError;
    QString state; // pending / sending / failed / dead
};

class AttendanceOutboxRepository {
public:
    explicit AttendanceOutboxRepository(const QString &dbPath);

    bool enqueue(const OutboxRecord &record);

    QVector<OutboxRecord> fetchPending(int limit = 50);

    bool markState(int id, const QString &state, const QString &lastError = {});

    bool incrementRetry(int id, const QString &lastError);

    bool markDead(int id, const QString &reason);

    bool remove(int id);

    bool removeByClientMsgId(const QString &clientMsgId);
    OutboxRecord findByClientMsgId(const QString &clientMsgId);

    int pendingCount();

private:
    QString m_dbPath;
};
