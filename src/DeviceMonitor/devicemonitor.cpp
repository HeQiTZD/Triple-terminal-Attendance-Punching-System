#include "devicemonitor.h"

DeviceMonitor::DeviceMonitor(TcpServer *tcpServer,DataManager *dataManager,QObject *parent)
    :m_tcpServer(tcpServer),m_dataManager(dataManager)
{
    Q_ASSERT(m_tcpServer);
    Q_ASSERT(m_dataManager);

    // // 示例：每 5 分钟给所有在线设备下发一次“同步指令”（按需开启）
    // m_syncTimer.setInterval(5 * 60 * 1000);
    // connect(&m_syncTimer, &QTimer::timeout, this, [this](){
    //     QJsonObject cmd;
    //     cmd[Protocol::kType] = "sync_request"; // 你需要在客户端实现该 type
    //     m_tcpServer->brodcastsToAll(cmd);
    // });
    // // m_syncTimer.start();
}

void DeviceMonitor::onClientConnected(const QString &deviceId, const QString &ipAddress)
{
    if(m_dataManager->isConnected()){
        m_dataManager->addOrUpdateDevice(deviceId,deviceId,ipAddress,"online");
    }

    sendSyncCommand(deviceId);
}

void DeviceMonitor::onClientDisconnected(const QString &deviceId)
{
    if (m_dataManager->isConnected()) {
        m_dataManager->updateDeviceStatus(deviceId, "offline");
    }
}

void DeviceMonitor::sendSyncCommand(const QString &deviceId)
{
    QJsonObject cmd;
    cmd[Protocol::kType] = "sync_request";
    cmd[Protocol::kDeviceId] = deviceId;
    m_tcpServer->sendToClient(deviceId,cmd);
}

void DeviceMonitor::onDeviceStatusReceived(const QString& deviceId, const QJsonObject& status)
{
    if(!m_dataManager->isConnected()) return;

    const QString online = status.value(Protocol::kStatus).toString("online");
    const QString deviceName = status.value("deviceName").toString(deviceId);
    const QString ipAddress  = status.value("ipAddress").toString();

    if(!ipAddress.isEmpty()){
        m_dataManager->addOrUpdateDevice(deviceId,deviceName,ipAddress,online);
    }else{
        m_dataManager->updateDeviceStatus(deviceId, online);
    }
}
