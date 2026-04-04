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
        emit errorOccurrend(m_db.lastError().text());
        return false;
    }

    m_isConnected = true;
    emit connectionStateChaned();

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
    emit connectionStateChaned();
}

bool DataManager::isConnected() const
{
    return m_isConnected;
}

bool DataManager::addPerson(const QString &name, const QString employeeId, const QString &department, const QString &position)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO Person (name,employee_id,department,position)"
                  "VALUSE (:name,:employee_id,:department,:position)");

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

bool DataManager::updatedPerson(int id, const QString &name, const QString &employeeId, const QString &department, const QString &position)
{

}

bool DataManager::deletePerson(int id)
{

}

QList<QObject *> DataManager::getAllPerson()
{

}

QObject *DataManager::getPersonById(int id)
{

}

QObject *DataManager::getPersonByEmployeeId(const QString &employee)
{

}

bool DataManager::updatePersonFaceFeature(int id, const QByteArray &faceFeature)
{

}

bool DataManager::addAttendanceRecore(int personId, const QDateTime &checkTime, const QString &deviceId, const QString &status)
{

}

QList<QObject *> DataManager::getAttendanceRecords(const QDateTime &startTime, const QDateTime &endTime)
{

}

QList<QObject *> DataManager::getAttendanceRecordsByPerson(int personId, const QDateTime &startTime, const QDateTime &endTime)
{

}

bool DataManager::addOrUpdateDevice(const QString &deviceId, const QString &deviceName, const QString &ipAddress, const QString &status)
{

}

bool DataManager::updateDeviceStatus(const QString &deviceId, const QString &status)
{

}

QList<QObject *> DataManager::getAllDevices()
{

}

QObject *DataManager::getDeviceById(const QString &deviceId)
{

}

bool DataManager::createTables()
{
    return createTables() && createAttendanceRecordTable() &&createDeviceTable();
}

bool DataManager::createPersonTable()
{
    QSqlQuery query(m_db);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS Person (
            id INT AUTO_INCREMENT PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            employee_Id VARCHAR(100) UNIQUE NOT NULL,
            department VARCHAR(100),
            position VARCHAR(100),
            face_feature BLOB,
            create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            update_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
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
        CREATE TABLE IF NOT EXISTS AttendanceRecordTable(
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
