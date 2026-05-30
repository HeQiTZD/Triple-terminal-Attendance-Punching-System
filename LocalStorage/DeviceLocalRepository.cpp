#include "DeviceLocalRepository.h"

DeviceLocalRepository::DeviceLocalRepository(const QString &dbPath)
    : BaseRepository(dbPath)
{
}

void DeviceLocalRepository::ensureRow(const QString &deviceId,
                                      const QString &deviceName,
                                      const QString &fwVersion)
{
    executeNonQuery(
        "INSERT OR IGNORE INTO device_local (id, device_id, device_name, fw_version) "
        "VALUES (1, :did, :dn, :fw)",
        {{":did", deviceId},
         {":dn", deviceName.isEmpty() ? deviceId : deviceName},
         {":fw", fwVersion}}
    );
}

DeviceLocalInfo DeviceLocalRepository::get()
{
    DeviceLocalInfo info;
    executeReader(
        "SELECT device_id, device_name, ip_address, fw_version FROM device_local WHERE id = 1",
        {},
        [&](QSqlQuery &q) {
            info.deviceId   = q.value(0).toString();
            info.deviceName = q.value(1).toString();
            info.ipAddress  = q.value(2).toString();
            info.fwVersion  = q.value(3).toString();
        });
    return info;
}

bool DeviceLocalRepository::updateDeviceId(const QString &deviceId)
{
    return executeNonQuery(
        "UPDATE device_local SET device_id = :v WHERE id = 1",
        {{":v", deviceId}}
    );
}

bool DeviceLocalRepository::updateDeviceName(const QString &deviceName)
{
    return executeNonQuery(
        "UPDATE device_local SET device_name = :v WHERE id = 1",
        {{":v", deviceName}}
    );
}

bool DeviceLocalRepository::updateIpAddress(const QString &ipAddress)
{
    return executeNonQuery(
        "UPDATE device_local SET ip_address = :v WHERE id = 1",
        {{":v", ipAddress}}
    );
}

bool DeviceLocalRepository::updateFwVersion(const QString &fwVersion)
{
    return executeNonQuery(
        "UPDATE device_local SET fw_version = :v WHERE id = 1",
        {{":v", fwVersion}}
    );
}
