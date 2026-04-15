#ifndef DEVICEMONITOR_H
#define DEVICEMONITOR_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>

#include "../TcpServer/tcpserver.h"
#include "../DataManager/datamanager.h"
#include "../Protocol/protocol.h"

class TcpServer;
class DataManager;

class DeviceMonitor : public QObject
{
    Q_OBJECT
public:
    explicit DeviceMonitor(TcpServer *tcpServer,DataManager *dataManager,QObject *parent = nullptr);

public slots:
    void onClientConnected(const QString &deviceId,const QString &ipAddress);
    void onClientDisconnected(const QString& deviceId);
    void onDeviceStatusReceived(const QString& deviceId, const QJsonObject& status);

private:
    TcpServer* m_tcpServer;
    DataManager* m_dataManager;
    QTimer m_syncTimer;

    void sendSyncCommand(const QString& deviceId);
};

#endif // DEVICEMONITOR_H
