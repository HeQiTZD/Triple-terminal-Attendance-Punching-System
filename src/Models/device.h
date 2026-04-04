#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QDateTime>
#include <QJsonObject>

class Device : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QDateTime lastOnline READ lastOnline WRITE setLastOnline NOTIFY lastOnlineChanged)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)

public:
    explicit Device(QObject *parent = nullptr);

    int id() const;
    void setId(int id);

    QString deviceId() const;
    void setDeviceId(const QString &deviceId);

    QString deviceName() const;
    void setDeviceName(const QString &deviceName);

    QString ipAddress() const;
    void setIpAddress(const QString &ipAddress);

    QDateTime lastOnline() const;
    void setLastOnline(const QDateTime &lastOnline);

    QString status() const;
    void setStatus(const QString &status);

    QJsonObject toJson() const;
    static Device* fromJson(const QJsonObject &json, QObject *parent = nullptr);

signals:
    void idChanged();
    void deviceIdChanged();
    void deviceNameChanged();
    void ipAddressChanged();
    void lastOnlineChanged();
    void statusChanged();

private:
    int m_id;
    QString m_deviceId;
    QString m_deviceName;
    QString m_ipAddress;
    QDateTime m_lastOnline;
    QString m_status; // "online", "offline"
};

#endif // DEVICE_H
