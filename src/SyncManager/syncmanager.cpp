#include "syncmanager.h"

SyncManager::SyncManager(TcpServer* tcpServer, DataService* dataService, QObject* parent)
    :m_tcpServer(tcpServer),m_dataService(dataService)
{
    //确保存在，有效
    Q_ASSERT(m_tcpServer);
    Q_ASSERT(m_dataService);

    connect(m_tcpServer,&TcpServer::syncRequested,this,&SyncManager::onSynRequested);
}

void SyncManager::sendPersonSyncNow(const QString& deviceId)
{
    if (deviceId.isEmpty()) return;
    if (!m_dataService->isConnected()) return;
    sendPersonSync(deviceId);
}

void SyncManager::onSynRequested(const QString &deviceId)
{
    if(!m_dataService->isConnected()) return;
    sendPersonSync(deviceId);
}

void SyncManager::sendPersonSync(const QString &deviceId)
{
    // DataManager 不再为返回对象设置 parent，这里使用完成后需自行释放
    const QList<QObject*> persons = m_dataService->getAllPerson();

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
    qDeleteAll(persons);

    QJsonObject msg;
    msg[Protocol::kType] = Protocol::kPersonSync;
    msg[Protocol::kDeviceId] = deviceId;
    msg[Protocol::kPersons] = arr;

    m_tcpServer->sendToClient(deviceId,msg);
}


