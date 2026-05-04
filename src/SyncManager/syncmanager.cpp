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

void SyncManager::sendFaceSyncNow(const QString& deviceId)
{
    if (deviceId.isEmpty()) return;
    if (!m_dataService->isConnected()) return;
    sendFaceSync(deviceId);
}

void SyncManager::onSynRequested(const QString &deviceId)
{
    if(!m_dataService->isConnected()) return;
    sendPersonSync(deviceId);
    // 设备首次连接/主动请求时，同步一份人脸库（全量）
    sendFaceSync(deviceId);
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

void SyncManager::sendFaceSync(const QString& deviceId)
{
    // 1) begin
    QJsonObject begin;
    begin[Protocol::kType] = Protocol::kFaceSyncBegin;
    begin[Protocol::kDeviceId] = deviceId;
    m_tcpServer->sendToClient(deviceId, begin);

    // 2) items: json header + binary frame
    const QList<QObject*> faceList = m_dataService->getAllFaceDataWithFeature();
    for (QObject* obj : faceList) {
        auto* fd = qobject_cast<FaceData*>(obj);
        if (!fd) continue;

        const QByteArray feature = fd->featureVector();
        if (feature.isEmpty()) {
            continue;
        }

        QJsonObject header;
        header[Protocol::kType] = Protocol::kFaceSyncItemHeader;
        header[Protocol::kDeviceId] = deviceId;
        header[Protocol::kEmployeeId] = fd->employeeId();
        header[Protocol::kFeatureSize] = fd->featureSize();
        header[Protocol::kPayloadLength] = feature.size();
        header["contentType"] = "application/octet-stream";
        header["payloadEncoding"] = "raw";

        m_tcpServer->sendToClient(deviceId, header);
        m_tcpServer->sendBinaryFrameToClient(deviceId, feature);
    }
    qDeleteAll(faceList);

    // 3) end
    QJsonObject end;
    end[Protocol::kType] = Protocol::kFaceSyncEnd;
    end[Protocol::kDeviceId] = deviceId;
    m_tcpServer->sendToClient(deviceId, end);
}


