#include "faceData.h"

FaceData::FaceData(QObject *parent)
    : QObject(parent), m_id(0), m_personId(0), m_status(""), m_featureSize(0)
{
}

FaceData::~FaceData()
{
}

int FaceData::id() const
{
    return m_id;
}

void FaceData::setId(int id)
{
    if (m_id != id) {
        m_id = id;
        emit idChanged(id);
    }
}

int FaceData::personId() const
{
    return m_personId;
}

void FaceData::setPersonId(int personId)
{
    if (m_personId != personId) {
        m_personId = personId;
        emit personIdChanged(personId);
    }
}

QByteArray FaceData::featureVector() const
{
    return m_featureVector;
}

void FaceData::setFeatureVector(const QByteArray &featureVector)
{
    if (m_featureVector != featureVector) {
        m_featureVector = featureVector;
        emit featureVectorChanged(featureVector);
    }
}

QString FaceData::status() const
{
    return m_status;
}

void FaceData::setStatus(const QString &status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}

QDateTime FaceData::createdAt() const
{
    return m_createdAt;
}

void FaceData::setCreatedAt(const QDateTime &createdAt)
{
    if (m_createdAt != createdAt) {
        m_createdAt = createdAt;
        emit createdAtChanged(createdAt);
    }
}

QDateTime FaceData::updatedAt() const
{
    return m_updatedAt;
}

void FaceData::setUpdatedAt(const QDateTime &updatedAt)
{
    if (m_updatedAt != updatedAt) {
        m_updatedAt = updatedAt;
        emit updatedAtChanged(updatedAt);
    }
}

int FaceData::featureSize() const
{
    return m_featureSize;
}

void FaceData::setFeatureSize(int featureSize)
{
    if (m_featureSize != featureSize) {
        m_featureSize = featureSize;
        emit featureSizeChanged(featureSize);
    }
}

