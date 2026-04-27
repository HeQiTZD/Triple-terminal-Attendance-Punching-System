#ifndef SYNCMANAGER_H
#define SYNCMANAGER_H

#include <QObject>
#include <QJsonArray>

#include "src/TcpServer/tcpserver.h"
#include "src/DataManager/datamanager.h"
#include "src/Protocol/protocol.h"

class TcpServer;
class DataManager;

class SyncManager : public QObject
{
    Q_OBJECT
public:
    explicit SyncManager(TcpServer* tcpServer, DataManager* dataManager, QObject* parent = nullptr);
    Q_INVOKABLE void sendPersonSyncNow(const QString& deviceId);

private slots:
    void onSynRequested(const QString &deviceId);

private:
    TcpServer* m_tcpServer;
    DataManager* m_dataManager;

    void sendPersonSync(const QString& deviceId);
};

#endif // SYNCMANAGER_H
