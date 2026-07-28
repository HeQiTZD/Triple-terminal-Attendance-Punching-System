#include "FaceFeatureRepository.h"

FaceFeatureRepository::FaceFeatureRepository(const QString &dbPath)
    : BaseRepository(dbPath)
{
}

bool FaceFeatureRepository::insertOrReplace(const QString &employeeId,
                                            const QByteArray &featureBlob,
                                            int featureSize,
                                            int syncGeneration)
{
    return executeNonQuery(
        "INSERT OR REPLACE INTO face_feature "
        "(employee_id, feature_blob, feature_size, updated_at, sync_generation) "
        "VALUES (:eid, :blob, :size, datetime('now'), :gen)",
        {{":eid", employeeId}, {":blob", featureBlob},
         {":size", featureSize}, {":gen", syncGeneration}}
    );
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
    executeReader(
        "SELECT employee_id, feature_blob, feature_size "
        "FROM face_feature WHERE sync_generation = :gen",
        {{":gen", generation}},
        [&](QSqlQuery &q) {
            FaceFeatureRecord r;
            r.employeeId  = q.value(0).toString();
            r.featureBlob = q.value(1).toByteArray();
            r.featureSize = q.value(2).toInt();
            if (r.featureSize == r.featureBlob.size())
                result.append(r);
        });
    return result;
}

bool FaceFeatureRepository::deleteStale(int currentGeneration)
{
    return executeNonQuery(
        "DELETE FROM face_feature WHERE sync_generation != :gen",
        {{":gen", currentGeneration}}
    );
}

int FaceFeatureRepository::countByGeneration(int generation)
{
    return executeScalar(
        "SELECT COUNT(*) FROM face_feature WHERE sync_generation = :gen",
        {{":gen", generation}},
        0
    ).toInt();
}

bool FaceFeatureRepository::clearAll()
{
    return executeNonQuery("DELETE FROM face_feature");
}
