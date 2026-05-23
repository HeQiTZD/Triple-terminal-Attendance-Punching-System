#include "AttendanceService.h"
#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

using namespace Protocol;

AttendanceService::AttendanceService(QObject* parent) : QObject(parent) {}

void AttendanceService::setTcpManager(TcpConnectionManager* tcp) { m_tcp = tcp; }

void AttendanceService::setBusy(bool v)
{
    if (m_busy == v) return;
    m_busy = v;
    emit busyChanged();
}

bool AttendanceService::hasAnyNonEmptyString(const QJsonObject& o)
{
    for (auto it = o.begin(); it != o.end(); ++it) {
        if (it.value().isString() && !it.value().toString().isEmpty())
            return true;
        if (it.value().isDouble() && it.key() == QStringLiteral("id") && it.value().toInt() != 0)
            return true;
    }
    return false;
}

QJsonObject AttendanceService::filterCriteriaToJson(const QVariantMap& m, bool forNested)
{
    QJsonObject o;
    for (auto it = m.constBegin(); it != m.constEnd(); ++it) {
        const QString key = it.key();
        if (key == QStringLiteral("id")) {
            const QVariant v = it.value();
            if (v.isValid() && (v.typeId() == QMetaType::Int || v.typeId() == QMetaType::LongLong
                || v.typeId() == QMetaType::Double)) {
                const int id = v.toInt();
                if (id > 0)
                    o[key] = id;
            }
            continue;
        }
        const QString s = it.value().toString();
        o[key] = s;
    }
    return o;
}

QVariantList AttendanceService::parseLiveRecords(const QJsonArray& arr)
{
    QVariantList out;
    for (const auto& v : arr) {
        const QJsonObject o = v.toObject();
        QVariantMap row;
        row[QStringLiteral("id")] = o.value(QStringLiteral("id")).toInt();
        row[QStringLiteral("employeeId")] = o.value(kEmployeeId).toString();
        row[QStringLiteral("personName")] = QString(); // 活库响应无姓名
        row[QStringLiteral("checkTime")] = o.value(kCheckTime).toString();
        row[QStringLiteral("deviceId")] = o.value(kDeviceId).toString();
        row[QStringLiteral("status")] = o.value(kStatus).toString();
        row[QStringLiteral("receivedTime")] = o.value(kReceivedTime).toString();
        out.append(row);
    }
    return out;
}

QVariantList AttendanceService::parseArchiveRecords(const QJsonArray& arr)
{
    QVariantList out;
    for (const auto& v : arr) {
        const QJsonObject o = v.toObject();
        QVariantMap row;
        row[QStringLiteral("id")] = o.value(QStringLiteral("id")).toInt();
        row[QStringLiteral("employeeId")] = o.value(kEmployeeId).toString();
        row[QStringLiteral("personName")] = o.value(kName).toString();
        row[QStringLiteral("department")] = o.value(kDepartment).toString();
        row[QStringLiteral("position")] = o.value(kPosition).toString();
        row[QStringLiteral("checkTime")] = o.value(kCheckTime).toString();
        row[QStringLiteral("deviceId")] = o.value(kDeviceId).toString();
        row[QStringLiteral("status")] = o.value(kStatus).toString();
        row[QStringLiteral("receivedTime")] = o.value(kReceivedTime).toString();
        row[QStringLiteral("archivedAt")] = o.value(QStringLiteral("archivedAt")).toString();
        row[QStringLiteral("archiveReason")] = o.value(QStringLiteral("archiveReason")).toString();
        out.append(row);
    }
    return out;
}

void AttendanceService::query(int idFilter,
    const QString& employeeId,
    const QString& checkTime,
    const QString& deviceId,
    const QString& status,
    const QString& receivedTime)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kAttendanceQuery, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    if (idFilter >= 0)
        data[QStringLiteral("id")] = idFilter;
    else
        data[QStringLiteral("id")] = QJsonValue(QJsonValue::Null);

    data[kEmployeeId] = employeeId;
    data[kCheckTime] = checkTime;
    data[kDeviceId] = deviceId;
    data[kStatus] = status;
    data[kReceivedTime] = receivedTime;

    QJsonObject msg;
    msg[kType] = kAttendanceQuery;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kAttendanceQuery, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code != ErrorCode::kSuccess) {
            emit operationFailed(kAttendanceQuery, code, text);
            return;
        }
        const QJsonObject d = resp.value(kData).toObject();
        m_records = parseLiveRecords(d.value(kRecords).toArray());
        emit recordsChanged();
        emit operationSucceeded(kAttendanceQuery, text.isEmpty() ? QStringLiteral("ok") : text);
        });
}

void AttendanceService::createRecord(const QString& employeeId,
    const QString& checkTime,
    const QString& status,
    const QString& deviceId)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kAttendanceCreate, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kEmployeeId] = employeeId;
    data[kCheckTime] = checkTime;
    data[kStatus] = status;
    if (!deviceId.isEmpty())
        data[kDeviceId] = deviceId;

    QJsonObject msg;
    msg[kType] = kAttendanceCreate;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kAttendanceCreate, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kAttendanceCreate, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kAttendanceCreate, code, text);
        });
}

void AttendanceService::updateRecord(const QVariantMap& locate, const QVariantMap& updates)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kAttendanceUpdate, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject loc = filterCriteriaToJson(locate, true);
    QJsonObject upd;
    for (auto it = updates.constBegin(); it != updates.constEnd(); ++it) {
        const QString s = it.value().toString();
        if (!s.isEmpty())
            upd[it.key()] = s;
    }

    if (!hasAnyNonEmptyString(loc) && !loc.contains(QStringLiteral("id"))) {
        emit operationFailed(kAttendanceUpdate, -1, QStringLiteral("locate 至少提供一个非空条件"));
        return;
    }
    // 若仅用 id：QML 传 { id: 5 }
    if (loc.contains(QStringLiteral("id")) && loc.value(QStringLiteral("id")).toInt() > 0)
    { /* ok */
    }
    else if (!hasAnyNonEmptyString(loc)) {
        emit operationFailed(kAttendanceUpdate, -1, QStringLiteral("locate 至少提供一个非空条件"));
        return;
    }

    if (upd.isEmpty()) {
        emit operationFailed(kAttendanceUpdate, -1, QStringLiteral("updates 至少一个非空字段"));
        return;
    }

    QJsonObject data;
    data[QStringLiteral("locate")] = loc;
    data[QStringLiteral("updates")] = upd;

    QJsonObject msg;
    msg[kType] = kAttendanceUpdate;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kAttendanceUpdate, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kAttendanceUpdate, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kAttendanceUpdate, code, text);
        });
}

void AttendanceService::deleteRecord(const QVariantMap& criteria)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kAttendanceDelete, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data = filterCriteriaToJson(criteria, true);
    if (!hasAnyNonEmptyString(data) && !(data.contains(QStringLiteral("id")) && data.value(QStringLiteral("id")).toInt() > 0)) {
        emit operationFailed(kAttendanceDelete, -1, QStringLiteral("至少提供一个非空删除条件"));
        return;
    }

    QJsonObject msg;
    msg[kType] = kAttendanceDelete;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kAttendanceDelete, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kAttendanceDelete, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kAttendanceDelete, code, text);
        });
}

void AttendanceService::queryArchive(int idFilter,
    const QString& employeeId,
    const QString& name,
    const QString& department,
    const QString& position,
    const QString& checkTime,
    const QString& deviceId,
    const QString& status,
    const QString& receivedTime,
    const QString& archivedAt,
    const QString& archiveReason)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kAttendanceArchiveQuery, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    if (idFilter >= 0)
        data[QStringLiteral("id")] = idFilter;
    else
        data[QStringLiteral("id")] = QJsonValue(QJsonValue::Null);

    data[kEmployeeId] = employeeId;
    data[kName] = name;
    data[kDepartment] = department;
    data[kPosition] = position;
    data[kCheckTime] = checkTime;
    data[kDeviceId] = deviceId;
    data[kStatus] = status;
    data[kReceivedTime] = receivedTime;
    data[QStringLiteral("archivedAt")] = archivedAt;
    data[QStringLiteral("archiveReason")] = archiveReason;

    QJsonObject msg;
    msg[kType] = kAttendanceArchiveQuery;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kAttendanceArchiveQuery, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code != ErrorCode::kSuccess) {
            emit operationFailed(kAttendanceArchiveQuery, code, text);
            return;
        }
        const QJsonObject d = resp.value(kData).toObject();
        m_archiveRecords = parseArchiveRecords(d.value(kRecords).toArray());
        emit archiveRecordsChanged();
        emit operationSucceeded(kAttendanceArchiveQuery, text.isEmpty() ? QStringLiteral("ok") : text);
        });
}

void AttendanceService::deleteArchive(const QString& employeeId)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kAttendanceArchiveDelete, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kEmployeeId] = employeeId;

    QJsonObject msg;
    msg[kType] = kAttendanceArchiveDelete;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kAttendanceArchiveDelete, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kAttendanceArchiveDelete, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kAttendanceArchiveDelete, code, text);
        });
}

void AttendanceService::exportToFile(const QUrl& fileUrl, const QString& content)
{
    QFile file(fileUrl.toLocalFile());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit operationFailed(QStringLiteral("attendance.export"), -1,
                             QStringLiteral("无法打开文件: ") + file.errorString());
        return;
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << content;
    file.close();
    emit operationSucceeded(QStringLiteral("attendance.export"), QStringLiteral("导出成功"));
}