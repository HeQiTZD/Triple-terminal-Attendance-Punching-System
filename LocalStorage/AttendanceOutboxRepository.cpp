#include "AttendanceOutboxRepository.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

AttendanceOutboxRepository::AttendanceOutboxRepository(QSqlDatabase &db)
    : m_db(db)
{
}

bool AttendanceOutboxRepository::enqueue(const OutboxRecord &record)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO attendance_outbox "
                  "(client_msg_id, employee_id, check_time, status, photo_blob, photo_size, state) "
                  "VALUES (:cid, :eid, :time, :status, :photo, :psize, :state)");
    query.bindValue(":cid",   record.clientMsgId);
    query.bindValue(":eid",   record.employeeId);
    query.bindValue(":time",  record.checkTime);
    query.bindValue(":status", record.status);
    query.bindValue(":photo", record.photoBlob);
    query.bindValue(":psize", record.photoSize);
    query.bindValue(":state", record.state.isEmpty() ? QStringLiteral("pending") : record.state);

    if (!query.exec()) {
        qWarning() << "AttendanceOutboxRepository enqueue failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QVector<OutboxRecord> AttendanceOutboxRepository::fetchPending(int limit)
{
    QVector<OutboxRecord> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, client_msg_id, employee_id, check_time, status, "
                  "photo_blob, photo_size, retry_count, last_error, state "
                  "FROM attendance_outbox WHERE state IN ('pending','failed') "
                  "ORDER BY created_at ASC LIMIT :lim");
    query.bindValue(":lim", limit);

    if (!query.exec()) {
        qWarning() << "AttendanceOutboxRepository fetchPending failed:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        OutboxRecord r;
        r.id          = query.value(0).toInt();
        r.clientMsgId = query.value(1).toString();
        r.employeeId  = query.value(2).toString();
        r.checkTime   = query.value(3).toString();
        r.status      = query.value(4).toString();
        r.photoBlob   = query.value(5).toByteArray();
        r.photoSize   = query.value(6).toInt();
        r.retryCount  = query.value(7).toInt();
        r.lastError   = query.value(8).toString();
        r.state       = query.value(9).toString();
        result.append(r);
    }
    return result;
}

bool AttendanceOutboxRepository::markState(int id, const QString &state,
                                           const QString &lastError)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE attendance_outbox SET state = :st, last_error = :err WHERE id = :id");
    query.bindValue(":st",  state);
    query.bindValue(":err", lastError);
    query.bindValue(":id",  id);

    if (!query.exec()) {
        qWarning() << "AttendanceOutboxRepository markState failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool AttendanceOutboxRepository::incrementRetry(int id, const QString &lastError)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE attendance_outbox SET retry_count = retry_count + 1, "
                  "last_error = :err WHERE id = :id");
    query.bindValue(":err", lastError);
    query.bindValue(":id",  id);

    if (!query.exec()) {
        qWarning() << "AttendanceOutboxRepository incrementRetry failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool AttendanceOutboxRepository::markDead(int id, const QString &reason)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE attendance_outbox SET state = 'dead', last_error = :err WHERE id = :id");
    query.bindValue(":err", reason);
    query.bindValue(":id",  id);

    if (!query.exec()) {
        qWarning() << "AttendanceOutboxRepository markDead failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool AttendanceOutboxRepository::remove(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM attendance_outbox WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "AttendanceOutboxRepository remove failed:" << query.lastError().text();
        return false;
    }
    return true;
}

int AttendanceOutboxRepository::pendingCount()
{
    QSqlQuery query(m_db);
    if (query.exec("SELECT COUNT(*) FROM attendance_outbox WHERE state IN ('pending','failed')")
        && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
