#include "datamanager.h"

DataManager::DataManager(QObject *parent) : QObject(parent),m_isConnected(false) {}

DataManager::~DataManager()
{
    close();
}

bool DataManager::initialize(const QString &host, const QString &database, const QString &username, const QString &password)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName(host);
    m_db.setDatabaseName(database);
    m_db.setUserName(username);
    m_db.setPassword(password);

    if(!m_db.open()){
        emit errorOccurred(m_db.lastError().text());
        return false;
    }

    m_isConnected = true;
    emit connectionStateChanged();

    //创建数据表
    if(!createTables()){
        return false;
    }

    return true;
}

void DataManager::close()
{
    if(m_db.isOpen()){
        m_db.close();
    }

    m_isConnected = false;
    emit connectionStateChanged();
}

bool DataManager::isConnected() const
{
    return m_isConnected;
}

bool DataManager::addPerson(const QString &name, const QString employeeId, const QString &department, const QString &position)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO Person (name, employee_id, department, position) "
        "VALUES (:name, :employee_id, :department, :position)"
    );

    query.bindValue(":name",name);
    query.bindValue(":employee_id",employeeId);
    query.bindValue(":department",department);
    query.bindValue(":position",position);

    if(!query.exec()){
        emit errorOccurred(query.lastError().text());
        return false;
    }

    int newId = query.lastInsertId().toInt();
    emit personAdded(newId);
    return true;
}

bool DataManager::updatedPerson(const QString &name, const QString &employeeId, const QString &department, const QString &position)
{
    //先找到对于的ID
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT id FROM Person WHERE name = :name AND employee_id = :employee_id");
    selectQuery.bindValue(":name",name);
    selectQuery.bindValue(":employee_id",employeeId);
    if(!selectQuery.exec()){
        emit errorOccurred(selectQuery.lastError().text());
        return false;
    }

    if(!selectQuery.next()){
        emit errorOccurred("该员工不存在");
        return false;
    }

    int targetId = selectQuery.value("id").toInt();


    QVariantMap updates;
    updates["name"] = name;
    updates["employee_id"] = employeeId;
    updates["department"] = department;
    updates["position"] = position;

    QVariantMap nonEmptyUpdates;
    static const QSet<QString> allowedFields = {"name","employee_id","department","position"};

    for(auto it = updates.begin();it != updates.end();++it){
        const QString &field = it.key();

        // 检查字段名是否合法
        if(!allowedFields.contains(field)){
            qWarning()<< "Invalid field ignored:" << field;
            continue;
        }

        const QVariant &value = it.value();
        // 判断值是否为空
        // - QVariant::isValid() == false: 无效值 (如默认构造的 QVariant())
        // - value.toString().isEmpty(): 空字符串 ""
        if(value.isValid() && !value.toString().isEmpty()){
            nonEmptyUpdates.insert(field,value);
        }else{
            qDebug()<< "Skipping empty value for field:" << field;
        }
    }

    // 2. 如果没有非空字段要更新，则直接返回成功
    if(nonEmptyUpdates.isEmpty()){
        qDebug() << "请输入需要更新的值" << targetId;
        return true;
    }

    // 3. 构建 SQL 更新语句
    QSqlQuery query(m_db);
    QString setClause = "";
    QList<QPair<QString,QVariant>> bindValus;

    for(auto it = nonEmptyUpdates.begin();it != nonEmptyUpdates.end();++it){
        if(!setClause.isEmpty()) setClause += ", ";
        setClause += it.key() +"=:_" + it.key();
        bindValus.append(qMakePair("_" + it.key(),it.value()));
    }

    QString sql = QString("UPDATE Person SET %1 WHERE id = :id").arg(setClause);

    // 4. 绑定参数
    for(const auto &pair:bindValus){
        query.bindValue(pair.first,pair.second);
    }
    query.bindValue(":id",targetId);

    // 5. 执行并返回结果
    if(!query.exec()){
        emit errorOccurred(query.lastError().text());
        return false;
    }

    emit personUpdated(targetId);
    return true;
}

bool DataManager::deletePerson(const QString &name,const QString &employeeId)
{
    //先找到对于的ID
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT id FROM Person WHERE name = :name AND employee_id = :employee_id");
    selectQuery.bindValue(":name",name);
    selectQuery.bindValue(":employee_id",employeeId);
    if(!selectQuery.exec()){
        emit errorOccurred(selectQuery.lastError().text());
        return false;
    }

    if(!selectQuery.next()){
        emit errorOccurred("该员工不存在");
        return false;
    }

    int targetId = selectQuery.value("id").toInt();

    //根据ID删除（这样更安全，避免并发问题
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM Person WHERE id = :id");
    deleteQuery.bindValue(":id",targetId);
    if(!deleteQuery.exec()){
        emit errorOccurred(deleteQuery.lastError().text());
        return false;
    }

    emit personDeleted(targetId);
    return true;
}

QList<QObject *> DataManager::getAllPerson()
{
    QList<QObject*> persons;
    QSqlQuery query(m_db);

    if(!query.exec("SELECT id, name, employee_id, department, position, created_at, updated_at FROM Person")){
        emit errorOccurred(query.lastError().text());
        return persons;
    }

    while(query.next()){
        Person *person = new Person(this);
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
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, employee_id, department, position, created_at, updated_at "
                  "FROM Person WHERE id=:id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        return nullptr;
    }

    Person *person = new Person(this);
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
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, employee_id, department, position, created_at, updated_at "
                  "FROM Person WHERE employee_id=:employee_id");
    query.bindValue(":employee_id", employeeId);

    if (!query.exec() || !query.next()) {
        return nullptr;
    }

    Person *person = new Person(this);
    person->setId(query.value(0).toInt());
    person->setName(query.value(1).toString());
    person->setEmployeeId(query.value(2).toString());
    person->setDepartment(query.value(3).toString());
    person->setPosition(query.value(4).toString());
    person->setCreatedAt(query.value(5).toDateTime());
    person->setUpdatedAt(query.value(6).toDateTime());

    return person;
}

bool DataManager::updatePersonFaceFeature(int id, const QByteArray &faceFeature)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Person SET face_feature = :face_feature WHERE id = :id");
    query.bindValue(":id",id);
    query.bindValue(":face_feature",faceFeature);

    if (!query.exec()) {
        emit errorOccurred(query.lastError().text());
        return false;
    }

    return true;
}

bool DataManager::addAttendanceRecore(int personId, const QDateTime &checkTime, const QString &deviceId, const QString &status)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO AttendanceRecord (person_id, check_time, device_id, status) "
                  "VALUES (:person_id, :check_time, :device_id, :status)");
    query.bindValue(":person_id", personId);
    query.bindValue(":check_time", checkTime);
    query.bindValue(":device_id", deviceId);
    query.bindValue(":status", status);

    if (!query.exec()) {
        emit errorOccurred(query.lastError().text());
        return false;
    }

    int newId = query.lastInsertId().toInt();
    emit attendanceRecordAdded(newId);
    return true;
}

QList<QObject *> DataManager::getAttendanceRecords(const QDateTime &startTime, const QDateTime &endTime)
{
    QList<QObject*> records;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, person_id, check_time, device_id, status, received_time "
                  "FROM AttendanceRecord WHERE check_time BETWEEN :start AND :end "
                  "ORDER BY check_time DESC");
    query.bindValue(":start", startTime);
    query.bindValue(":end", endTime);

    if (!query.exec()) {
        emit errorOccurred(query.lastError().text());
        return records;
    }

    while (query.next()) {
        AttendanceRecord *record = new AttendanceRecord(this);
        record->setId(query.value(0).toInt());
        record->setPersonId(query.value(1).toInt());
        record->setCheckTime(query.value(2).toDateTime());
        record->setDeviceId(query.value(3).toString());
        record->setStatus(query.value(4).toString());
        record->setReceivedTime(query.value(5).toDateTime());
        records.append(record);
    }

    return records;
}

QList<QObject *> DataManager::getAttendanceRecordsByPerson(int personId, const QDateTime &startTime, const QDateTime &endTime)
{
    QList<QObject*> records;
    QSqlQuery query(m_db);
    query.prepare("SELECT id, person_id, check_time, device_id, status, received_time "
                  "FROM AttendanceRecord WHERE person_id=:person_id AND "
                  "check_time BETWEEN :start AND :end ORDER BY check_time DESC");
    query.bindValue(":person_id", personId);
    query.bindValue(":start", startTime);
    query.bindValue(":end", endTime);

    if (!query.exec()) {
        emit errorOccurred(query.lastError().text());
        return records;
    }

    while (query.next()) {
        AttendanceRecord *record = new AttendanceRecord(this);
        record->setId(query.value(0).toInt());
        record->setPersonId(query.value(1).toInt());
        record->setCheckTime(query.value(2).toDateTime());
        record->setDeviceId(query.value(3).toString());
        record->setStatus(query.value(4).toString());
        record->setReceivedTime(query.value(5).toDateTime());
        records.append(record);
    }

    return records;
}

bool DataManager::addOrUpdateDevice(const QString &deviceId, const QString &deviceName, const QString &ipAddress, const QString &status)
{
    QSqlQuery query(m_db);

    // 先检查设备是否存在
    query.prepare("SELECT id FROM Device WHERE device_id=:device_id");
    query.bindValue(":device_id", deviceId);

    if (query.exec() && query.next()) {
        // 更新现有设备
        query.prepare("UPDATE Device SET device_name=:device_name, ip_address=:ip_address, "
                      "last_online=NOW(), status=:status WHERE device_id=:device_id");
    } else {
        // 插入新设备
        query.prepare("INSERT INTO Device (device_id, device_name, ip_address, last_online, status) "
                      "VALUES (:device_id, :device_name, :ip_address, NOW(), :status)");
    }

    query.bindValue(":device_id", deviceId);
    query.bindValue(":device_name", deviceName);
    query.bindValue(":ip_address", ipAddress);
    query.bindValue(":status", status);

    if (!query.exec()) {
        emit errorOccurred(query.lastError().text());
        return false;
    }

    emit deviceStatusChanged(deviceId, status);
    return true;
}

bool DataManager::updateDeviceStatus(const QString &deviceId, const QString &status)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE Device SET status=:status, last_online=NOW() WHERE device_id=:device_id");
    query.bindValue(":status", status);
    query.bindValue(":device_id", deviceId);

    if (!query.exec()) {
        emit errorOccurred(query.lastError().text());
        return false;
    }

    emit deviceStatusChanged(deviceId, status);
    return true;
}

QList<QObject *> DataManager::getAllDevices()
{
    QList<QObject*> devices;
    QSqlQuery query(m_db);

    if (!query.exec("SELECT id, device_id, device_name, ip_address, last_online, status FROM Device")) {
        emit errorOccurred(query.lastError().text());
        return devices;
    }

    while (query.next()) {
        Device *device = new Device(this);
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
    QSqlQuery query(m_db);
    query.prepare("SELECT id, device_id, device_name, ip_address, last_online, status "
                  "FROM Device WHERE device_id=:device_id");
    query.bindValue(":device_id", deviceId);

    if (!query.exec() || !query.next()) {
        return nullptr;
    }

    Device *device = new Device(this);
    device->setId(query.value(0).toInt());
    device->setDeviceId(query.value(1).toString());
    device->setDeviceName(query.value(2).toString());
    device->setIpAddress(query.value(3).toString());
    device->setLastOnline(query.value(4).toDateTime());
    device->setStatus(query.value(5).toString());

    return device;
}

bool DataManager::createTables()
{
    return createPersonTable() && createAttendanceRecordTable() &&createDeviceTable();
}

bool DataManager::createPersonTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
CREATE TABLE IF NOT EXISTS Person (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    employee_id VARCHAR(100) UNIQUE NOT NULL,
    department VARCHAR(100),
    position VARCHAR(100),
    face_feature BLOB,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
)
)";

    if(!query.exec(sql)){
        emit errorOccurred(query.lastError().text());
        return false;
    }
    return true;
}

bool DataManager::createAttendanceRecordTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
CREATE TABLE IF NOT EXISTS AttendanceRecord (
    id INT AUTO_INCREMENT PRIMARY KEY,
    person_id INT NOT NULL,
    check_time TIMESTAMP NOT NULL,
    device_id VARCHAR(50),
    status VARCHAR(20),
    received_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (person_id) REFERENCES Person(id)
)
)";

    if(!query.exec(sql)){
        emit errorOccurred(query.lastError().text());
        return false;
    }
    return true;
}

bool DataManager::createDeviceTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
CREATE TABLE IF NOT EXISTS Device (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) UNIQUE NOT NULL,
    device_name VARCHAR(100),
    ip_address VARCHAR(50),
    last_online TIMESTAMP,
    status VARCHAR(20) DEFAULT 'offline'
)
)";

    if (!query.exec(sql)) {
        emit errorOccurred(query.lastError().text());
        return false;
    }
    return true;
}
