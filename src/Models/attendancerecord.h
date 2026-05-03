#ifndef ATTENDANCERECORD_H
#define ATTENDANCERECORD_H

#include <QObject>
#include <QDateTime>
#include <QJsonObject>

class AttendanceRecord : public QObject{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString employeeId READ employeeId WRITE setEmployeeId NOTIFY employeeIdChanged)
    Q_PROPERTY(QString personName READ personName WRITE setPersonName NOTIFY personNameChanged)
    Q_PROPERTY(QDateTime checkTime READ checkTime WRITE setCheckTime NOTIFY checkTimeChanged)
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QDateTime receivedTime READ receivedTime WRITE setReceivedTime NOTIFY receivedTimeChanged)

public:
    explicit AttendanceRecord(QObject *parent = nullptr);

    int id() const;
    void setId(int id);

    QString employeeId() const;
    void setEmployeeId(const QString &employeeId);

    QString personName() const;
    void setPersonName(const QString &personName);

    QDateTime checkTime() const;
    void setCheckTime(const QDateTime &checkTime);

    QString deviceId() const;
    void setDeviceId(const QString &deviceId);

    QString status() const;
    void setStatus(const QString &status);

    QDateTime receivedTime() const;
    void setReceivedTime(const QDateTime &receivedTime);

    QJsonObject toJson() const;
    static AttendanceRecord* fromJson(const QJsonObject &json, QObject *parent = nullptr);

signals:
    void idChanged();
    void employeeIdChanged();
    void personNameChanged();
    void checkTimeChanged();
    void deviceIdChanged();
    void statusChanged();
    void receivedTimeChanged();

private:
    int m_id;
    QString m_employeeId;
    QString m_personName;
    QDateTime m_checkTime;
    QString m_deviceId;
    QString m_status;
    QDateTime m_receivedTime;
};

#endif // ATTENDANCERECORD_H
