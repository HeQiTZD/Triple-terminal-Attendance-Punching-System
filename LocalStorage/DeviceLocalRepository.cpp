#include "DeviceLocalRepository.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

DeviceLocalRepository::DeviceLocalRepository(QSqlDatabase &db)
    : m_db(db)
{
}

void DeviceLocalRepository::ensureRow(const QString &deviceId,
                                      const QString &deviceName,
                                      const QString &fwVersion)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT OR IGNORE INTO device_local (id, device_id, device_name, fw_version) "
                  "VALUES (1, :did, :dn, :fw)");
    query.bindValue(":did", deviceId);
    query.bindValue(":dn",  deviceName.isEmpty() ? deviceId : deviceName);
    query.bindValue(":fw",  fwVersion);

    if (!query.exec()) {
        qWarning() << "DeviceLocalRepository ensureRow failed:" << query.lastError().text();
    }
}

DeviceLocalInfo DeviceLocalRepository::get()
{
    DeviceLocalInfo info;
    QSqlQuery query(m_db);
    query.prepare("SELECT device_id, device_name, ip_address, fw_version FROM device_local WHERE id = 1");

    if (query.exec() && query.next()) {
        info.deviceId   = query.value(0).toString();
        info.deviceName = query.value(1).toString();
        info.ipAddress  = query.value(2).toString();
        info.fwVersion  = query.value(3).toString();
    }
    return info;
}

bool DeviceLocalRepository::updateDeviceId(const QString &deviceId)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE device_local SET device_id = :v WHERE id = 1");
    query.bindValue(":v", deviceId);
    if (!query.exec()) {
        qWarning() << "DeviceLocalRepository updateDeviceId failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DeviceLocalRepository::updateDeviceName(const QString &deviceName)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE device_local SET device_name = :v WHERE id = 1");
    query.bindValue(":v", deviceName);
    if (!query.exec()) {
        qWarning() << "DeviceLocalRepository updateDeviceName failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DeviceLocalRepository::updateIpAddress(const QString &ipAddress)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE device_local SET ip_address = :v WHERE id = 1");
    query.bindValue(":v", ipAddress);
    if (!query.exec()) {
        qWarning() << "DeviceLocalRepository updateIpAddress failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DeviceLocalRepository::updateFwVersion(const QString &fwVersion)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE device_local SET fw_version = :v WHERE id = 1");
    query.bindValue(":v", fwVersion);
    if (!query.exec()) {
        qWarning() << "DeviceLocalRepository updateFwVersion failed:" << query.lastError().text();
        return false;
    }
    return true;
}
