#ifndef PROTOCOL_H
#define PROTOCOL_H

// 职责 : 定义通信协议数据结构
// 开发思路 :
// - 定义消息类型枚举
// - 定义各种消息结构体
// - 提供序列化/反序列化方法

#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector>

namespace Protocol
{
    //消息类型
    //设计目的 ：定义系统中所有可能的通信消息类型，让收发双方知道如何处理消息。
    enum MessageType{
        HEARTBEAT,//心跳包 - 保持连接活跃
        SYNC_PERSON_REQUEST,//请求同步人员数据
        SYNC_PERSON_RESPONSE,//人员数据同步响应
        UPLOAD_ATTENDANCE,//上传打卡记录
        UPLOAD_RESPONSE,//上传相应
        DEVICE_STATUS//设备状态上报
    };

    //打卡记录
    struct AttendanceRecord{
        QString employeeId;
        QString checktTme;
        QString status;

        //网络传输需要文本格式，JSON是通用标准
        QJsonObject toJson() const;//序列化：对象->Json

        //fromJson()，接收到数据后需要转化为C++对象使用
        static AttendanceRecord fromJson(const QJsonObject &obj);//反序列化：Json -> 对象
    };

    struct PersonData{
        QString employeeId;
        QString name;
        QByteArray faceFeature;//二进制数据（faceFeature）必须转为文本才能在JSON中传输
        int featureSize;

        QJsonObject toJson() const;
        static PersonData fromJson(const QJsonObject &obj);
    };

    //消息打包/解包
    //创建标准格式的消息
    QJsonObject createMessage(MessageType type,const QJsonObject &data);
    //从消息中解析出类型
    MessageType parseMessageType(const QJsonObject &message);
    //类型转字符串（用于日志和调试）
    QString messageTypeToString(MessageType type);
};

#endif // PROTOCOL_H
