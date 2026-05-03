#include "devicemonitor.h"

DeviceMonitor::DeviceMonitor(TcpServer *tcpServer, DataService *dataService, QObject *parent)
    :m_tcpServer(tcpServer),m_dataService(dataService)
{
    Q_ASSERT(m_tcpServer);
    Q_ASSERT(m_dataService);

    // DeviceMonitor 只负责“同步触发/策略”，不负责设备状态落库。
    // 监听数据库层的设备状态变更（由控制器层统一写库后发出）。
    connect(m_dataService, &DataService::deviceStatusChanged,
            this, &DeviceMonitor::onDeviceStatusChanged);

    // // 示例：每 5 分钟给所有在线设备下发一次“同步指令”（按需开启）
    // m_syncTimer.setInterval(5 * 60 * 1000);
    // connect(&m_syncTimer, &QTimer::timeout, this, [this](){
    //     QJsonObject cmd;
    //     cmd[Protocol::kType] = "sync_request"; // 你需要在客户端实现该 type
    //     m_tcpServer->brodcastsToAll(cmd);
    // });
    // // m_syncTimer.start();
}

void DeviceMonitor::sendSyncCommand(const QString &deviceId)
{
    QJsonObject cmd;
    cmd[Protocol::kType] = "sync_request";
    cmd[Protocol::kDeviceId] = deviceId;
    m_tcpServer->sendToClient(deviceId,cmd);
}

void DeviceMonitor::onDeviceStatusChanged(const QString& deviceId, const QString& status)
{
    // 仅在设备变为在线时触发同步（策略可扩展：节流、重试、队列等）。
    if (status == "online") {
        sendSyncCommand(deviceId);
    }
}
