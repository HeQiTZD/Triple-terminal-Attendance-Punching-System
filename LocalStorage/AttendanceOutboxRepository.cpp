#include "AttendanceOutboxRepository.h"

AttendanceOutboxRepository::AttendanceOutboxRepository(const QString &dbPath)
    : BaseRepository(dbPath)
{
}

bool AttendanceOutboxRepository::enqueue(const OutboxRecord &record)
{
    return executeNonQuery(
        "INSERT INTO attendance_outbox "
        "(client_msg_id, employee_id, check_time, status, photo_blob, photo_size, state) "
        "VALUES (:cid, :eid, :time, :status, :photo, :psize, :state)",
        {{":cid", record.clientMsgId},
         {":eid", record.employeeId},
         {":time", record.checkTime},
         {":status", record.status},
         {":photo", record.photoBlob},
         {":psize", record.photoSize},
         {":state", record.state.isEmpty() ? QStringLiteral("pending") : record.state}}
    );
}

QVector<OutboxRecord> AttendanceOutboxRepository::fetchPending(int limit)
{
    QVector<OutboxRecord> result;
    executeReader(
        "SELECT id, client_msg_id, employee_id, check_time, status, "
        "photo_blob, photo_size, retry_count, last_error, state "
        "FROM attendance_outbox WHERE state IN ('pending','failed') "
        "ORDER BY created_at ASC LIMIT :lim",
        {{":lim", limit}},
        [&](QSqlQuery &q) {
            OutboxRecord r;
            r.id          = q.value(0).toInt();
            r.clientMsgId = q.value(1).toString();
            r.employeeId  = q.value(2).toString();
            r.checkTime   = q.value(3).toString();
            r.status      = q.value(4).toString();
            r.photoBlob   = q.value(5).toByteArray();
            r.photoSize   = q.value(6).toInt();
            r.retryCount  = q.value(7).toInt();
            r.lastError   = q.value(8).toString();
            r.state       = q.value(9).toString();
            result.append(r);
        });
    return result;
}

bool AttendanceOutboxRepository::markState(int id, const QString &state,
                                           const QString &lastError)
{
    return executeNonQuery(
        "UPDATE attendance_outbox SET state = :st, last_error = :err WHERE id = :id",
        {{":st", state}, {":err", lastError}, {":id", id}}
    );
}

bool AttendanceOutboxRepository::incrementRetry(int id, const QString &lastError)
{
    return executeNonQuery(
        "UPDATE attendance_outbox SET retry_count = retry_count + 1, "
        "last_error = :err WHERE id = :id",
        {{":err", lastError}, {":id", id}}
    );
}

bool AttendanceOutboxRepository::markDead(int id, const QString &reason)
{
    return executeNonQuery(
        "UPDATE attendance_outbox SET state = 'dead', last_error = :err WHERE id = :id",
        {{":err", reason}, {":id", id}}
    );
}

bool AttendanceOutboxRepository::remove(int id)
{
    return executeNonQuery(
        "DELETE FROM attendance_outbox WHERE id = :id",
        {{":id", id}}
    );
}

bool AttendanceOutboxRepository::removeByClientMsgId(const QString &clientMsgId)
{
    return executeNonQuery(
        "DELETE FROM attendance_outbox WHERE client_msg_id = :cid",
        {{":cid", clientMsgId}}
    );
}

OutboxRecord AttendanceOutboxRepository::findByClientMsgId(const QString &clientMsgId)
{
    OutboxRecord r;
    executeReader(
        "SELECT id, client_msg_id, employee_id, check_time, status, "
        "photo_blob, photo_size, retry_count, last_error, state "
        "FROM attendance_outbox WHERE client_msg_id = :cid",
        {{":cid", clientMsgId}},
        [&](QSqlQuery &q) {
            r.id          = q.value(0).toInt();
            r.clientMsgId = q.value(1).toString();
            r.employeeId  = q.value(2).toString();
            r.checkTime   = q.value(3).toString();
            r.status      = q.value(4).toString();
            r.photoBlob   = q.value(5).toByteArray();
            r.photoSize   = q.value(6).toInt();
            r.retryCount  = q.value(7).toInt();
            r.lastError   = q.value(8).toString();
            r.state       = q.value(9).toString();
        });
    return r;
}

OutboxRecord AttendanceOutboxRepository::findLatestByEmployeeId(const QString &employeeId)
{
    OutboxRecord r;
    executeReader(
        "SELECT id, client_msg_id, employee_id, check_time, status, "
        "photo_blob, photo_size, retry_count, last_error, state "
        "FROM attendance_outbox WHERE employee_id = :eid "
        "ORDER BY datetime(check_time) DESC, id DESC LIMIT 1",
        {{":eid", employeeId}},
        [&](QSqlQuery &q) {
            r.id          = q.value(0).toInt();
            r.clientMsgId = q.value(1).toString();
            r.employeeId  = q.value(2).toString();
            r.checkTime   = q.value(3).toString();
            r.status      = q.value(4).toString();
            r.photoBlob   = q.value(5).toByteArray();
            r.photoSize   = q.value(6).toInt();
            r.retryCount  = q.value(7).toInt();
            r.lastError   = q.value(8).toString();
            r.state       = q.value(9).toString();
        });
    return r;
}

int AttendanceOutboxRepository::pendingCount()
{
    return executeScalar(
        "SELECT COUNT(*) FROM attendance_outbox WHERE state IN ('pending','failed')",
        {},
        0
    ).toInt();
}
