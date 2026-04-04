#ifndef ATTENDANCERECORD_H
#define ATTENDANCERECORD_H

#include <QObject>
#include <QDateTime>
#include <QJsonObject>

class AttendanceRecord : public QObject{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int personId READ personId WRITE setPersonId NOTIFY personIdChanged)
    Q_PROPERTY(QDateTime checkTime READ checkTime WRITE setCheckTime NOTIFY checkTimeChanged)
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QDateTime receivedTime READ receivedTime WRITE setReceivedTime NOTIFY receivedTimeChanged)

public:
    explicit AttendanceRecord(QObject *parent = nullptr);

    int id() const;
    void setId(int id);

    int personId() const;
    void setPersonId(int personId);

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
    void personIdChanged();
    void checkTimeChanged();
    void deviceIdChanged();
    void statusChanged();
    void receivedTimeChanged();

private:
    int m_id;
    int m_personId;
    QDateTime m_checkTime;
    QString m_deviceId;
    QString m_status;
    QDateTime m_receivedTime;
};

#endif // ATTENDANCERECORD_H
