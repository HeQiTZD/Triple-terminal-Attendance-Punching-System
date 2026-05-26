#include "DeviceLocalRepository.h"
#include "../Utils/DatabaseManager.h"
#include <QSqlError>
#include <QSqlQuery>

DeviceLocalRepository::DeviceLocalRepository(const QString &dbPath)
    : m_dbPath(dbPath)
{
}

void DeviceLocalRepository::ensureRow(const QString &deviceId,
                                      const QString &deviceName,
                                      const QString &fwVersion)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("INSERT OR IGNORE INTO device_local (id, device_id, device_name, fw_version) "
                  "VALUES (1, :did, :dn, :fw)");
    query.bindValue(":did", deviceId);
    query.bindValue(":dn",  deviceName.isEmpty() ? deviceId : deviceName);
    query.bindValue(":fw",  fwVersion);

    query.exec();
}

DeviceLocalInfo DeviceLocalRepository::get()
{
    DeviceLocalInfo info;
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
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
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE device_local SET device_id = :v WHERE id = 1");
    query.bindValue(":v", deviceId);
    if (!query.exec()) {
        return false;
    }
    return true;
}

bool DeviceLocalRepository::updateDeviceName(const QString &deviceName)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE device_local SET device_name = :v WHERE id = 1");
    query.bindValue(":v", deviceName);
    if (!query.exec()) {
        return false;
    }
    return true;
}

bool DeviceLocalRepository::updateIpAddress(const QString &ipAddress)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE device_local SET ip_address = :v WHERE id = 1");
    query.bindValue(":v", ipAddress);
    if (!query.exec()) {
        return false;
    }
    return true;
}

bool DeviceLocalRepository::updateFwVersion(const QString &fwVersion)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("UPDATE device_local SET fw_version = :v WHERE id = 1");
    query.bindValue(":v", fwVersion);
    if (!query.exec()) {
        return false;
    }
    return true;
}
