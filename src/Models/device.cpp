#include "device.h"

Device::Device(QObject *parent)
    : QObject(parent)
    , m_id(0)
{
}

int Device::id() const { return m_id; }
void Device::setId(int id) {
    if (m_id != id) {
        m_id = id;
        emit idChanged();
    }
}

QString Device::deviceId() const { return m_deviceId; }
void Device::setDeviceId(const QString &deviceId) {
    if (m_deviceId != deviceId) {
        m_deviceId = deviceId;
        emit deviceIdChanged();
    }
}

QString Device::deviceName() const { return m_deviceName; }
void Device::setDeviceName(const QString &deviceName) {
    if (m_deviceName != deviceName) {
        m_deviceName = deviceName;
        emit deviceNameChanged();
    }
}

QString Device::ipAddress() const { return m_ipAddress; }
void Device::setIpAddress(const QString &ipAddress) {
    if (m_ipAddress != ipAddress) {
        m_ipAddress = ipAddress;
        emit ipAddressChanged();
    }
}

QDateTime Device::lastOnline() const { return m_lastOnline; }
void Device::setLastOnline(const QDateTime &lastOnline) {
    if (m_lastOnline != lastOnline) {
        m_lastOnline = lastOnline;
        emit lastOnlineChanged();
    }
}

QString Device::status() const { return m_status; }
void Device::setStatus(const QString &status) {
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

QJsonObject Device::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["deviceId"] = m_deviceId;
    json["deviceName"] = m_deviceName;
    json["ipAddress"] = m_ipAddress;
    json["lastOnline"] = m_lastOnline.toString(Qt::ISODate);
    json["status"] = m_status;
    return json;
}

Device* Device::fromJson(const QJsonObject &json, QObject *parent)
{
    Device *device = new Device(parent);
    device->setId(json["id"].toInt());
    device->setDeviceId(json["deviceId"].toString());
    device->setDeviceName(json["deviceName"].toString());
    device->setIpAddress(json["ipAddress"].toString());
    device->setLastOnline(QDateTime::fromString(json["lastOnline"].toString(), Qt::ISODate));
    device->setStatus(json["status"].toString());
    return device;
}
