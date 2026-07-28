#pragma once

#include "BaseRepository.h"
#include <QByteArray>
#include <QString>
#include <QVector>

struct FaceFeatureRecord {
    QString    employeeId;
    QByteArray featureBlob;
    int        featureSize = 0;
};

class FaceFeatureRepository : public BaseRepository {
public:
    explicit FaceFeatureRepository(const QString &dbPath);

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
};
