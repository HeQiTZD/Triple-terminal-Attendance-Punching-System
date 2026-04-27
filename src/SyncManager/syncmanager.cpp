#include "syncmanager.h"

SyncManager::SyncManager(TcpServer* tcpServer, DataManager* dataManager, QObject* parent)
    :m_tcpServer(tcpServer),m_dataManager(dataManager)
{
    //确保存在，有效
    Q_ASSERT(m_tcpServer);
    Q_ASSERT(m_dataManager);

    connect(m_tcpServer,&TcpServer::syncRequested,this,&SyncManager::onSynRequested);
}

void SyncManager::sendPersonSyncNow(const QString& deviceId)
{
    if (deviceId.isEmpty()) return;
    if (!m_dataManager->isConnected()) return;
    sendPersonSync(deviceId);
}

void SyncManager::onSynRequested(const QString &deviceId)
{
    if(!m_dataManager->isConnected()) return;
    sendPersonSync(deviceId);
}

void SyncManager::sendPersonSync(const QString &deviceId)
{
    const QList<QObject*> persons = m_dataManager->getAllPerson();

    QJsonArray arr;
    for(QObject* obj : persons){
        auto* p = qobject_cast<Person*>(obj);
        if(!p) continue;

        QJsonObject item;
        item["id"] = p->id();
        item["name"] = p->name();
        item["employeeId"] = p->employeeId();
        item["department"] = p->department();
        item["position"] = p->position();
        arr.append(item);
    }

    QJsonObject msg;
    msg[Protocol::kType] = Protocol::kPersonSync;
    msg[Protocol::kDeviceId] = deviceId;
    msg[Protocol::kPersons] = arr;

    m_tcpServer->sendToClient(deviceId,msg);
}


