#ifndef FACEDATA_H
#define FACEDATA_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDateTime>

class FaceData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int personId READ personId WRITE setPersonId NOTIFY personIdChanged)
    Q_PROPERTY(QByteArray featureVector READ featureVector WRITE setFeatureVector NOTIFY featureVectorChanged)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt NOTIFY createdAtChanged)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt NOTIFY updatedAtChanged)
    Q_PROPERTY(int featureSize READ featureSize WRITE setFeatureSize NOTIFY featureSizeChanged)

public:
    explicit FaceData(QObject *parent = nullptr);
    ~FaceData();

    int id() const;
    void setId(int id);

    int personId() const;
    void setPersonId(int personId);

    QByteArray featureVector() const;
    void setFeatureVector(const QByteArray &featureVector);

    QString status() const;
    void setStatus(const QString &status);

    QDateTime createdAt() const;
    void setCreatedAt(const QDateTime &createdAt);

    QDateTime updatedAt() const;
    void setUpdatedAt(const QDateTime &updatedAt);

    int featureSize() const;
    void setFeatureSize(int featureSize);

signals:
    void idChanged(int id);
    void personIdChanged(int personId);
    void featureVectorChanged(const QByteArray &featureVector);
    void statusChanged(const QString &status);
    void createdAtChanged(const QDateTime &createdAt);
    void updatedAtChanged(const QDateTime &updatedAt);
    void featureSizeChanged(int featureSize);

private:
    int m_id;
    int m_personId;
    QByteArray m_featureVector;
    QString m_status;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    int m_featureSize;
};

#endif // FACEDATA_H