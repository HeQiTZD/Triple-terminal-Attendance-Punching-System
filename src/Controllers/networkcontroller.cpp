#include "networkcontroller.h"
#include "src/Protocol/protocol.h"

networkcontroller::networkcontroller(TcpServer* tcpServer, DataService* dataService, QObject* parent)
    : QObject(parent), m_tcpServer(tcpServer), m_dataService(dataService)
{
    // 检查 m_tcpServer 是否为空
    Q_ASSERT(m_tcpServer);
    // 检查 m_dataService 是否为空
    Q_ASSERT(m_dataService);

    connect(m_tcpServer, &TcpServer::clientConnected, this, &networkcontroller::onClientConnected);
    connect(m_tcpServer, &TcpServer::clientDisconnected, this, &networkcontroller::onClientDisconnected);
    connect(m_tcpServer, &TcpServer::attendanceRecordReceived, this, &networkcontroller::onAttendanceRecordReceived);
    connect(m_tcpServer, &TcpServer::deviceStatusReceived, this, &networkcontroller::onDeviceStatusReceiced);
}

void networkcontroller::onClientConnected(const QString& deviceId, const QString& ipAddress)
{
    // 设备上线：写入/更新设备表
    if (m_dataService->isConnected()) {
        m_dataService->addOrUpdateDevice(deviceId, deviceId, ipAddress, "online");
    }
}

void networkcontroller::onClientDisconnected(const QString& deviceId)
{
    // 设备离线：更新状态
    if (m_dataService->isConnected()) {
        m_dataService->updateDeviceStatus(deviceId, "offline");
    }
}

void networkcontroller::onAttendanceRecordReceived(const QJsonObject& record)
{
    if (!m_dataService->isConnected()) return;

    QString err;
    if (!validateAttendanceRecord(record, &err)) {
        return;
    }

    const QString employeeId = record.value(Protocol::kPersonEmployeeId).toString();
    if (employeeId.trimmed().isEmpty()) {
        return;
    }
    // DataManager 不再为返回对象设置 parent，这里只用作"是否存在"判断后立即释放
    QObject* personObj = m_dataService->getPersonByEmployeeId(employeeId);
    if (!personObj) {
        return;
    }
    delete personObj;

    const QString checkTimeStr = record.value(Protocol::kCheckTime).toString();
    const QDateTime checkTime = QDateTime::fromString(checkTimeStr, Qt::ISODate);
    if (!checkTime.isValid()) return;

    const QString deviceId = record.value(Protocol::kDeviceId).toString();
    const QString status = record.value(Protocol::kStatus).toString("ok");

    m_dataService->addAttendanceRecord(employeeId, checkTime, deviceId, status);
}

void networkcontroller::onDeviceStatusReceiced(const QString& deviceId, const QJsonObject& status)
{
    if (!m_dataService->isConnected()) return;

    QString err;
    if (!validateDeviceStatus(status, &err)) {
        return;
    }

    const QString online = status.value(Protocol::kStatus).toString("online");
    const QString deviceName = status.value("deviceName").toString(deviceId);
    const QString ipAddress = status.value("ipAddress").toString();

    // 设备状态落库：控制器层统一处理，避免多处覆盖/重复写。
    if (!ipAddress.isEmpty()) {
        m_dataService->addOrUpdateDevice(deviceId, deviceName, ipAddress, online);
    } else {
        m_dataService->updateDeviceStatus(deviceId, online);
    }
}

bool networkcontroller::validateAttendanceRecord(const QJsonObject& obj, QString* error) const
{
    const auto type = obj.value(Protocol::kType).toString();
    if (type != Protocol::kAttendanceRecord) {
        if (error) *error = "type mismatch";
        return false;
    }

    if (!obj.contains(Protocol::kPersonEmployeeId) || obj.value(Protocol::kPersonEmployeeId).toString().isEmpty()) {
        if (error) *error = "missing employeeId";
        return false;
    }

    if (!obj.contains(Protocol::kCheckTime) || obj.value(Protocol::kCheckTime).toString().isEmpty()) {
        if (error) *error = "missing checkTime";
        return false;
    }

    return true;
}

bool networkcontroller::validateDeviceStatus(const QJsonObject& obj, QString* error) const
{
    const auto type = obj.value(Protocol::kType).toString();
    if (type != Protocol::kDeviceStatus) {
        if (error) *error = "type mismatch";
        return false;
    }
    return true;
}

