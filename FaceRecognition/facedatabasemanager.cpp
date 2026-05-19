#include "facedatabasemanager.h"
#include "../LocalStorage/localstorage.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

FaceDatabaseManager::FaceDatabaseManager(QObject *parent)
    : QObject(parent)
{
}

FaceDatabaseManager* FaceDatabaseManager::instance()
{
    static FaceDatabaseManager* s_instance = nullptr;
    if (!s_instance) {
        s_instance = new FaceDatabaseManager();
    }
    return s_instance;
}

bool FaceDatabaseManager::loadFromDatabase()
{
    QMutexLocker locker(&m_mutex);

    m_faceRecords.clear();

    // Read current_generation from sync_meta
    int generation = 1;
    {
        auto &syncMeta = LocalStorage::instance()->syncMeta();
        generation = syncMeta.get().currentGeneration;
        if (generation <= 0) {
            generation = 1;
        }
    }

    // Load from face_feature table by generation
    auto &faceRepo = LocalStorage::instance()->faceFeatures();
    auto features = faceRepo.loadByGeneration(generation);

    for (const auto &f : features) {
        if (f.featureSize != f.featureBlob.size()) {
            qWarning() << "FaceDatabaseManager: feature_size mismatch for" << f.employeeId
                       << "expected" << f.featureSize << "got" << f.featureBlob.size();
            continue;
        }
        FaceRecord record;
        record.employeeId  = f.employeeId;
        record.featureData = f.featureBlob;
        record.featureSize = f.featureSize;
        m_faceRecords.append(record);
    }

    qDebug() << "FaceDatabaseManager: loaded" << m_faceRecords.size()
             << "face records (generation" << generation << ")";
    return true;
}

void FaceDatabaseManager::reload()
{
    loadFromDatabase();
}

void FaceDatabaseManager::clear()
{
    QMutexLocker locker(&m_mutex);
    m_faceRecords.clear();
}

QPair<QString, float> FaceDatabaseManager::findBestMatch(const arcfaceengine::FaceFeature &targetFeature)
{
    QMutexLocker locker(&m_mutex);

    QString bestMatchId;
    float maxSimilarity = 0.0f;

    arcfaceengine* engine = arcfaceengine::instance();

    for (const auto &record : m_faceRecords) {
        arcfaceengine::FaceFeature dbFeature;
        dbFeature.data = record.featureData;
        dbFeature.size = record.featureSize;

        float similarity = engine->compareFeatures(targetFeature, dbFeature);

        if (similarity > maxSimilarity) {
            maxSimilarity = similarity;
            bestMatchId = record.employeeId;
        }
    }

    return qMakePair(bestMatchId, maxSimilarity);
}

int FaceDatabaseManager::getPersonCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_faceRecords.size();
}
