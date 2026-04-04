#include "attendancerecord.h"

AttendanceRecord::AttendanceRecord(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_personId(0)
{
}

int AttendanceRecord::id() const { return m_id; }
void AttendanceRecord::setId(int id) {
    if (m_id != id) {
        m_id = id;
        emit idChanged();
    }
}

int AttendanceRecord::personId() const { return m_personId; }
void AttendanceRecord::setPersonId(int personId) {
    if (m_personId != personId) {
        m_personId = personId;
        emit personIdChanged();
    }
}

QDateTime AttendanceRecord::checkTime() const { return m_checkTime; }
void AttendanceRecord::setCheckTime(const QDateTime &checkTime) {
    if (m_checkTime != checkTime) {
        m_checkTime = checkTime;
        emit checkTimeChanged();
    }
}

QString AttendanceRecord::deviceId() const { return m_deviceId; }
void AttendanceRecord::setDeviceId(const QString &deviceId) {
    if (m_deviceId != deviceId) {
        m_deviceId = deviceId;
        emit deviceIdChanged();
    }
}

QString AttendanceRecord::status() const { return m_status; }
void AttendanceRecord::setStatus(const QString &status) {
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

QDateTime AttendanceRecord::receivedTime() const { return m_receivedTime; }
void AttendanceRecord::setReceivedTime(const QDateTime &receivedTime) {
    if (m_receivedTime != receivedTime) {
        m_receivedTime = receivedTime;
        emit receivedTimeChanged();
    }
}

QJsonObject AttendanceRecord::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["personId"] = m_personId;
    json["checkTime"] = m_checkTime.toString(Qt::ISODate);
    json["deviceId"] = m_deviceId;
    json["status"] = m_status;
    json["receivedTime"] = m_receivedTime.toString(Qt::ISODate);
    return json;
}

AttendanceRecord* AttendanceRecord::fromJson(const QJsonObject &json, QObject *parent)
{
    AttendanceRecord *record = new AttendanceRecord(parent);
    record->setId(json["id"].toInt());
    record->setPersonId(json["personId"].toInt());
    record->setCheckTime(QDateTime::fromString(json["checkTime"].toString(), Qt::ISODate));
    record->setDeviceId(json["deviceId"].toString());
    record->setStatus(json["status"].toString());
    record->setReceivedTime(QDateTime::fromString(json["receivedTime"].toString(), Qt::ISODate));
    return record;
}
