#pragma once

#include <QByteArray>
#include <QSqlDatabase>
#include <QString>
#include <QVector>

struct FaceFeatureRecord {
    QString    employeeId;
    QByteArray featureBlob;
    int        featureSize = 0;
};

class FaceFeatureRepository {
public:
    explicit FaceFeatureRepository(QSqlDatabase &db);

    bool insertOrReplace(const QString &employeeId,
                         const QByteArray &featureBlob,
                         int featureSize,
                         int syncGeneration);

    bool insertOrReplaceBatch(const QVector<FaceFeatureRecord> &records,
                              int syncGeneration);

    QVector<FaceFeatureRecord> loadByGeneration(int generation);

    bool deleteStale(int currentGeneration);

    int countByGeneration(int generation);

    bool clearAll();

private:
    QSqlDatabase &m_db;
};
