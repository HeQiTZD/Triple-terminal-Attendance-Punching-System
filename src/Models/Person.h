#ifndef PERSON_H
#define PERSON_H

#include <QJsonObject>
#include <QObject>
#include <QDateTime>
#endif // PERSON_H
class Person : public QObject{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString employeeId READ employeeId WRITE setEmployeeId NOTIFY employeeIdChanged)
    Q_PROPERTY(QString department READ department WRITE setDepartment NOTIFY departmentChanged)
    Q_PROPERTY(QString position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt NOTIFY createdAtChanged)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt NOTIFY updatedAtChanged)

public:
    explicit Person(QObject *parent = nullptr);

    int id() const;
    void setId(int id);

    QString name() const;
    void setName(const QString &name);

    QString employeeId() const;
    void setEmployeeId(const QString &employeeId);

    QString department() const;
    void setDepartment(const QString &department);

    QString position() const;
    void setPosition(const QString &position);

    QByteArray faceFeature() const;
    void setFaceFeature(const QByteArray &faceFeature);

    QDateTime createdAt() const;
    void setCreatedAt(const QDateTime &createdAt);

    QDateTime updatedAt() const;
    void setUpdatedAt(const QDateTime &updateAt);

    QJsonObject toJson() const;
    static Person* fromJson(const QJsonObject &json,QObject *parent = nullptr);

signals:
    void idChanged();
    void nameChanged();
    void employeeIdChanged();
    void departmentChanged();
    void positionChanged();
    void createdAtChanged();
    void updatedAtChanged();

private:
    int m_id;
    QString m_name;
    QString m_employeeId;
    QString m_department;
    QString m_position;
    QByteArray m_faceFeature;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};
