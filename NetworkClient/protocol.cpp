#include "protocol.h"

//Protocol::Protocol() {}

QJsonObject Protocol::AttendanceRecord::toJson() const
{
    QJsonObject obj;
    obj["employeeId"] = employeeId;
    obj["checktTime"] = checktTime;
    obj["status"] = status;
    return obj;
}

Protocol::AttendanceRecord Protocol::AttendanceRecord::fromJson(const QJsonObject &obj)
{
    AttendanceRecord record;
    record.employeeId = obj["employeeId"].toString();
    record.checktTime = obj["checktTme"].toString();
    record.status = obj["status"].toString();
    return record;
}

QJsonObject Protocol::PersonData::toJson() const
{
    QJsonObject obj;
    obj["employeeID"] = employeeId;
    obj["name"] = name;
    obj["faceFeature"] = QString(faceFeature.toBase64());
    obj["featureSize"] = featureSize;
    return obj;
}

Protocol::PersonData Protocol::PersonData::fromJson(const QJsonObject &obj)
{
    PersonData person;
    person.employeeId = obj["employeeID"].toString();
    person.faceFeature = QByteArray::fromBase64(obj["faceFeature"].toString().toUtf8());
    person.featureSize = obj["featureSize"].toInt();
    return person;
}

QJsonObject Protocol::createMessage(MessageType type, const QJsonObject &data)
{
    QJsonObject message;
    message["type"] = messageTypeToString(type);
    message["data"] = data;
    return message;
}

QString Protocol::messageTypeToString(MessageType type)
{
    switch(type){
    case HEARTBEAT: return "HEARTBEAT";
    case SYNC_PERSON_REQUEST: return "SYNC_PERSON_REQUEST";
    case SYNC_PERSON_RESPONSE: return "SYNC_PERSON_RESPONSE";
    case UPLOAD_ATTENDANCE: return "UPLOAD_ATTENDANCE";
    case UPLOAD_RESPONSE: return "UPLOAD_RESPONSE";
    case DEVICE_STATUS: return "DEVICE_STATUS";
    default: return "UNKNOWN";
    }
}

//解析消息类型
Protocol::MessageType Protocol::parseMessageType(const QJsonObject &message)
{
    static const QMap<QString,MessageType> typeMap = {
        {"HEARTBEAT",HEARTBEAT},
        {"SYNC_PERSON_REQUEST",SYNC_PERSON_REQUEST},
        {"SYNC_PERSON_RESPONSE",SYNC_PERSON_RESPONSE},
        {"UPLOAD_ATTENDANCE",UPLOAD_ATTENDANCE},
        {"UPLOAD_RESPONSE",UPLOAD_RESPONSE},
        {"DEVICE_STATUS",DEVICE_STATUS},
    };

    QString typeStr = message["type"].toString();
    return typeMap.value(typeStr,MessageType(999));//999 无效类型标识符
}
