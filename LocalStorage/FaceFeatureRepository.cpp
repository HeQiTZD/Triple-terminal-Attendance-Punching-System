#include "FaceFeatureRepository.h"
#include "../Utils/DatabaseManager.h"
#include <QSqlError>
#include <QSqlQuery>

FaceFeatureRepository::FaceFeatureRepository(const QString &dbPath)
    : m_dbPath(dbPath)
{
}

bool FaceFeatureRepository::insertOrReplace(const QString &employeeId,
                                            const QByteArray &featureBlob,
                                            int featureSize,
                                            int syncGeneration)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO face_feature "
                  "(employee_id, feature_blob, feature_size, updated_at, sync_generation) "
                  "VALUES (:eid, :blob, :size, datetime('now'), :gen)");
    query.bindValue(":eid", employeeId);
    query.bindValue(":blob", featureBlob);
    query.bindValue(":size", featureSize);
    query.bindValue(":gen", syncGeneration);

    if (!query.exec()) {
        return false;
    }
    return true;
}

bool FaceFeatureRepository::insertOrReplaceBatch(const QVector<FaceFeatureRecord> &records,
                                                 int syncGeneration)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO face_feature "
                  "(employee_id, feature_blob, feature_size, updated_at, sync_generation) "
                  "VALUES (:eid, :blob, :size, datetime('now'), :gen)");

    for (const auto &r : records) {
        query.bindValue(":eid", r.employeeId);
        query.bindValue(":blob", r.featureBlob);
        query.bindValue(":size", r.featureSize);
        query.bindValue(":gen", syncGeneration);
        if (!query.exec()) {
            return false;
        }
    }
    return true;
}

QVector<FaceFeatureRecord> FaceFeatureRepository::loadByGeneration(int generation)
{
    QVector<FaceFeatureRecord> result;
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("SELECT employee_id, feature_blob, feature_size "
                  "FROM face_feature WHERE sync_generation = :gen");
    query.bindValue(":gen", generation);

    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        FaceFeatureRecord r;
        r.employeeId  = query.value(0).toString();
        r.featureBlob = query.value(1).toByteArray();
        r.featureSize = query.value(2).toInt();

        if (r.featureSize != r.featureBlob.size()) {
            continue;
        }
        result.append(r);
    }
    return result;
}

bool FaceFeatureRepository::deleteStale(int currentGeneration)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("DELETE FROM face_feature WHERE sync_generation != :gen");
    query.bindValue(":gen", currentGeneration);

    if (!query.exec()) {
        return false;
    }
    return true;
}

int FaceFeatureRepository::countByGeneration(int generation)
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM face_feature WHERE sync_generation = :gen");
    query.bindValue(":gen", generation);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool FaceFeatureRepository::clearAll()
{
    QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
    QSqlQuery query(db);
    if (!query.exec("DELETE FROM face_feature")) {
        return false;
    }
    return true;
}
