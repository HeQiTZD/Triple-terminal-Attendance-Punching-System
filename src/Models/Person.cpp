#include "Person.h"

Person::Person(QObject *parent) : QObject(parent),m_id(0)
{
}

int Person::id() const
{
    return m_id;
}

void Person::setId(int id)
{
    if(m_id != id){
        m_id = id;
        emit idChanged();
    }
}

QString Person::name() const
{
    return m_name;
}

void Person::setName(const QString &name)
{
    if(m_name != name){
        m_name = name;
        emit nameChanged();
    }
}

QString Person::employeeId() const
{
    return m_employeeId;
}

void Person::setEmployeeId(const QString &employeeId)
{
    if(m_employeeId != employeeId){
        m_employeeId = employeeId;
        emit employeeIdChanged();
    }
}

QString Person::department() const
{
    return m_department;
}

void Person::setDepartment(const QString &department)
{
    if(m_department != department){
        m_department = department;
        emit departmentChanged();
    }
}

QString Person::position() const
{
    return m_position;
}

void Person::setPosition(const QString &position)
{
    if(m_position != position){
        m_position = position;
        emit positionChanged();
    }
}

QByteArray Person::faceFeature() const
{
    return m_faceFeature;
}

void Person::setFaceFeature(const QByteArray &faceFeature)
{
    if(faceFeature != faceFeature){
        m_faceFeature = faceFeature;
    }
}

QDateTime Person::createdAt() const
{
    return m_createdAt;
}

void Person::setCreatedAt(const QDateTime &createdAt)
{
    if(m_createdAt != createdAt){
        m_createdAt = createdAt;
        emit createdAtChanged();
    }
}

QDateTime Person::updatedAt() const
{
    return m_updatedAt;
}

void Person::setUpdatedAt(const QDateTime &updatedAt)
{
    if(m_updatedAt != updatedAt){
        m_updatedAt = updatedAt;
        emit updatedAtChanged();
    }
}

QJsonObject Person::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["employeeId"] = m_employeeId;
    json["department"] = m_department;
    json["position"] = m_position;
    json["createdAt"] = m_createdAt.toString(Qt::ISODate);
    json["updatedAt"] = m_updatedAt.toString(Qt::ISODate);
    return json;
}

Person *Person::fromJson(const QJsonObject &json, QObject *parent)
{
    Person *person = new Person(parent);
    person->setId(json["id"].toInt());
    person->setName(json["name"].toString());
    person->setEmployeeId(json["employeeId"].toString());
    person->setDepartment(json["department"].toString());
    person->setPosition(json["position"].toString());
    person->setCreatedAt(QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate));
    person->setUpdatedAt(QDateTime::fromString(json["updatedAt"].toString(), Qt::ISODate));
    return person;
}
