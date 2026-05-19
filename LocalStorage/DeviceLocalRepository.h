#pragma once

#include <QSqlDatabase>
#include <QString>

struct DeviceLocalInfo {
    QString deviceId;
    QString deviceName;
    QString ipAddress;
    QString fwVersion;
};

class DeviceLocalRepository {
public:
    explicit DeviceLocalRepository(QSqlDatabase &db);

    void ensureRow(const QString &deviceId,
                   const QString &deviceName = {},
                   const QString &fwVersion = QStringLiteral("1.0.0"));

    DeviceLocalInfo get();

    bool updateDeviceId(const QString &deviceId);
    bool updateDeviceName(const QString &deviceName);
    bool updateIpAddress(const QString &ipAddress);
    bool updateFwVersion(const QString &fwVersion);

private:
    QSqlDatabase &m_db;
};
