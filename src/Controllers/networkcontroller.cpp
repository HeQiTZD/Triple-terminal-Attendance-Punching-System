#include "networkcontroller.h"
#include "src/Protocol/protocol.h"

networkcontroller::networkcontroller(TcpServer* tcpServer, DataManager* dataManager, QObject* parent)
: QObject(parent), m_tcpServer(tcpServer), m_dataManager(dataManager)
{
    // 检查 m_tcpServer 是否为空
    Q_ASSERT(m_tcpServer);
    // 检查 m_dataManager 是否为空
    Q_ASSERT(m_dataManager);

    connect(m_tcpServer,&TcpServer::clientConnected,this,&networkcontroller::onClientConnected);
    connect(m_tcpServer,&TcpServer::clientDisconnected,this,&networkcontroller::onClientDisconnected);
    connect(m_tcpServer,&TcpServer::attendanceRecordReceived,this,&networkcontroller::onAttendanceRecordReceived);
    connect(m_tcpServer,&TcpServer::deviceStatusReceived,this,&networkcontroller::onDeviceStatusReceiced);
    
}

void networkcontroller::onClientConnected(const QString &deviceId, const QString &ipAddress)
{
    // 设备上线：写入/更新设备表
    if(m_dataManager->isConnected()){
        m_dataManager->addOrUpdateDevice(deviceId,deviceId,ipAddress,"online");
    }
}

void networkcontroller::onClientDisconnected(const QString &deviceId)
{
     // 设备离线：更新状态
    if(m_dataManager->isConnected()){
         m_dataManager->updateDeviceStatus(deviceId,"offline");
    }
}

void networkcontroller::onAttendanceRecordReceived(const QJsonObject &record)
{
    if(!m_dataManager->isConnected()) return;

    QString err;
    if(!validateAttendanceRecord(record,&err)){
        return;
    }

    const QString employeeId = record.value(Protocol::kPersonEmployeeId).toString();
    QObject* personObj = m_dataManager->getPersonByEmployeeId(employeeId);
    if(!personObj){
        return;
    }

    const int personId = personObj->property("id").toInt();

    const QString checkTimeStr = record.value(Protocol::kCheckTime).toString();
    const QDateTime checkTime = QDateTime::fromString(checkTimeStr,Qt::ISODate);
    if(!checkTime.isValid()) return;

    const QString deviceId = record.value(Protocol::kDeviceId).toString();
    const QString status = record.value(Protocol::kStatus).toString("ok");

    m_dataManager->addAttendanceRecore(personId,checkTime,deviceId,status);
}

void networkcontroller::onDeviceStatusReceiced(const QString &deviceId, const QJsonObject &status)
{
    if(!m_dataManager->isConnected()) return;

    QString err;
    if(!validateDeviceStatus(status,&err)){
        return;
    }

    // 简化：只更新在线状态字段。你也可以解析 status payload 里的更多信息。
    const QString online = status.value(Protocol::kStatus).toString("online");
    m_dataManager->updateDeviceStatus(deviceId, online);
}

bool networkcontroller::validateAttendanceRecord(const QJsonObject &obj, QString *error) const
{
    const auto type = obj.value(Protocol::kType).toString();
    if(type != Protocol::kAttendanceRecord){
        if(error) *error = "type mismatch";
        return false;
    }

    if(!obj.contains(Protocol::kPersonEmployeeId) || obj.value(Protocol::kPersonEmployeeId).toString().isEmpty()){
        if(error) *error = "missing employeeId";
        return false;
    }

    if(!obj.contains(Protocol::kCheckTime) || obj.value(Protocol::kCheckTime).toString().isEmpty()){
        if(error) *error = "missing checkTime";
        return false;
    }

    return true;
}

bool networkcontroller::validateDeviceStatus(const QJsonObject &obj, QString *error) const
{

}
