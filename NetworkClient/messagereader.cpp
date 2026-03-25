#include "messagereader.h"

Messagereader::Messagereader(QTcpSocket *socket,QObject *parent):
    QObject(parent),
    m_socket(socket)
{
    //参数校验
    if(!m_socket){
        qWarning()<<"Messagereader: socket为空";
    }
}

void Messagereader::start()
{
    if(!m_socket){
        qWarning()<<"消息接收无法功能无法启动,socket为空";
        return;
    }

    /*
        连接socket的readyRead信号
        当socket有数据可读时，自动调用onReadyRead（）
    */
    connect(m_socket,&QTcpSocket::readyRead,this,&Messagereader::onReadyRead);
    qDebug()<<"开始接收数据";
}

void Messagereader::stop()
{
    //断开信号,停止接收
    disconnect(m_socket,&QTcpSocket::readyRead,this,&Messagereader::onReadyRead);
    qDebug()<<"停止接收数据";
}

void Messagereader::onReadyRead()
{
    //检查socket是否有效
    if(!m_socket || m_socket->state() != QAbstractSocket::ConnectedState){
        return;
    }
    
    //读取所有可用数据到缓冲区
    QByteArray newData = m_socket->readAll();
    m_buffer.append(newData);

    //尝试解析缓冲区里的数据
    //循环处理可能存在的多条消息
    while(true){
        QJsonObject message;
        if(tryParseMessage(&message)){
            //解析成功
            emit messageReceived(message);
        }else{
            //没有完整消息，退出循环等待更多数据
            break;
        }
    }

}

bool Messagereader::tryParseMessage(QJsonObject *outMessage)
{
    //查找换行符（消息分割符号）
    //MessageWrite发送时会在每条消息后添加分隔符（\n）
    int newlineindex = m_buffer.indexOf('\n');//indexof() 在一个字符串或容器中查找指定元素或子串第一次出现的位置（索引）

    if(newlineindex == -1){
        //没有找到换行符，消息不完整
        //等待下次接收更多数据
        return false;
    }

    //提取一条完整的消息，不包括换行符
    QByteArray messageData = m_buffer.left(newlineindex);

    //从缓冲区移除已处理的消息，包括换行符
    m_buffer.remove(0,newlineindex+1);

    if(messageData.isEmpty()){
        //跳过空消息，继续尝试解析下一条
        return tryParseMessage(outMessage);
    }

    //JSON解析
    QJsonParseError parseErrors;
    QJsonDocument doc = QJsonDocument::fromJson(messageData,&parseErrors);

    if(parseErrors.error != QJsonParseError::NoError){
        //JSON解析失败
        QString errorStr = QString("JSON解析失败：%1(位置：%2)").arg(parseErrors.errorString()).arg(parseErrors.offset);
        qWarning()<<"Messageready:"<<errorStr;
        qWarning()<<"原始数据:"<<messageData;

        emit parseError(errorStr);
    }

    if(!doc.isObject()){
        //不是JSON对象
        QString errorStr = "解析结果不是JSON对象";
        qWarning()<<"MessageReady:"<<errorStr;
        return false;
    }

    /*
        doc.object() 是 Qt 中 QJsonDocument 类的一个成员函数，
        它的作用是获取文档内部存储的 JSON 对象数据，并将其作为一个
        QJsonObject 类型的值返回。
    */
    //解析成功
    *outMessage = doc.object();//
    return true;
}
