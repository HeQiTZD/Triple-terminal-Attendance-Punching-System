#include "messagewriter.h"

Messagewriter::Messagewriter(QTcpSocket *socket,QObject *parent)
    : QObject(parent),
    m_socket(socket)
{
    //参数校验
    if(!m_socket){
        qWarning()<<"socket为空";
    }
}

bool Messagewriter::send(const QJsonObject &message)
{
    //检查socket是否为空
    if(!m_socket){
        qWarning()<<"socket未初始化";
        emit sendError("socket未初始化");
        return false;
    }

    //检查连接状态
    if(m_socket->state() != QAbstractSocket::ConnectedState){
        emit sendError("socket未连接");
        return false;
    }

    /*
        QJsonObject 只是 JSON 对象（键值对）
        QJsonDocument 是完整的 JSON 文档，支持序列化/反序列化
        网络传输需要字节流，必须通过 QJsonDocument 转换

        toJson() 将 JSON 文档转换为 QByteArray （字节数组）
        QJsonDocument::Compact 紧凑格式，无多余空格和换行（占用空间小）

        选择 Compact 的原因 ：
        1. 减少网络流量 ：去掉多余空格和换行
        2. 提高传输效率 ：数据包更小，发送更快
        3. 便于解析 ：接收方按换行符分割消息
    */
    //Json序列化
    QJsonDocument doc(message);//包装成文档
    QByteArray data = doc.toJson(QJsonDocument::Compact);//序列化为字节

    //添加消息分隔符（换行符）
    data.append("\n");

    //发送数据
    qint64 bytesWritten = m_socket->write(data);//写入缓冲区

    //转化后数据：{"type":"heartbeat","time":"2024-01-01"}

    /*
        错误原因的原理和存储位置
        存储位置 QTcpSocket 基类 QAbstractSocket 的成员变量 m_errorString
        何时设置 发生错误时，Qt 内部自动设置
        如何获取 调用 errorString() 方法 或通过error()获取错误码
        语言 跟随系统语言（中文系统返回中文）
        有效期 直到下一次错误发生前都有效
    */
    if(bytesWritten == -1){
        emit sendError("写入失败："+m_socket->errorString());
        return false;
    }

    //确保数据立即发送
    //flush() 的作用：强制将缓冲区中的数据立即发送到网络 ，不等待系统默认的缓冲机制。
    if(!m_socket->flush()){
        qWarning()<<"发送数据失败";
        return false;
    }

    //回调通知 ，不是网络发送。它让调用者知道"哪条消息"在"什么时候"成功发送了。
    emit messageSent(message);
    return true;
}

bool Messagewriter::send(const QByteArray &data)
{
    //检查socket是否为空
    if(!m_socket){
        qWarning()<<"socket未初始化";
        emit sendError("socket未初始化");
        return false;
    }

    //检查连接状态
    if(m_socket->state() != QAbstractSocket::ConnectedState){
        emit sendError("socket未连接");
        return false;
    }

    /*直接发送原始数据  "原始数据"指的是 已经序列化好的字节流 ，不是未经处理的原始数据。
        TCP 是流式协议，没有消息边界，解决方案：使用换行符作为消息分隔符
        发送方：{"type":"a"}\n{"type":"b"}\n
        接收方按 \n 分割：{"type":"a"} | {"type":"b"}
    */
    QByteArray dataWithNewline = data;//复制数据
    if(!dataWithNewline.endsWith('\n')){//检查是否已有换行
        dataWithNewline.append('\n');//没有则添加
    }
    
    qint64 bytesWritten = m_socket->write(dataWithNewline);

    if(bytesWritten == -1){
        emit sendError("写入失败："+m_socket->errorString());
        return false;
    }

    m_socket->flush();
    return true;
}

//批量发送
int Messagewriter::sendBatch(const QVector<QJsonObject> &message)
{
    int successCount = 0;
    for(const QJsonObject &msg : message){
        if(send(msg)){
            successCount++;
        }else{
            //发送失败，停止发送
            qWarning()<<"批量发送中断，成功"<<successCount<<"条，失败原因:"<<msg;
            break;
        }
    }
    return successCount;
}