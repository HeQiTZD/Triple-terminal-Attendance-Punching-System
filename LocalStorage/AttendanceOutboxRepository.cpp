#include "AttendanceOutboxRepository.h"
#include "../Utils/DatabaseManager.h"
#include <QSqlError>
#include <QSqlQuery>

AttendanceOutboxRepository::AttendanceOutboxRepository(const QString &dbPath)
    : m_dbPath(dbPath)
{
}

bool AttendanceOutboxRepository::enqueue(const OutboxRecord &record)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
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
        return false;
    }
    return true;
}

QVector<OutboxRecord> AttendanceOutboxRepository::fetchPending(int limit)
{
    QVector<OutboxRecord> result;
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("SELECT id, client_msg_id, employee_id, check_time, status, "
                  "photo_blob, photo_size, retry_count, last_error, state "
                  "FROM attendance_outbox WHERE state IN ('pending','failed') "
                  "ORDER BY created_at ASC LIMIT :lim");
    query.bindValue(":lim", limit);

    if (!query.exec()) {
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
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE attendance_outbox SET state = :st, last_error = :err WHERE id = :id");
    query.bindValue(":st",  state);
    query.bindValue(":err", lastError);
    query.bindValue(":id",  id);

    if (!query.exec()) {
        return false;
    }
    return true;
}

bool AttendanceOutboxRepository::incrementRetry(int id, const QString &lastError)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE attendance_outbox SET retry_count = retry_count + 1, "
                  "last_error = :err WHERE id = :id");
    query.bindValue(":err", lastError);
    query.bindValue(":id",  id);

    if (!query.exec()) {
        return false;
    }
    return true;
}

bool AttendanceOutboxRepository::markDead(int id, const QString &reason)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE attendance_outbox SET state = 'dead', last_error = :err WHERE id = :id");
    query.bindValue(":err", reason);
    query.bindValue(":id",  id);

    if (!query.exec()) {
        return false;
    }
    return true;
}

bool AttendanceOutboxRepository::remove(int id)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("DELETE FROM attendance_outbox WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        return false;
    }
    return true;
}

bool AttendanceOutboxRepository::removeByClientMsgId(const QString &clientMsgId)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("DELETE FROM attendance_outbox WHERE client_msg_id = :cid");
    query.bindValue(":cid", clientMsgId);

    if (!query.exec()) {
        return false;
    }
    return true;
}

OutboxRecord AttendanceOutboxRepository::findByClientMsgId(const QString &clientMsgId)
{
    OutboxRecord r;
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("SELECT id, client_msg_id, employee_id, check_time, status, "
                  "photo_blob, photo_size, retry_count, last_error, state "
                  "FROM attendance_outbox WHERE client_msg_id = :cid");
    query.bindValue(":cid", clientMsgId);

    if (query.exec() && query.next()) {
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
    }
    return r;
}

OutboxRecord AttendanceOutboxRepository::findLatestByEmployeeId(const QString &employeeId)
{
    OutboxRecord r;
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("SELECT id, client_msg_id, employee_id, check_time, status, "
                  "photo_blob, photo_size, retry_count, last_error, state "
                  "FROM attendance_outbox WHERE employee_id = :eid "
                  "ORDER BY datetime(check_time) DESC, id DESC LIMIT 1");
    query.bindValue(":eid", employeeId);

    if (query.exec() && query.next()) {
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
    }
    return r;
}

int AttendanceOutboxRepository::pendingCount()
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    if (query.exec("SELECT COUNT(*) FROM attendance_outbox WHERE state IN ('pending','failed')")
        && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
