#include "datamanager.h"

namespace {
// 使用具名连接，避免与同进程内的其它 QMYSQL 默认连接互相覆盖
constexpr const char* kConnectionName = "AttendanceMain";

QString sqlQuotedMysqlString(const QString &s)
{
    QString escaped = s;
    escaped.replace(QLatin1Char('\''), QStringLiteral("''"));
    return QStringLiteral("'") + escaped + QStringLiteral("'");
}
}

DataManager::DataManager(QObject *parent) : QObject(parent),m_isConnected(false) {}

DataManager::~DataManager()
{
    close();
}

bool DataManager::initialize(const QString &host, const QString &database, const QString &username, const QString &password)
{
    if (QSqlDatabase::contains(kConnectionName)) {
        m_db = QSqlDatabase::database(kConnectionName, /*open=*/false);
    } else {
        m_db = QSqlDatabase::addDatabase("QMYSQL", kConnectionName);
    }
    m_db.setHostName(host);
    m_db.setDatabaseName(database);
    m_db.setUserName(username);
    m_db.setPassword(password);

    if(!m_db.open()){
        emit operationResult(false, QString("数据库连接失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    m_isConnected = true;
    emit connectionStateChanged();
    emit operationResult(true, QString("数据库连接成功：%1").arg(database));

    //创建数据表
    if(!createTables()){
        emit operationResult(false, QString("数据表创建失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    emit operationResult(true, "数据表创建完成");
    return true;
}

void DataManager::close()
{
    if(m_db.isOpen()){
        m_db.close();
    }
    // 释放连接前需要让本对象不再持有 QSqlDatabase 副本，否则 removeDatabase 会发出资源泄漏警告
    m_db = QSqlDatabase();
    if (QSqlDatabase::contains(kConnectionName)) {
        QSqlDatabase::removeDatabase(kConnectionName);
    }

    const bool wasConnected = m_isConnected;
    m_isConnected = false;
    if (wasConnected) {
        emit connectionStateChanged();
        emit operationResult(true, "数据库连接已关闭");
    }
}

bool DataManager::isConnected() const
{
    return m_isConnected && m_db.isOpen() && m_db.isValid();
}

bool DataManager::addPerson(const QString &name, const QString &employeeId, const QString &department, const QString &position)
{
    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO Person (name, employee_id, department, position) "
        "VALUES (:name, :employee_id, :department, :position)"
    );

    query.bindValue(":name",name);
    query.bindValue(":employee_id",employeeId);
    query.bindValue(":department",department);
    query.bindValue(":position",position);

    if (!query.exec()) {
        m_db.rollback();
        // MySQL 1062: Duplicate entry，给出更友好的提示
        if (query.lastError().nativeErrorCode() == QStringLiteral("1062")) {
            emit operationResult(false, QStringLiteral("员工工号已存在：%1").arg(employeeId));
        } else {
            emit operationResult(false, QString("人员添加失败：%1").arg(query.lastError().text()));
        }
        return false;
    }

    if (!m_db.commit()) {
        m_db.rollback();
        emit operationResult(false, QString("提交事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    emit operationResult(true, "人员添加成功");
    return true;
}

bool DataManager::updatedPerson(const QString &employeeId, const QVariantMap &updates)
{
    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return false;
    }

    // 构建 SQL SET 子句（字段非空过滤由 DataService 完成，这里仅做白名单保护；值用引号包裹并转义）
    QString setClause;
    static const QSet<QString> allowedFields = {"name","department","position"};
    for (auto it = updates.begin(); it != updates.end(); ++it) {
        const QString &field = it.key();
        if (!allowedFields.contains(field))
            continue;
        if (!setClause.isEmpty())
            setClause += QStringLiteral(", ");
        setClause += field + QLatin1Char('=') + sqlQuotedMysqlString(it.value().toString());
    }
    if (setClause.isEmpty()) {
        emit operationResult(false, "没有可更新的字段");
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    auto rollbackWithError = [&](const QString &msg) -> bool {
        m_db.rollback();
        emit operationResult(false, msg);
        return false;
    };

    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT id FROM Person WHERE employee_id = :employee_id FOR UPDATE");
    selectQuery.bindValue(":employee_id", employeeId);
    if (!selectQuery.exec()) {
        return rollbackWithError(QString("查询人员失败：%1").arg(selectQuery.lastError().text()));
    }
    if (!selectQuery.next()) {
        m_db.rollback();
        emit operationTip("该员工不存在，请确认信息填写是否正确");
        return false;
    }

    const int targetId = selectQuery.value("id").toInt();

    const QString sql = QStringLiteral("UPDATE Person SET ") + setClause
        + QStringLiteral(" WHERE id = ") + QString::number(targetId);
    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        return rollbackWithError(QString("修改人员失败：%1").arg(query.lastError().text()));
    }
    if (!m_db.commit()) {
        return rollbackWithError(QString("提交事务失败：%1").arg(m_db.lastError().text()));
    }

    emit operationResult(true, "修改成功");
    return true;
}

bool DataManager::deletePerson(const QString &employeeId)
{
    if (!m_isConnected) {
        emit operationTip("数据库未连接");
        return false;
    }

    if (employeeId.isEmpty()) {
        emit operationTip("员工工号不能为空");
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    // 失败统一回滚并发出 operationResult
    auto rollbackWithError = [&](const QString &msg) -> bool {
        m_db.rollback();
        emit operationResult(false, msg);
        return false;
    };

    // 1) 锁定人员行（FOR UPDATE；要求 InnoDB）
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT employee_id FROM Person WHERE employee_id = :employee_id FOR UPDATE");
    selectQuery.bindValue(":employee_id", employeeId);
    if (!selectQuery.exec()) {
        return rollbackWithError(QString("查询人员失败：%1").arg(selectQuery.lastError().text()));
    }
    if (!selectQuery.next()) {
        m_db.rollback();
        emit operationTip("该员工不存在，请确认信息填写是否正确");
        return false;
    }

    // 2) 归档：将该员工所有打卡记录连同员工快照一次性写入归档表
    QSqlQuery archiveQuery(m_db);
    archiveQuery.prepare(
        "INSERT INTO AttendanceRecordArchive ("
        "    employee_id, name, department, position, "
        "    check_time, device_id, status, received_time, archive_reason"
        ") "
        "SELECT p.employee_id, p.name, p.department, p.position, "
        "       ar.check_time, ar.device_id, ar.status, ar.received_time, 'resigned' "
        "FROM AttendanceRecord ar "
        "INNER JOIN Person p ON p.employee_id = ar.employee_id "
        "WHERE p.employee_id = :eid"
    );
    archiveQuery.bindValue(":eid", employeeId);
    if (!archiveQuery.exec()) {
        return rollbackWithError(QString("归档打卡记录失败：%1").arg(archiveQuery.lastError().text()));
    }

    // 3) 删除原打卡记录（避免 Person 外键阻拦）
    QSqlQuery deleteAttendanceQuery(m_db);
    deleteAttendanceQuery.prepare("DELETE FROM AttendanceRecord WHERE employee_id = :eid");
    deleteAttendanceQuery.bindValue(":eid", employeeId);
    if (!deleteAttendanceQuery.exec()) {
        return rollbackWithError(QString("删除原打卡记录失败：%1").arg(deleteAttendanceQuery.lastError().text()));
    }

    // 4) 删除人员；face_data 由 ON DELETE CASCADE 级联删除
    QSqlQuery deletePersonQuery(m_db);
    deletePersonQuery.prepare("DELETE FROM Person WHERE employee_id = :eid");
    deletePersonQuery.bindValue(":eid", employeeId);
    if (!deletePersonQuery.exec()) {
        return rollbackWithError(QString("删除人员失败：%1").arg(deletePersonQuery.lastError().text()));
    }

    if (!m_db.commit()) {
        return rollbackWithError(QString("提交事务失败：%1").arg(m_db.lastError().text()));
    }

    emit operationResult(true, "员工已离职：打卡记录已归档，人员与人脸数据已删除");
    return true;
}

QList<QObject *> DataManager::getAllPerson()
{
    QList<QObject*> persons;
    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return persons;
    }

    QSqlQuery query(m_db);
    if(!query.exec("SELECT id, name, employee_id, department, position, created_at, updated_at FROM Person")){
        emit operationResult(false, QString("查询人员列表失败：%1").arg(query.lastError().text()));
        return persons;
    }

    while(query.next()){
        // 不指定 parent，QML 通过 JavaScriptOwnership 接管；C++ 调用方需自行 qDeleteAll。
        Person *person = new Person();
        person->setId(query.value(0).toInt());
        person->setName(query.value(1).toString());
        person->setEmployeeId(query.value(2).toString());
        person->setDepartment(query.value(3).toString());
        person->setPosition(query.value(4).toString());
        person->setCreatedAt(query.value(5).toDateTime());
        person->setUpdatedAt(query.value(6).toDateTime());
        persons.append(person);
    }

    return persons;
}

QObject *DataManager::getPersonById(int id)
{
    if (!m_isConnected) return nullptr;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, employee_id, department, position, created_at, updated_at "
                  "FROM Person WHERE id=:id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        return nullptr;
    }

    Person *person = new Person();
    person->setId(query.value(0).toInt());
    person->setName(query.value(1).toString());
    person->setEmployeeId(query.value(2).toString());
    person->setDepartment(query.value(3).toString());
    person->setPosition(query.value(4).toString());
    person->setCreatedAt(query.value(5).toDateTime());
    person->setUpdatedAt(query.value(6).toDateTime());

    return person;
}

QObject *DataManager::getPersonByEmployeeId(const QString &employeeId)
{
    if (!m_isConnected) return nullptr;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, employee_id, department, position, created_at, updated_at "
                  "FROM Person WHERE employee_id=:employee_id");
    query.bindValue(":employee_id", employeeId);

    if (!query.exec() || !query.next()) {
        return nullptr;
    }

    Person *person = new Person();
    person->setId(query.value(0).toInt());
    person->setName(query.value(1).toString());
    person->setEmployeeId(query.value(2).toString());
    person->setDepartment(query.value(3).toString());
    person->setPosition(query.value(4).toString());
    person->setCreatedAt(query.value(5).toDateTime());
    person->setUpdatedAt(query.value(6).toDateTime());

    return person;
}

namespace{
    QString escapeLikeFragment(const QString &s){
        QString out = s;
        out.replace('\\',QStringLiteral("\\\\"));
        out.replace('%',QStringLiteral("\\%"));
        out.replace('_',QStringLiteral("\\_"));
        return out;
    }
}
QList<QObject*> DataManager::selectPersons(const QString& name, const QString &employeeId, const QString& department, const QString& position)
{
    QList<QObject*> persons;//存储查询结果

    if(!m_isConnected){
        emit operationTip(QStringLiteral("数据库未连接"));
        return persons;
    }

    const QString tName = name.trimmed();
    const QString tEid   = employeeId.trimmed();
    const QString tDept  = department.trimmed();
    const QString tPos  = position.trimmed();

    QStringList conditions;
    if(!tName.isEmpty()){
        conditions << QStringLiteral("name LIKE :name ESCAPE '\\\\'");
    }
    if(!tEid.isEmpty()){
        conditions << QStringLiteral("employee_id = :employee_id");
    }
    if(!tDept.isEmpty()){
        conditions << QStringLiteral("department LIKE :department ESCAPE '\\\\'");
    }
    if(!tPos.isEmpty()){
        conditions << QStringLiteral("position LIKE :position ESCAPE '\\\\'");
    }

    if(conditions.isEmpty()){
        return persons;
    }

    const QString sql = QStringLiteral(
                                "SELECT id, name, employee_id, department, position, created_at, updated_at "
                                "FROM Person WHERE ")
                                + conditions.join(QStringLiteral(" AND "));

    QSqlQuery query(m_db);
    query.prepare(sql);
    if(!tName.isEmpty()){
        query.bindValue(QStringLiteral(":name"),QStringLiteral("%")+escapeLikeFragment(tName)+QStringLiteral("%"));
    }
    if(!tEid.isEmpty()){
        query.bindValue(QStringLiteral(":employee_id"),tEid);
    }
    if(!tDept.isEmpty()){
        query.bindValue(QStringLiteral(":department"),QStringLiteral("%")+escapeLikeFragment(tDept)+QStringLiteral("%"));
    }
    if(!tPos.isEmpty()){
        query.bindValue(QStringLiteral(":position"),QStringLiteral("%") + escapeLikeFragment(tPos) + QStringLiteral("%"));
    }

    if(!query.exec()){
        emit operationResult(false,QStringLiteral("查询人员失败：%1").arg(query.lastError().text()));
        return persons;
    }

    while(query.next()){
        Person *person = new Person();
        person->setId(query.value(0).toInt());
        person->setName(query.value(1).toString());
        person->setEmployeeId(query.value(2).toString());
        person->setDepartment(query.value(3).toString());
        person->setPosition(query.value(4).toString());
        person->setCreatedAt(query.value(5).toDateTime());
        person->setUpdatedAt(query.value(6).toDateTime());

        persons.append(person);
    }

    return persons;
}

bool DataManager::addAttendanceRecord(const QString &employeeId, const QDateTime &checkTime, const QString &deviceId, const QString &status)
{
    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO AttendanceRecord (employee_id, check_time, device_id, status) "
                  "VALUES (:employee_id, :check_time, :device_id, :status)");
    query.bindValue(":employee_id", employeeId);
    query.bindValue(":check_time", checkTime);
    query.bindValue(":device_id", deviceId);
    query.bindValue(":status", status);

    if (!query.exec()) {
        m_db.rollback();
        emit operationResult(false, QString("新增考勤记录失败：%1").arg(query.lastError().text()));
        return false;
    }

    const int newId = query.lastInsertId().toInt();

    if (!m_db.commit()) {
        m_db.rollback();
        emit operationResult(false, QString("提交事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    emit attendanceRecordAdded(newId);
    return true;
}

QList<QObject *> DataManager::selectAttendanceRecord(const QString &employee_id, const QDateTime &startTime,
    const QDateTime &endTime, const QString &deviceId, const QString &status, const QString &personName)
{
    QList<QObject *> records;

    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return records;
    }

    const QString eid = employee_id.trimmed();
    const QString dev = deviceId.trimmed();
    const QString st = status.trimmed();
    const QString tName = personName.trimmed();
    const bool hasStart = startTime.isValid();
    const bool hasEnd = endTime.isValid();

    QStringList conditions;
    if (!eid.isEmpty())
        conditions << QStringLiteral("ar.employee_id = :employee_id");

    if (hasStart && hasEnd)
        conditions << QStringLiteral("ar.check_time BETWEEN :start AND :end");
    else if (hasStart)
        conditions << QStringLiteral("ar.check_time >= :start");
    else if (hasEnd)
        conditions << QStringLiteral("ar.check_time <= :end");

    if (!dev.isEmpty())
        conditions << QStringLiteral("ar.device_id = :device_id");

    if (!st.isEmpty())
        conditions << QStringLiteral("ar.status = :status");

    if (!tName.isEmpty())
        conditions << QStringLiteral("p.name LIKE :person_name ESCAPE '\\\\'");

    if (conditions.isEmpty())
        return records;

    const QString sql = QStringLiteral(
        "SELECT ar.id, ar.employee_id, COALESCE(p.name, ''), ar.check_time, "
        "ar.device_id, ar.status, ar.received_time "
        "FROM AttendanceRecord ar "
        "LEFT JOIN Person p ON p.employee_id = ar.employee_id "
        "WHERE ")
        + conditions.join(QStringLiteral(" AND "))
        + QStringLiteral(" ORDER BY ar.check_time DESC");

    QSqlQuery query(m_db);
    query.prepare(sql);

    if (!eid.isEmpty())
        query.bindValue(QStringLiteral(":employee_id"), eid);

    if (hasStart && hasEnd) {
        query.bindValue(QStringLiteral(":start"), startTime);
        query.bindValue(QStringLiteral(":end"), endTime);
    } else if (hasStart) {
        query.bindValue(QStringLiteral(":start"), startTime);
    } else if (hasEnd) {
        query.bindValue(QStringLiteral(":end"), endTime);
    }

    if (!dev.isEmpty())
        query.bindValue(QStringLiteral(":device_id"), dev);

    if (!st.isEmpty())
        query.bindValue(QStringLiteral(":status"), st);

    if (!tName.isEmpty()) {
        query.bindValue(QStringLiteral(":person_name"),
            QStringLiteral("%") + escapeLikeFragment(tName) + QStringLiteral("%"));
    }

    if (!query.exec()) {
        emit operationResult(false,
            QStringLiteral("查询考勤记录失败：%1").arg(query.lastError().text()));
        return records;
    }

    while (query.next()) {
        AttendanceRecord *record = new AttendanceRecord();
        record->setId(query.value(0).toInt());
        record->setEmployeeId(query.value(1).toString());
        record->setPersonName(query.value(2).toString());
        record->setCheckTime(query.value(3).toDateTime());
        record->setDeviceId(query.value(4).toString());
        record->setStatus(query.value(5).toString());
        record->setReceivedTime(query.value(6).toDateTime());
        records.append(record);
    }

    return records;
}

bool DataManager::addOrUpdateDevice(const QString &deviceId, const QString &deviceName, const QString &ipAddress, const QString &status)
{
    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    // 用 INSERT ... ON DUPLICATE KEY UPDATE 单语句完成新增/更新，避免 TOCTOU 竞态与 QSqlQuery 复用问题
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO Device (device_id, device_name, ip_address, last_online, status) "
        "VALUES (:device_id, :device_name, :ip_address, NOW(), :status) "
        "ON DUPLICATE KEY UPDATE "
        "  device_name = VALUES(device_name), "
        "  ip_address  = VALUES(ip_address), "
        "  last_online = NOW(), "
        "  status      = VALUES(status)"
    );
    query.bindValue(":device_id", deviceId);
    query.bindValue(":device_name", deviceName);
    query.bindValue(":ip_address", ipAddress);
    query.bindValue(":status", status);

    if (!query.exec()) {
        m_db.rollback();
        emit operationResult(false, QString("新增/更新设备失败：%1").arg(query.lastError().text()));
        return false;
    }

    if (!m_db.commit()) {
        m_db.rollback();
        emit operationResult(false, QString("提交事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    emit deviceStatusChanged(deviceId, status);
    return true;
}

bool DataManager::updateDeviceStatus(const QString &deviceId, const QString &status)
{
    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("UPDATE Device SET status=:status, last_online=NOW() WHERE device_id=:device_id");
    query.bindValue(":status", status);
    query.bindValue(":device_id", deviceId);

    if (!query.exec()) {
        m_db.rollback();
        emit operationResult(false, QString("更新设备状态失败：%1").arg(query.lastError().text()));
        return false;
    }

    if (query.numRowsAffected() == 0) {
        m_db.rollback();
        emit operationTip(QStringLiteral("设备不存在：%1").arg(deviceId));
        return false;
    }

    if (!m_db.commit()) {
        m_db.rollback();
        emit operationResult(false, QString("提交事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    emit deviceStatusChanged(deviceId, status);
    return true;
}

QList<QObject *> DataManager::getAllDevices()
{
    QList<QObject*> devices;
    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return devices;
    }

    QSqlQuery query(m_db);
    if (!query.exec("SELECT id, device_id, device_name, ip_address, last_online, status FROM Device")) {
        emit operationResult(false, QString("查询设备列表失败：%1").arg(query.lastError().text()));
        return devices;
    }

    while (query.next()) {
        Device *device = new Device();
        device->setId(query.value(0).toInt());
        device->setDeviceId(query.value(1).toString());
        device->setDeviceName(query.value(2).toString());
        device->setIpAddress(query.value(3).toString());
        device->setLastOnline(query.value(4).toDateTime());
        device->setStatus(query.value(5).toString());
        devices.append(device);
    }

    return devices;
}

QObject *DataManager::getDeviceById(const QString &deviceId)
{
    if (!m_isConnected) return nullptr;

    QSqlQuery query(m_db);
    query.prepare("SELECT id, device_id, device_name, ip_address, last_online, status "
                  "FROM Device WHERE device_id=:device_id");
    query.bindValue(":device_id", deviceId);

    if (!query.exec() || !query.next()) {
        return nullptr;
    }

    Device *device = new Device();
    device->setId(query.value(0).toInt());
    device->setDeviceId(query.value(1).toString());
    device->setDeviceName(query.value(2).toString());
    device->setIpAddress(query.value(3).toString());
    device->setLastOnline(query.value(4).toDateTime());
    device->setStatus(query.value(5).toString());

    return device;
}

QList<QObject *> DataManager::selectDevice(const QString &deviceId, const QString &deviceName,
    const QString &ipAddress, const QString &status)
{
    QList<QObject *> devices;

    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return devices;
    }

    const QString did = deviceId.trimmed();
    const QString dname = deviceName.trimmed();
    const QString ip = ipAddress.trimmed();
    const QString st = status.trimmed();

    QStringList conditions;
    if (!did.isEmpty())
        conditions << QStringLiteral("device_id = :device_id");
    if (!dname.isEmpty())
        conditions << QStringLiteral("device_name LIKE :device_name ESCAPE '\\\\'");
    if (!ip.isEmpty())
        conditions << QStringLiteral("ip_address LIKE :ip_address ESCAPE '\\\\'");
    if (!st.isEmpty())
        conditions << QStringLiteral("status = :status");

    if (conditions.isEmpty())
        return devices;

    const QString sql = QStringLiteral(
        "SELECT id, device_id, device_name, ip_address, last_online, status FROM Device WHERE ")
        + conditions.join(QStringLiteral(" AND "))
        + QStringLiteral(" ORDER BY device_id");

    QSqlQuery query(m_db);
    query.prepare(sql);

    if (!did.isEmpty())
        query.bindValue(QStringLiteral(":device_id"), did);
    if (!dname.isEmpty()) {
        query.bindValue(QStringLiteral(":device_name"),
            QStringLiteral("%") + escapeLikeFragment(dname) + QStringLiteral("%"));
    }
    if (!ip.isEmpty()) {
        query.bindValue(QStringLiteral(":ip_address"),
            QStringLiteral("%") + escapeLikeFragment(ip) + QStringLiteral("%"));
    }
    if (!st.isEmpty())
        query.bindValue(QStringLiteral(":status"), st);

    if (!query.exec()) {
        emit operationResult(false,
            QStringLiteral("查询设备失败：%1").arg(query.lastError().text()));
        return devices;
    }

    while (query.next()) {
        Device *device = new Device();
        device->setId(query.value(0).toInt());
        device->setDeviceId(query.value(1).toString());
        device->setDeviceName(query.value(2).toString());
        device->setIpAddress(query.value(3).toString());
        device->setLastOnline(query.value(4).toDateTime());
        device->setStatus(query.value(5).toString());
        devices.append(device);
    }

    return devices;
}

bool DataManager::createTables()
{
    return createPersonTable()
           && createAttendanceRecordTable()
           && createAttendanceRecordArchiveTable()
           && createDeviceTable()
           && createFaceDataTable();
}

bool DataManager::createPersonTable()
{
    QSqlQuery query(m_db);
    const QString sql = R"SQL(
CREATE TABLE IF NOT EXISTS Person (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    employee_id VARCHAR(100) UNIQUE NOT NULL,
    department VARCHAR(100),
    position VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);
)SQL";

    if(!query.exec(sql)){
        emit operationResult(false, QString("创建 Person 表失败：%1").arg(query.lastError().text()));
        return false;
    }
    return true;
}

// ========== 人脸数据管理实现（基于employee_id） ==========

bool DataManager::addFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector)
{
    if (!m_isConnected) {
        emit operationTip("数据库未连接");
        return false;
    }

    if (employeeId.isEmpty()) {
        emit operationTip("Employee ID 不能为空");
        return false;
    }

    if (featureVector.isEmpty()) {
        emit operationTip("Feature vector 不能为空");
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    auto rollbackWithError = [&](const QString &msg) -> bool {
        m_db.rollback();
        emit operationResult(false, msg);
        return false;
    };

    QSqlQuery existsQuery(m_db);
    existsQuery.prepare("SELECT 1 FROM Person WHERE employee_id = :employee_id FOR UPDATE");
    existsQuery.bindValue(":employee_id", employeeId);

    if (!existsQuery.exec()) {
        return rollbackWithError(QString("查询人员失败：%1").arg(existsQuery.lastError().text()));
    }

    if (!existsQuery.next()) {
        m_db.rollback();
        emit operationTip(QString("员工工号不存在：%1").arg(employeeId));
        return false;
    }

    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT id FROM face_data WHERE employee_id = :employee_id FOR UPDATE");
    checkQuery.bindValue(":employee_id", employeeId);

    if (!checkQuery.exec()) {
        return rollbackWithError(QString("检查人脸数据失败：%1").arg(checkQuery.lastError().text()));
    }

    if (checkQuery.next()) {
        m_db.rollback();
        emit operationTip(QString("该员工已存在人脸数据：%1").arg(employeeId));
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO face_data (employee_id, feature_vector, feature_size) "
        "VALUES (:employee_id, :feature_vector, :feature_size)");
    query.bindValue(":employee_id", employeeId);
    query.bindValue(":feature_vector", featureVector);
    query.bindValue(":feature_size", featureVector.size());

    if (!query.exec()) {
        return rollbackWithError(QString("新增人脸数据失败：%1").arg(query.lastError().text()));
    }

    const int newId = query.lastInsertId().toInt();
    if (!m_db.commit()) {
        return rollbackWithError(QString("提交事务失败：%1").arg(m_db.lastError().text()));
    }

    emit faceDataAdded(newId, employeeId);

    qDebug() << "Face data added for employee:" << employeeId << "with ID:" << newId;
    return true;
}

bool DataManager::updateFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector)
{
    if (!m_isConnected) {
        emit operationTip("数据库未连接");
        return false;
    }

    if (employeeId.isEmpty()) {
        emit operationTip("Employee ID 不能为空");
        return false;
    }

    if (featureVector.isEmpty()) {
        emit operationTip("Feature vector 不能为空");
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    auto rollbackWithError = [&](const QString &msg) -> bool {
        m_db.rollback();
        emit operationResult(false, msg);
        return false;
    };

    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT id FROM face_data WHERE employee_id = :employee_id FOR UPDATE");
    selectQuery.bindValue(":employee_id", employeeId);

    if (!selectQuery.exec()) {
        return rollbackWithError(QString("查询人脸数据失败：%1").arg(selectQuery.lastError().text()));
    }

    if (!selectQuery.next()) {
        m_db.rollback();
        emit operationTip(QString("未找到该员工人脸数据：%1").arg(employeeId));
        return false;
    }

    const int faceDataId = selectQuery.value(0).toInt();

    QSqlQuery query(m_db);
    query.prepare("UPDATE face_data SET feature_vector = :feature_vector, feature_size = :feature_size, "
        "updated_at = CURRENT_TIMESTAMP WHERE id = :id");
    query.bindValue(":feature_vector", featureVector);
    query.bindValue(":feature_size", featureVector.size());
    query.bindValue(":id", faceDataId);

    if (!query.exec()) {
        return rollbackWithError(QString("更新人脸数据失败：%1").arg(query.lastError().text()));
    }

    if (!m_db.commit()) {
        return rollbackWithError(QString("提交事务失败：%1").arg(m_db.lastError().text()));
    }

    emit faceDataUpdated(faceDataId, employeeId);

    qDebug() << "Face data updated for employee:" << employeeId;
    return true;
}

bool DataManager::deleteFaceDataByEmployeeId(const QString& employeeId)
{
    if (!m_isConnected) {
        emit operationTip("数据库未连接");
        return false;
    }

    if (employeeId.isEmpty()) {
        emit operationTip("Employee ID 不能为空");
        return false;
    }

    if (!m_db.transaction()) {
        emit operationResult(false, QString("开启事务失败：%1").arg(m_db.lastError().text()));
        return false;
    }

    auto rollbackWithError = [&](const QString &msg) -> bool {
        m_db.rollback();
        emit operationResult(false, msg);
        return false;
    };

    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT id FROM face_data WHERE employee_id = :employee_id FOR UPDATE");
    selectQuery.bindValue(":employee_id", employeeId);

    if (!selectQuery.exec()) {
        return rollbackWithError(QString("查询人脸数据失败：%1").arg(selectQuery.lastError().text()));
    }

    if (!selectQuery.next()) {
        m_db.rollback();
        emit operationTip(QString("未找到该员工人脸数据：%1").arg(employeeId));
        return false;
    }

    const int faceDataId = selectQuery.value(0).toInt();

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM face_data WHERE id = :id");
    query.bindValue(":id", faceDataId);

    if (!query.exec()) {
        return rollbackWithError(QString("删除人脸数据失败：%1").arg(query.lastError().text()));
    }

    if (!m_db.commit()) {
        return rollbackWithError(QString("提交事务失败：%1").arg(m_db.lastError().text()));
    }

    emit faceDataDeleted(faceDataId);

    qDebug() << "Face data deleted for employee:" << employeeId;
    return true;
}

QObject* DataManager::getFaceDataByEmployeeId(const QString& employeeId)
{
    if (!m_isConnected) {
        return nullptr;
    }

    if (employeeId.isEmpty()) {
        return nullptr;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, employee_id, feature_vector, feature_size, created_at, updated_at "
        "FROM face_data WHERE employee_id = :employee_id"
    );
    query.bindValue(":employee_id", employeeId);

    if (!query.exec()) {
        qWarning() << "Failed to query face data:" << query.lastError().text();
        return nullptr;
    }

    if (!query.next()) {
        qDebug() << "No face data found for employee:" << employeeId;
        return nullptr;
    }

    FaceData* faceData = new FaceData();
    faceData->setId(query.value(0).toInt());
    faceData->setEmployeeId(query.value(1).toString());
    faceData->setFeatureVector(query.value(2).toByteArray());
    faceData->setFeatureSize(query.value(3).toInt());
    faceData->setCreatedAt(query.value(4).toDateTime());
    faceData->setUpdatedAt(query.value(5).toDateTime());

    return faceData;
}

QList<QObject*> DataManager::getAllFaceData()
{
    QList<QObject*> result;

    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return result;
    }

    // 列表场景不返回 feature_vector（LONGBLOB），按需通过 getFaceDataByEmployeeId 获取单条
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, employee_id, feature_size, created_at, updated_at "
        "FROM face_data ORDER BY created_at DESC"
    );

    if (!query.exec()) {
        qWarning() << "Failed to query all face data:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        FaceData* faceData = new FaceData();
        faceData->setId(query.value(0).toInt());
        faceData->setEmployeeId(query.value(1).toString());
        faceData->setFeatureSize(query.value(2).toInt());
        faceData->setCreatedAt(query.value(3).toDateTime());
        faceData->setUpdatedAt(query.value(4).toDateTime());
        faceData->setFeatureVector(QByteArray());
        result.append(faceData);
    }

    qDebug() << "Retrieved" << result.size() << "face data records";
    return result;
}

QList<QObject *> DataManager::selectFaceData(const QString &employeeId, const QDateTime &createdStart,
    const QDateTime &createdEnd, const QDateTime &updatedStart, const QDateTime &updatedEnd)
{
    QList<QObject *> result;

    if (!m_isConnected) {
        emit operationTip(QStringLiteral("数据库未连接"));
        return result;
    }

    const QString eid = employeeId.trimmed();
    const bool hasCs = createdStart.isValid();
    const bool hasCe = createdEnd.isValid();
    const bool hasUs = updatedStart.isValid();
    const bool hasUe = updatedEnd.isValid();

    QStringList conditions;
    if (!eid.isEmpty())
        conditions << QStringLiteral("fd.employee_id = :employee_id");

    if (hasCs && hasCe)
        conditions << QStringLiteral("fd.created_at BETWEEN :created_start AND :created_end");
    else if (hasCs)
        conditions << QStringLiteral("fd.created_at >= :created_start");
    else if (hasCe)
        conditions << QStringLiteral("fd.created_at <= :created_end");

    if (hasUs && hasUe)
        conditions << QStringLiteral("fd.updated_at BETWEEN :updated_start AND :updated_end");
    else if (hasUs)
        conditions << QStringLiteral("fd.updated_at >= :updated_start");
    else if (hasUe)
        conditions << QStringLiteral("fd.updated_at <= :updated_end");

    if (conditions.isEmpty())
        return result;

    const QString sql = QStringLiteral(
        "SELECT fd.id, fd.employee_id, COALESCE(p.name, ''), fd.created_at, fd.updated_at, fd.feature_size "
        "FROM face_data fd "
        "LEFT JOIN Person p ON p.employee_id = fd.employee_id "
        "WHERE ")
        + conditions.join(QStringLiteral(" AND "))
        + QStringLiteral(" ORDER BY fd.created_at DESC");

    QSqlQuery query(m_db);
    query.prepare(sql);

    if (!eid.isEmpty())
        query.bindValue(QStringLiteral(":employee_id"), eid);

    if (hasCs && hasCe) {
        query.bindValue(QStringLiteral(":created_start"), createdStart);
        query.bindValue(QStringLiteral(":created_end"), createdEnd);
    } else if (hasCs) {
        query.bindValue(QStringLiteral(":created_start"), createdStart);
    } else if (hasCe) {
        query.bindValue(QStringLiteral(":created_end"), createdEnd);
    }

    if (hasUs && hasUe) {
        query.bindValue(QStringLiteral(":updated_start"), updatedStart);
        query.bindValue(QStringLiteral(":updated_end"), updatedEnd);
    } else if (hasUs) {
        query.bindValue(QStringLiteral(":updated_start"), updatedStart);
    } else if (hasUe) {
        query.bindValue(QStringLiteral(":updated_end"), updatedEnd);
    }

    if (!query.exec()) {
        emit operationResult(false,
            QStringLiteral("查询人脸数据失败：%1").arg(query.lastError().text()));
        return result;
    }

    while (query.next()) {
        FaceData *faceData = new FaceData();
        faceData->setId(query.value(0).toInt());
        faceData->setEmployeeId(query.value(1).toString());
        faceData->setPersonName(query.value(2).toString());
        faceData->setCreatedAt(query.value(3).toDateTime());
        faceData->setUpdatedAt(query.value(4).toDateTime());
        faceData->setFeatureSize(query.value(5).toInt());
        faceData->setFeatureVector(QByteArray());
        result.append(faceData);
    }

    return result;
}

bool DataManager::createAttendanceRecordTable()
{
    QSqlQuery query(m_db);
    const QString sql = R"SQL(
CREATE TABLE IF NOT EXISTS AttendanceRecord (
    id INT AUTO_INCREMENT PRIMARY KEY,
    employee_id VARCHAR(100) NOT NULL,
    check_time TIMESTAMP NOT NULL,
    device_id VARCHAR(50),
    status VARCHAR(20),
    received_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_employee_id (employee_id),
    FOREIGN KEY (employee_id) REFERENCES Person(employee_id)
);
)SQL";

    if(!query.exec(sql)){
        emit operationResult(false, QString("创建 AttendanceRecord 表失败：%1").arg(query.lastError().text()));
        return false;
    }
    return true;
}

bool DataManager::createAttendanceRecordArchiveTable()
{
    QSqlQuery query(m_db);
    const QString sql = R"SQL(
CREATE TABLE IF NOT EXISTS AttendanceRecordArchive (
    id INT AUTO_INCREMENT PRIMARY KEY,

    employee_id VARCHAR(100) NOT NULL,
    name        VARCHAR(100),
    department  VARCHAR(100),
    position    VARCHAR(100),

    check_time     TIMESTAMP NOT NULL,
    device_id      VARCHAR(50),
    status         VARCHAR(20),
    received_time  TIMESTAMP NULL DEFAULT NULL,

    archived_at    TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    archive_reason VARCHAR(50) DEFAULT 'resigned',

    INDEX idx_employee_id (employee_id),
    INDEX idx_check_time  (check_time)
);
)SQL";

    if (!query.exec(sql)) {
        emit operationResult(false, QString("创建 AttendanceRecordArchive 表失败：%1").arg(query.lastError().text()));
        return false;
    }
    return true;
}

bool DataManager::createDeviceTable()
{
    QSqlQuery query(m_db);
    const QString sql = R"SQL(
CREATE TABLE IF NOT EXISTS Device (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) UNIQUE NOT NULL,
    device_name VARCHAR(100),
    ip_address VARCHAR(50),
    last_online TIMESTAMP,
    status VARCHAR(20) DEFAULT 'offline'
);
)SQL";

    if (!query.exec(sql)) {
        emit operationResult(false, QString("创建 Device 表失败：%1").arg(query.lastError().text()));
        return false;
    }
    return true;
}

bool DataManager::createFaceDataTable()
{
    QSqlQuery query(m_db);
    const QString sql = R"SQL(
CREATE TABLE IF NOT EXISTS face_data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    employee_id VARCHAR(100) NOT NULL UNIQUE,
    feature_vector LONGBLOB NOT NULL,
    feature_size INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (employee_id) REFERENCES Person(employee_id) ON DELETE CASCADE
);
)SQL";

    if (!query.exec(sql)) {
        emit operationResult(false, QString("创建 face_data 表失败：%1").arg(query.lastError().text()));
        qWarning() << "Failed to create face_data table:" << query.lastError().text() << "\nSQL:\n" << sql;
        return false;
    }

    qDebug() << "face_data table created successfully";
    return true;
}