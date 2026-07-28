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
    Q_PROPERTY(QString employeeId READ employeeId WRITE setEmployeeId NOTIFY employeeIdChanged)
    Q_PROPERTY(QString personName READ personName WRITE setPersonName NOTIFY personNameChanged)
    Q_PROPERTY(QByteArray featureVector READ featureVector WRITE setFeatureVector NOTIFY featureVectorChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt NOTIFY createdAtChanged)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt NOTIFY updatedAtChanged)
    Q_PROPERTY(int featureSize READ featureSize WRITE setFeatureSize NOTIFY featureSizeChanged)

public:
    explicit FaceData(QObject *parent = nullptr);
    ~FaceData();

    int id() const;
    void setId(int id);

    QString employeeId() const;
    void setEmployeeId(const QString &employeeId);

    QString personName() const;
    void setPersonName(const QString &personName);

    QByteArray featureVector() const;
    void setFeatureVector(const QByteArray &featureVector);

    QDateTime createdAt() const;
    void setCreatedAt(const QDateTime &createdAt);

    QDateTime updatedAt() const;
    void setUpdatedAt(const QDateTime &updatedAt);

    int featureSize() const;
    void setFeatureSize(int featureSize);

signals:
    void idChanged(int id);
    void employeeIdChanged(const QString &employeeId);
    void personNameChanged(const QString &personName);
    void featureVectorChanged(const QByteArray &featureVector);
    void createdAtChanged(const QDateTime &createdAt);
    void updatedAtChanged(const QDateTime &updatedAt);
    void featureSizeChanged(int featureSize);

private:
    int m_id;
    QString m_employeeId;
    QString m_personName;
    QByteArray m_featureVector;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    int m_featureSize;
};

#endif // FACEDATA_H
