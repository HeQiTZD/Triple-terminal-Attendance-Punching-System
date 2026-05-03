#ifndef DEVICEMONITOR_H
#define DEVICEMONITOR_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>

#include "../TcpServer/tcpserver.h"
#include "../Services/dataservice.h"
#include "../Protocol/protocol.h"

class TcpServer;
class DataService;

class DeviceMonitor : public QObject
{
    Q_OBJECT
public:
    explicit DeviceMonitor(TcpServer *tcpServer, DataService *dataService, QObject *parent = nullptr);

private slots:
    void onDeviceStatusChanged(const QString& deviceId, const QString& status);

private:
    TcpServer* m_tcpServer;
    DataService* m_dataService;
    QTimer m_syncTimer;

    void sendSyncCommand(const QString& deviceId);
};

#endif // DEVICEMONITOR_H
