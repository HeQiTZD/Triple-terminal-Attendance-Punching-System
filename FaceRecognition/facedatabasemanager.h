#pragma once

#include <QObject>
#include <QMutex>
#include <QVector>
#include "arcfaceengine.h"

class FaceDatabaseManager : public QObject
{
    Q_OBJECT

public:
    struct FaceRecord {
        QString    employeeId;
        QByteArray featureData;
        int        featureSize = 0;
    };

    static FaceDatabaseManager* instance();

    bool loadFromDatabase();

    void reload();

    void clear();

    QPair<QString, float> findBestMatch(const arcfaceengine::FaceFeature &targetFeature);

    int getPersonCount() const;

private:
    explicit FaceDatabaseManager(QObject *parent = nullptr);

    QVector<FaceRecord> m_faceRecords;
    mutable QMutex m_mutex;
};
