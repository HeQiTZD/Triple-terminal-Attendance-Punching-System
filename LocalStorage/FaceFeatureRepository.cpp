#include "FaceFeatureRepository.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

FaceFeatureRepository::FaceFeatureRepository(QSqlDatabase &db)
    : m_db(db)
{
}

bool FaceFeatureRepository::insertOrReplace(const QString &employeeId,
                                            const QByteArray &featureBlob,
                                            int featureSize,
                                            int syncGeneration)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT OR REPLACE INTO face_feature "
                  "(employee_id, feature_blob, feature_size, updated_at, sync_generation) "
                  "VALUES (:eid, :blob, :size, datetime('now'), :gen)");
    query.bindValue(":eid", employeeId);
    query.bindValue(":blob", featureBlob);
    query.bindValue(":size", featureSize);
    query.bindValue(":gen", syncGeneration);

    if (!query.exec()) {
        qWarning() << "FaceFeatureRepository insertOrReplace failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool FaceFeatureRepository::insertOrReplaceBatch(const QVector<FaceFeatureRecord> &records,
                                                 int syncGeneration)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT OR REPLACE INTO face_feature "
                  "(employee_id, feature_blob, feature_size, updated_at, sync_generation) "
                  "VALUES (:eid, :blob, :size, datetime('now'), :gen)");

    for (const auto &r : records) {
        query.bindValue(":eid", r.employeeId);
        query.bindValue(":blob", r.featureBlob);
        query.bindValue(":size", r.featureSize);
        query.bindValue(":gen", syncGeneration);
        if (!query.exec()) {
            qWarning() << "FaceFeatureRepository batch insert failed:" << query.lastError().text();
            return false;
        }
    }
    return true;
}

QVector<FaceFeatureRecord> FaceFeatureRepository::loadByGeneration(int generation)
{
    QVector<FaceFeatureRecord> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT employee_id, feature_blob, feature_size "
                  "FROM face_feature WHERE sync_generation = :gen");
    query.bindValue(":gen", generation);

    if (!query.exec()) {
        qWarning() << "FaceFeatureRepository loadByGeneration failed:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        FaceFeatureRecord r;
        r.employeeId  = query.value(0).toString();
        r.featureBlob = query.value(1).toByteArray();
        r.featureSize = query.value(2).toInt();

        if (r.featureSize != r.featureBlob.size()) {
            qWarning() << "FaceFeatureRepository: feature_size mismatch for" << r.employeeId
                       << "expected" << r.featureSize << "got" << r.featureBlob.size();
            continue;
        }
        result.append(r);
    }
    return result;
}

bool FaceFeatureRepository::deleteStale(int currentGeneration)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM face_feature WHERE sync_generation != :gen");
    query.bindValue(":gen", currentGeneration);

    if (!query.exec()) {
        qWarning() << "FaceFeatureRepository deleteStale failed:" << query.lastError().text();
        return false;
    }
    qDebug() << "FaceFeatureRepository: deleted stale features, affected" << query.numRowsAffected();
    return true;
}

int FaceFeatureRepository::countByGeneration(int generation)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM face_feature WHERE sync_generation = :gen");
    query.bindValue(":gen", generation);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool FaceFeatureRepository::clearAll()
{
    QSqlQuery query(m_db);
    if (!query.exec("DELETE FROM face_feature")) {
        qWarning() << "FaceFeatureRepository clearAll failed:" << query.lastError().text();
        return false;
    }
    return true;
}
