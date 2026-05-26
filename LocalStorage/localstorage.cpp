#include "localstorage.h"
#include "../Config/configmanager.h"
#include "../Utils/DatabaseManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

LocalStorage* LocalStorage::s_instance = nullptr;
QMutex LocalStorage::s_mutex;

static const char* kInitialSchema = R"SQL(
CREATE TABLE IF NOT EXISTS face_feature (
    employee_id     TEXT NOT NULL PRIMARY KEY,
    feature_blob    BLOB NOT NULL,
    feature_size    INTEGER NOT NULL,
    updated_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
    sync_generation INTEGER NOT NULL DEFAULT 0
);
CREATE INDEX IF NOT EXISTS idx_facefeature_sync_gen ON face_feature(sync_generation);

CREATE TABLE IF NOT EXISTS attendance_outbox (
    id             INTEGER PRIMARY KEY AUTOINCREMENT,
    client_msg_id  TEXT NOT NULL UNIQUE,
    employee_id    TEXT NOT NULL,
    check_time     DATETIME NOT NULL,
    status         TEXT NOT NULL DEFAULT 'ok',
    photo_blob     BLOB,
    photo_size     INTEGER DEFAULT 0,
    created_at     DATETIME DEFAULT CURRENT_TIMESTAMP,
    retry_count    INTEGER NOT NULL DEFAULT 0,
    last_error     TEXT,
    state          TEXT NOT NULL DEFAULT 'pending'
        CHECK (state IN ('pending','sending','failed','dead'))
);
CREATE INDEX IF NOT EXISTS idx_outbox_state ON attendance_outbox(state);
CREATE INDEX IF NOT EXISTS idx_outbox_employee_id ON attendance_outbox(employee_id);

CREATE TABLE IF NOT EXISTS sync_meta (
    id                       INTEGER PRIMARY KEY CHECK (id = 1),
    current_generation       INTEGER NOT NULL DEFAULT 1,
    staging_generation       INTEGER NOT NULL DEFAULT 0,
    last_sync_request_msg_id TEXT,
    last_sync_ok_at          DATETIME,
    last_sync_status         TEXT,
    face_count               INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS device_local (
    id          INTEGER PRIMARY KEY CHECK (id = 1),
    device_id   TEXT NOT NULL,
    device_name TEXT NOT NULL DEFAULT '',
    ip_address  TEXT NOT NULL DEFAULT '',
    fw_version  TEXT NOT NULL DEFAULT '1.0.0'
);

CREATE TABLE IF NOT EXISTS schema_version (
    version    INTEGER PRIMARY KEY,
    applied_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

INSERT OR REPLACE INTO schema_version (version) VALUES (1);
)SQL";

LocalStorage* LocalStorage::instance()
{
    if (!s_instance) {
        QMutexLocker locker(&s_mutex);
        if (!s_instance) {
            s_instance = new LocalStorage();
        }
    }
    return s_instance;
}

LocalStorage::LocalStorage(QObject *parent)
    : QObject(parent)
{
}

LocalStorage::~LocalStorage()
{
    delete m_faceFeatures;
    delete m_outbox;
    delete m_syncMeta;
    delete m_deviceLocal;
    if (m_db.isOpen()) {
        m_db.close();
    }
}

FaceFeatureRepository& LocalStorage::faceFeatures()
{
    return *m_faceFeatures;
}

AttendanceOutboxRepository& LocalStorage::outbox()
{
    return *m_outbox;
}

SyncMetaRepository& LocalStorage::syncMeta()
{
    return *m_syncMeta;
}

DeviceLocalRepository& LocalStorage::deviceLocal()
{
    return *m_deviceLocal;
}

bool LocalStorage::connectDatabse()
{
    ConfigManager* config = ConfigManager::instance();
    QString dbFilePath = config->getDatabasePath();

    if (dbFilePath.isEmpty()) {
        dbFilePath = ConfigManager::getDefaultDatabasePath();
        config->setDatabasePath(dbFilePath);
        config->saveConfig();
    }

    QFileInfo dbFileInfo(dbFilePath);
    QString dbDir = dbFileInfo.path();
    QDir dataDir;
    if (!dataDir.exists(dbDir)) {
        if (!dataDir.mkpath(dbDir)) {
            return false;
        }
    }

    m_dbPath = dbFilePath;

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbFilePath);

    if (!m_db.open()) {
        return false;
    }

    {
        QSqlQuery q(m_db);
        q.exec("PRAGMA journal_mode=WAL");
        q.exec("PRAGMA foreign_keys=ON");
        q.exec("PRAGMA encoding='UTF-8'");
    }

    if (!runMigrations()) {
        m_db.close();
        return false;
    }

    m_faceFeatures = new FaceFeatureRepository(m_dbPath);
    m_outbox       = new AttendanceOutboxRepository(m_dbPath);
    m_syncMeta     = new SyncMetaRepository(m_dbPath);
    m_deviceLocal  = new DeviceLocalRepository(m_dbPath);

    m_syncMeta->ensureRow();
    m_deviceLocal->ensureRow(
        config->getDeviceId().isEmpty() ? QStringLiteral("device_001") : config->getDeviceId(),
        config->getDeviceId(),
        config->getFwVersion());

    return true;
}

bool LocalStorage::ensureSchemaVersionTable()
{
    QSqlQuery q(m_db);
    return q.exec("CREATE TABLE IF NOT EXISTS schema_version ("
                  "version INTEGER PRIMARY KEY,"
                  "applied_at DATETIME DEFAULT CURRENT_TIMESTAMP)");
}

bool LocalStorage::runMigrations()
{
    if (!ensureSchemaVersionTable()) {
        return false;
    }

    int currentVersion = 0;
    {
        QSqlQuery q(m_db);
        if (q.exec("SELECT MAX(version) FROM schema_version") && q.next()) {
            currentVersion = q.value(0).toInt();
        }
    }

    if (currentVersion < 1) {
        if (!runInitialSchema()) {
            return false;
        }
        currentVersion = 1;
    }

    return true;
}

bool LocalStorage::runInitialSchema()
{
    {
        QSqlQuery q(m_db);
        q.exec("DROP TABLE IF EXISTS face_feature");
        q.exec("DROP TABLE IF EXISTS attendance_outbox");
        q.exec("DROP TABLE IF EXISTS sync_meta");
        q.exec("DROP TABLE IF EXISTS device_local");
        q.exec("DROP TABLE IF EXISTS schema_version");
        q.exec("DROP TABLE IF EXISTS AttendanceRecord");
        q.exec("DROP TABLE IF EXISTS Person");
    }

    QStringList statements = {
        R"SQL(
        CREATE TABLE IF NOT EXISTS face_feature (
            employee_id     TEXT NOT NULL PRIMARY KEY,
            feature_blob    BLOB NOT NULL,
            feature_size    INTEGER NOT NULL,
            updated_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
            sync_generation INTEGER NOT NULL DEFAULT 0
        ))SQL",

        "CREATE INDEX IF NOT EXISTS idx_facefeature_sync_gen ON face_feature(sync_generation)",

        R"SQL(
        CREATE TABLE IF NOT EXISTS attendance_outbox (
            id             INTEGER PRIMARY KEY AUTOINCREMENT,
            client_msg_id  TEXT NOT NULL UNIQUE,
            employee_id    TEXT NOT NULL,
            check_time     DATETIME NOT NULL,
            status         TEXT NOT NULL DEFAULT 'ok',
            photo_blob     BLOB,
            photo_size     INTEGER DEFAULT 0,
            created_at     DATETIME DEFAULT CURRENT_TIMESTAMP,
            retry_count    INTEGER NOT NULL DEFAULT 0,
            last_error     TEXT,
            state          TEXT NOT NULL DEFAULT 'pending'
                CHECK (state IN ('pending','sending','failed','dead'))
        ))SQL",

        "CREATE INDEX IF NOT EXISTS idx_outbox_state ON attendance_outbox(state)",
        "CREATE INDEX IF NOT EXISTS idx_outbox_employee_id ON attendance_outbox(employee_id)",

        R"SQL(
        CREATE TABLE IF NOT EXISTS sync_meta (
            id                       INTEGER PRIMARY KEY CHECK (id = 1),
            current_generation       INTEGER NOT NULL DEFAULT 1,
            staging_generation       INTEGER NOT NULL DEFAULT 0,
            last_sync_request_msg_id TEXT,
            last_sync_ok_at          DATETIME,
            last_sync_status         TEXT,
            face_count               INTEGER DEFAULT 0
        ))SQL",

        R"SQL(
        CREATE TABLE IF NOT EXISTS device_local (
            id          INTEGER PRIMARY KEY CHECK (id = 1),
            device_id   TEXT NOT NULL,
            device_name TEXT NOT NULL DEFAULT '',
            ip_address  TEXT NOT NULL DEFAULT '',
            fw_version  TEXT NOT NULL DEFAULT '1.0.0'
        ))SQL",

        R"SQL(
        CREATE TABLE IF NOT EXISTS schema_version (
            version    INTEGER PRIMARY KEY,
            applied_at DATETIME DEFAULT CURRENT_TIMESTAMP
        ))SQL",

        "INSERT OR REPLACE INTO schema_version (version) VALUES (1)"
    };

    QSqlQuery q(m_db);
    for (const QString &stmt : statements) {
        if (!q.exec(stmt)) {
            return false;
        }
    }

    return true;
}

bool LocalStorage::syncPersons(const QVector<ServerProtocol::PersonData> &persons)
{
    QMutexLocker locker(&s_mutex);

    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    if (!db.isOpen()) {
        emit personsSyncFailed("数据库未连接");
        return false;
    }

    if (!db.transaction()) {
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO face_feature "
                  "(employee_id, feature_blob, feature_size, updated_at, sync_generation) "
                  "VALUES (:eid, :blob, :size, datetime('now'), 1)");

    int successCount = 0;
    for (const auto &person : persons) {
        if (person.faceFeature.isEmpty() || person.featureSize <= 0) {
            continue;
        }
        query.bindValue(":eid",  person.employeeId);
        query.bindValue(":blob", person.faceFeature);
        query.bindValue(":size", person.featureSize);

        if (!query.exec()) {
            db.rollback();
            emit personsSyncFailed(QString("插入人脸特征失败: %1").arg(person.employeeId));
            return false;
        }
        successCount++;
    }

    if (!db.commit()) {
        db.rollback();
        emit personsSyncFailed("提交事务失败");
        return false;
    }

    emit personsSyncCompleted(successCount);
    return true;
}

bool LocalStorage::addAttendanceRecord(const QString &employeeId, const QString &status)
{
    QMutexLocker locker(&s_mutex);

    if (!m_db.isOpen()) {
        return false;
    }

    OutboxRecord record;
    record.clientMsgId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    record.employeeId  = employeeId;
    record.checkTime   = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    record.status      = status;
    record.state       = QStringLiteral("pending");

    return m_outbox->enqueue(record);
}

QVector<ServerProtocol::AttendanceRecord> LocalStorage::getUnuploadedRecords()
{
    QMutexLocker locker(&s_mutex);
    QVector<ServerProtocol::AttendanceRecord> records;

    if (!m_db.isOpen()) {
        return records;
    }

    auto pending = m_outbox->fetchPending(-1);
    for (const auto &r : pending) {
        ServerProtocol::AttendanceRecord rec;
        rec.employeeId = r.employeeId;
        rec.checkTime  = r.checkTime;
        rec.status     = r.status;
        records.append(rec);
    }

    return records;
}

bool LocalStorage::markAsUploaded(int recordId)
{
    QMutexLocker locker(&s_mutex);

    if (!m_db.isOpen()) {
        return false;
    }

    return m_outbox->remove(recordId);
}

bool LocalStorage::markBatchAsUploaded(const QVector<int> &recordIds)
{
    QMutexLocker locker(&s_mutex);

    if (!m_db.isOpen()) {
        return false;
    }

    for (int id : recordIds) {
        if (!m_outbox->remove(id)) {
            return false;
        }
    }
    return true;
}
