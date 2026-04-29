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
    return createPersonTable() && createAttendanceRecordTable() &&createDeviceTable() && createFaceDataTable();
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
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP;
)
)";

    if(!query.exec(sql)){
        emit errorOccurred(query.lastError().text());
        return false;
    }
    return true;
}

// ========== 人脸数据管理实现（基于employee_id） ==========

bool DataManager::addFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector)
{
    if (!m_isConnected) {
        emit errorOccurred("Database not connected");
        return false;
    }

    if (employeeId.isEmpty()) {
        emit errorOccurred("Employee ID cannot be empty");
        return false;
    }

    if (featureVector.isEmpty()) {
        emit errorOccurred("Feature vector cannot be empty");
        return false;
    }

    // 先根据employee_id获取person_id
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare("SELECT id FROM Person WHERE employee_id = :employee_id");
    selectQuery.bindValue(":employee_id", employeeId);

    if (!selectQuery.exec()) {
        emit errorOccurred("Failed to query person: " + selectQuery.lastError().text());
        return false;
    }

    if (!selectQuery.next()) {
        emit errorOccurred(QString("Employee with ID %1 not found").arg(employeeId));
        return false;
    }

    int personId = selectQuery.value(0).toInt();

    // 检查是否已存在该员工的人脸数据
    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT id FROM face_data WHERE person_id = :person_id");
    checkQuery.bindValue(":person_id", personId);

    if (!checkQuery.exec()) {
        emit errorOccurred("Failed to check face data: " + checkQuery.lastError().text());
        return false;
    }

    if (checkQuery.next()) {
        emit errorOccurred(QString("Face data already exists for employee %1").arg(employeeId));
        return false;
    }

    // 插入人脸数据
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO face_data (person_id, feature_vector, feature_size, status) "
        "VALUES (:person_id, :feature_vector, :feature_size, :status)");
    query.bindValue(":person_id", personId);
    query.bindValue(":feature_vector", featureVector);
    query.bindValue(":feature_size", featureVector.size());
    query.bindValue(":status", "active");

    if (!query.exec()) {
        emit errorOccurred("Failed to add face data: " + query.lastError().text());
        return false;
    }

    int newId = query.lastInsertId().toInt();
    updatePersonFaceFeature(personId, featureVector);
    emit faceDataAdded(newId, personId);

    qDebug() << "Face data added for employee:" << employeeId << "with ID:" << newId;
    return true;
}

bool DataManager::updateFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector)
{
    if (!m_isConnected) {
        emit errorOccurred("Database not connected");
        return false;
    }

    if (employeeId.isEmpty()) {
        emit errorOccurred("Employee ID cannot be empty");
        return false;
    }

    if (featureVector.isEmpty()) {
        emit errorOccurred("Feature vector cannot be empty");
        return false;
    }

    // 先根据employee_id获取person_id和face_data_id
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare(
        "SELECT fd.id, fd.person_id FROM face_data fd "
        "INNER JOIN Person p ON fd.person_id = p.id "
        "WHERE p.employee_id = :employee_id"
    );
    selectQuery.bindValue(":employee_id", employeeId);

    if (!selectQuery.exec()) {
        emit errorOccurred("Failed to query face data: " + selectQuery.lastError().text());
        return false;
    }

    if (!selectQuery.next()) {
        emit errorOccurred(QString("Face data not found for employee %1").arg(employeeId));
        return false;
    }

    int faceDataId = selectQuery.value(0).toInt();
    int personId = selectQuery.value(1).toInt();

    // 更新人脸特征
    QSqlQuery query(m_db);
    query.prepare("UPDATE face_data SET feature_vector = :feature_vector, feature_size = :feature_size, "
        "updated_at = CURRENT_TIMESTAMP WHERE id = :id");
    query.bindValue(":feature_vector", featureVector);
    query.bindValue(":feature_size", featureVector.size());
    query.bindValue(":id", faceDataId);

    if (!query.exec()) {
        emit errorOccurred("Failed to update face data: " + query.lastError().text());
        return false;
    }

    updatePersonFaceFeature(personId, featureVector);
    emit faceDataUpdated(faceDataId, personId);

    qDebug() << "Face data updated for employee:" << employeeId;
    return true;
}

bool DataManager::deleteFaceDataByEmployeeId(const QString& employeeId)
{
    if (!m_isConnected) {
        emit errorOccurred("Database not connected");
        return false;
    }

    if (employeeId.isEmpty()) {
        emit errorOccurred("Employee ID cannot be empty");
        return false;
    }

    // 先根据employee_id获取face_data_id
    QSqlQuery selectQuery(m_db);
    selectQuery.prepare(
        "SELECT fd.id FROM face_data fd "
        "INNER JOIN Person p ON fd.person_id = p.id "
        "WHERE p.employee_id = :employee_id"
    );
    selectQuery.bindValue(":employee_id", employeeId);

    if (!selectQuery.exec()) {
        emit errorOccurred("Failed to query face data: " + selectQuery.lastError().text());
        return false;
    }

    if (!selectQuery.next()) {
        emit errorOccurred(QString("Face data not found for employee %1").arg(employeeId));
        return false;
    }

    int faceDataId = selectQuery.value(0).toInt();

    // 删除数据库记录
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM face_data WHERE id = :id");
    query.bindValue(":id", faceDataId);

    if (!query.exec()) {
        emit errorOccurred("Failed to delete face data: " + query.lastError().text());
        return false;
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
        "SELECT fd.id, fd.person_id, fd.feature_vector, fd.feature_size, fd.status, fd.created_at, fd.updated_at "
        "FROM face_data fd "
        "INNER JOIN Person p ON fd.person_id = p.id "
        "WHERE p.employee_id = :employee_id"
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
    faceData->setPersonId(query.value(1).toInt());
    faceData->setFeatureVector(query.value(2).toByteArray());
    faceData->setFeatureSize(query.value(3).toInt());
    faceData->setStatus(query.value(4).toString());
    faceData->setCreatedAt(query.value(5).toDateTime());
    faceData->setUpdatedAt(query.value(6).toDateTime());

    return faceData;
}

QList<QObject*> DataManager::getAllFaceData()
{
    QList<QObject*> result;

    if (!m_isConnected) {
        return result;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, person_id, feature_vector, feature_size, status, created_at, updated_at "
        "FROM face_data ORDER BY created_at DESC"
    );

    if (!query.exec()) {
        qWarning() << "Failed to query all face data:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        FaceData* faceData = new FaceData();
        faceData->setId(query.value(0).toInt());
        faceData->setPersonId(query.value(1).toInt());
        faceData->setFeatureVector(query.value(2).toByteArray());
        faceData->setFeatureSize(query.value(3).toInt());
        faceData->setStatus(query.value(4).toString());
        faceData->setCreatedAt(query.value(5).toDateTime());
        faceData->setUpdatedAt(query.value(6).toDateTime());
        result.append(faceData);
    }

    qDebug() << "Retrieved" << result.size() << "face data records";
    return result;
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

bool DataManager::createFaceDataTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
CREATE TABLE IF NOT EXISTS face_data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    person_id INT NOT NULL,
    feature_vector LONGBLOB NOT NULL,
    feature_size INT NOT NULL,
    status VARCHAR(20) DEFAULT 'active' COMMENT '状态: active, inactive, failed',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (person_id) REFERENCES Person(id) ON DELETE CASCADE,
    INDEX idx_person_id (person_id),
    INDEX idx_status (status)
);)";

    if (!query.exec(sql)) {
        emit errorOccurred(query.lastError().text());
        qWarning() << "Failed to create face_data table:" << query.lastError().text();
        return false;
    }

    qDebug() << "face_data table created successfully";
    return true;
}