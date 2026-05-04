#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include <QObject>
#include <QJsonArray>

#include "src/TcpServer/tcpserver.h"
#include "src/Services/dataservice.h"
#include "src/Protocol/protocol.h"
#include "src/Models/Person.h"
#include "src/Models/faceData.h"

class TcpServer;
class DataService;

class SyncManager : public QObject
{
    Q_OBJECT
public:
    explicit SyncManager(TcpServer* tcpServer, DataService* dataService, QObject* parent = nullptr);
    Q_INVOKABLE void sendPersonSyncNow(const QString& deviceId);
    Q_INVOKABLE void sendFaceSyncNow(const QString& deviceId);

private slots:
    void onSynRequested(const QString &deviceId);

private:
    TcpServer* m_tcpServer;
    DataService* m_dataService;

    void sendPersonSync(const QString& deviceId);
    void sendFaceSync(const QString& deviceId);
};

#endif // SYNCMANAGER_H
