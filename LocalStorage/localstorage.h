#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>

#include "../NetworkClient/serverprotocol.h"
#include "FaceFeatureRepository.h"
#include "AttendanceOutboxRepository.h"
#include "SyncMetaRepository.h"
#include "DeviceLocalRepository.h"

class LocalStorage : public QObject
{
    Q_OBJECT

public:
    static LocalStorage* instance();

    bool connectDatabse();

    // ---- Repository accessors (for phase-3 code) ----
    FaceFeatureRepository&      faceFeatures();
    AttendanceOutboxRepository& outbox();
    SyncMetaRepository&         syncMeta();
    DeviceLocalRepository&      deviceLocal();

    // ---- Backward-compatible API (delegates to repositories) ----
    bool syncPersons(const QVector<ServerProtocol::PersonData> &persons);
    bool addAttendanceRecord(const QString &employeeId, const QString &status);
    QVector<ServerProtocol::AttendanceRecord> getUnuploadedRecords();
    bool markAsUploaded(int recordId);
    bool markBatchAsUploaded(const QVector<int> &recordIds);

signals:
    void personsSyncCompleted(int count);
    void personsSyncFailed(const QString &error);

private:
    explicit LocalStorage(QObject *parent = nullptr);
    ~LocalStorage();

    bool runMigrations();
    bool runInitialSchema();
    bool ensureSchemaVersionTable();

    QSqlDatabase m_db;
    static QMutex s_mutex;
    static LocalStorage* s_instance;

    FaceFeatureRepository*      m_faceFeatures = nullptr;
    AttendanceOutboxRepository* m_outbox = nullptr;
    SyncMetaRepository*         m_syncMeta = nullptr;
    DeviceLocalRepository*      m_deviceLocal = nullptr;
};
