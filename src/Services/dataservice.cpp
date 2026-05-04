#include "dataservice.h"

#include "src/DataManager/datamanager.h"

#include <QDebug>
#include <QStringList>

DataService::DataService(DataManager* dataManager, QObject* parent)
    : QObject(parent), m_dataManager(dataManager)
{
    Q_ASSERT(m_dataManager);
    if (!m_dataManager) {
        qFatal("DataService: dataManager must not be null");
    }

    connect(m_dataManager, &DataManager::connectionStateChanged,
            this, &DataService::connectionStateChanged);

    connect(m_dataManager, &DataManager::deviceStatusChanged,
            this, &DataService::deviceStatusChanged);
    connect(m_dataManager, &DataManager::deviceRecordChanged,
            this, &DataService::deviceRecordChanged);

    // 只转发 DataManager 的信号到 QML（不保存错误信息）
    connect(m_dataManager, &DataManager::operationResult,
            this, &DataService::operationResult);
    connect(m_dataManager, &DataManager::operationTip,
            this, &DataService::operationTip);
}

bool DataService::isConnected() const
{
    return m_dataManager && m_dataManager->isConnected();
}

bool DataService::ensureConnected()
{
    if (!m_dataManager) return false;
    if (!m_dataManager->isConnected()) return false;
    return true;
}

// ========== Person ==========

bool DataService::addPerson(const QString& name, const QString& employeeId,
                            const QString& department, const QString& position)
{
    // 先校验入参，再校验连接（连接异常不应掩盖参数错误）
    const QString n = name.trimmed();
    const QString e = employeeId.trimmed();
    const QString d = department.trimmed();
    const QString p = position.trimmed();

    if (n.isEmpty()) { emit Data_Validation(QStringLiteral("请输入 姓名!")); return false; }
    if (e.isEmpty()) { emit Data_Validation(QStringLiteral("请输入 员工号!")); return false; }
    if (d.isEmpty()) { emit Data_Validation(QStringLiteral("请输入 所在部门!")); return false; }
    if (p.isEmpty()) { emit Data_Validation(QStringLiteral("请输入 所在部门的职位!")); return false; }

    if (!ensureConnected()) return false;
    return m_dataManager->addPerson(n, e, d, p);
}

bool DataService::updatedPerson(const QString& name, const QString& employeeId,
                                const QString& department, const QString& position)
{
    const QString e = employeeId.trimmed();
    if (e.isEmpty()) { emit Data_Validation(QStringLiteral("请输入 员工号!")); return false; }

    QVariantMap updates;
    if (!name.trimmed().isEmpty()) updates.insert("name", name.trimmed());
    if (!department.trimmed().isEmpty()) updates.insert("department", department.trimmed());
    if (!position.trimmed().isEmpty()) updates.insert("position", position.trimmed());

    if (updates.isEmpty()) { emit Data_Validation(QStringLiteral("请输入 需要修改的内容!")); return false; }

    if (!ensureConnected()) return false;

    // 由 DataManager 统一发出 operationResult，避免重复通知
    return m_dataManager->updatedPerson(e, updates);
}

bool DataService::deletePerson(const QString& employeeId)
{
    const QString e = employeeId.trimmed();
    if (e.isEmpty()) { emit Data_Validation(QStringLiteral("请输入 员工号!")); return false; }
    if (!ensureConnected()) return false;
    return m_dataManager->deletePerson(e);
}

QList<QObject*> DataService::getAllPerson()
{
    if (!ensureConnected()) return {};
    return m_dataManager->getAllPerson();
}

QObject* DataService::getPersonById(int id)
{
    if (id <= 0) return nullptr;
    if (!ensureConnected()) return nullptr;
    return m_dataManager->getPersonById(id);
}

QObject* DataService::getPersonByEmployeeId(const QString& employeeId)
{
    const QString e = employeeId.trimmed();
    if (e.isEmpty()) return nullptr;
    if (!ensureConnected()) return nullptr;
    return m_dataManager->getPersonByEmployeeId(e);
}

QList<QObject*> DataService::selectPersons(const QString& name, const QString &employeeId, const QString& department, const QString& position)
{
    const QString n = name.trimmed();
    const QString e = employeeId.trimmed();
    const QString d = department.trimmed();
    const QString p = position.trimmed();

    if(n.isEmpty() && e.isEmpty() && d.isEmpty() && p.isEmpty()){
        emit Data_Validation(QStringLiteral("请输入至少一项查询条件"));
        return {};
    }

    if(!ensureConnected()) return {};

    // 修复：原代码错把 position 当 department、department 当 position 传给 DataManager
    return m_dataManager->selectPersons(n, e, d, p);
}

// ========== Attendance ==========

bool DataService::addAttendanceRecord(const QString& employeeId, const QDateTime& checkTime,
                                      const QString& deviceId, const QString& status)
{
    const QString e = employeeId.trimmed();
    const QString d = deviceId.trimmed();
    const QString s = status.trimmed();
    if (e.isEmpty()) return false;
    if (!checkTime.isValid()) return false;
    if (d.isEmpty()) return false;
    if (!ensureConnected()) return false;
    return m_dataManager->addAttendanceRecord(e, checkTime, d, s);
}

QList<QObject *> DataService::selectAttendanceRecord(const QString &employee_id, const QDateTime &startTime,
    const QDateTime &endTime, const QString &deviceId, const QString &status, const QString &personName)
{
    const QString e = employee_id.trimmed();
    const QString device = deviceId.trimmed();
    const QString st = status.trimmed();
    const QString name = personName.trimmed();

    if (e.isEmpty() && !startTime.isValid() && !endTime.isValid() && device.isEmpty() && st.isEmpty()
        && name.isEmpty()) {
        emit Data_Validation(QStringLiteral("请输入至少一项查询条件"));
        return {};
    }

    if (startTime.isValid() && endTime.isValid() && startTime > endTime) {
        emit Data_Validation(QStringLiteral("请输入正确的时间范围"));
        return {};
    }

    if (!ensureConnected())
        return {};

    return m_dataManager->selectAttendanceRecord(e, startTime, endTime, device, st, name);
}

// ========== Device ==========

bool DataService::addOrUpdateDevice(const QString& deviceId, const QString& deviceName,
                                    const QString& ipAddress, const QString& status)
{
    const QString did = deviceId.trimmed();
    if (did.isEmpty()) return false;
    if (!ensureConnected()) return false;
    return m_dataManager->addOrUpdateDevice(did, deviceName.trimmed(), ipAddress.trimmed(), status.trimmed());
}

bool DataService::updateDevice(const QString& deviceId, const QVariantMap& updates)
{
    const QString did = deviceId.trimmed();
    if (did.isEmpty()) return false;
    if (!ensureConnected()) return false;

    static const QStringList kKeys = {
        QStringLiteral("device_name"),
        QStringLiteral("ip_address"),
        QStringLiteral("status"),
    };
    QVariantMap cleaned;
    for (const QString& k : kKeys) {
        if (!updates.contains(k))
            continue;
        const QString v = updates.value(k).toString().trimmed();
        if (v.isEmpty())
            continue;
        cleaned.insert(k, v);
    }
    if (cleaned.isEmpty())
        return false;

    return m_dataManager->updateDevice(did, cleaned);
}

bool DataService::updateDeviceStatus(const QString& deviceId, const QString& status)
{
    const QString did = deviceId.trimmed();
    const QString st = status.trimmed();
    if (did.isEmpty() || st.isEmpty()) return false;
    if (!ensureConnected()) return false;
    QVariantMap m;
    m.insert(QStringLiteral("status"), st);
    return m_dataManager->updateDevice(did, m);
}

QList<QObject*> DataService::getAllDevices()
{
    if (!ensureConnected()) return {};
    return m_dataManager->getAllDevices();
}

QObject* DataService::getDeviceById(const QString& deviceId)
{
    const QString did = deviceId.trimmed();
    if (did.isEmpty()) return nullptr;
    if (!ensureConnected()) return nullptr;
    return m_dataManager->getDeviceById(did);
}

QList<QObject *> DataService::selectDevice(const QString &deviceId, const QString &deviceName,
    const QString &ipAddress, const QString &status)
{
    const QString did = deviceId.trimmed();
    const QString dname = deviceName.trimmed();
    const QString ip = ipAddress.trimmed();
    const QString st = status.trimmed();

    if (did.isEmpty() && dname.isEmpty() && ip.isEmpty() && st.isEmpty()) {
        emit Data_Validation(QStringLiteral("请输入至少一项查询条件"));
        return {};
    }

    if (!ensureConnected())
        return {};

    return m_dataManager->selectDevice(did, dname, ip, st);
}

// ========== Face data ==========

bool DataService::addFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector)
{
    const QString e = employeeId.trimmed();
    if (e.isEmpty()) return false;
    if (featureVector.isEmpty()) return false;
    if (!ensureConnected()) return false;
    return m_dataManager->addFaceDataByEmployeeId(e, featureVector);
}

bool DataService::updateFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector)
{
    const QString e = employeeId.trimmed();
    if (e.isEmpty()) return false;
    if (featureVector.isEmpty()) return false;
    if (!ensureConnected()) return false;
    return m_dataManager->updateFaceDataByEmployeeId(e, featureVector);
}

bool DataService::deleteFaceDataByEmployeeId(const QString& employeeId)
{
    const QString e = employeeId.trimmed();
    if (e.isEmpty()) return false;
    if (!ensureConnected()) return false;
    return m_dataManager->deleteFaceDataByEmployeeId(e);
}

QObject* DataService::getFaceDataByEmployeeId(const QString& employeeId)
{
    const QString e = employeeId.trimmed();
    if (e.isEmpty()) return nullptr;
    if (!ensureConnected()) return nullptr;
    return m_dataManager->getFaceDataByEmployeeId(e);
}

QList<QObject*> DataService::getAllFaceData()
{
    if (!ensureConnected()) return {};
    return m_dataManager->getAllFaceData();
}

QList<QObject*> DataService::getAllFaceDataWithFeature()
{
    if (!ensureConnected()) return {};
    return m_dataManager->getAllFaceDataWithFeature();
}

QList<QObject *> DataService::selectFaceData(const QString &employeeId, const QDateTime &createdStart,
    const QDateTime &createdEnd, const QDateTime &updatedStart, const QDateTime &updatedEnd)
{
    const QString e = employeeId.trimmed();
    const bool hasCs = createdStart.isValid();
    const bool hasCe = createdEnd.isValid();
    const bool hasUs = updatedStart.isValid();
    const bool hasUe = updatedEnd.isValid();

    if (e.isEmpty() && !hasCs && !hasCe && !hasUs && !hasUe) {
        emit Data_Validation(QStringLiteral("请输入至少一项查询条件"));
        return {};
    }

    if (hasCs && hasCe && createdStart > createdEnd) {
        emit Data_Validation(QStringLiteral("请输入正确的创建时间范围"));
        return {};
    }
    if (hasUs && hasUe && updatedStart > updatedEnd) {
        emit Data_Validation(QStringLiteral("请输入正确的更新时间范围"));
        return {};
    }

    if (!ensureConnected())
        return {};

    return m_dataManager->selectFaceData(e, createdStart, createdEnd, updatedStart, updatedEnd);
}

