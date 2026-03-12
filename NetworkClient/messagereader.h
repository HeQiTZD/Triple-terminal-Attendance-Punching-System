#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

#include <QObject>
#include <QJsonObject>
#include <QTcpSocket>

class Messagereader : public QObject
{
    Q_OBJECT

public:
    explicit Messagereader(QTcpSocket *socket,QObject *parent = nullptr);

    //开始接收
    void start();
    //停止接收
    void stop();

signals:
    //接收消息信号
    void messageReceived(const QJsonObject &message);//成功解析JSON消息后发送
    //发生错误信号
    void parseError(const QString error);//解析JSON失败时

private slots:
    void onReadyRead();//socket有数据可读时，作用读取数据到缓冲区，尝试解析

private:
    bool tryParseMessage(QJsonObject *outMessage);//尝试解析

private:
    QTcpSocket *m_socket;//套接字对象
    QByteArray m_buffer;//缓存接收到的字节数据，处理粘包
};

#endif // MESSAGEREADER_H

/*设计思路
┌─────────────────────────────────────────┐
│           Messagereader                 │
│            消息读取器                    │
├─────────────────────────────────────────┤
│                                         │
│  1. socket 有数据 → onReadyRead()       │
│                                         │
│  2. 读取数据到 m_buffer                 │
│                                         │
│  3. tryParseMessage() 按 \n 分割        │
│                                         │
│  4. JSON 解析成功 → emit messageReceived │
│                                         │
│  5. JSON 解析失败 → emit parseError      │
│                                         │
└─────────────────────────────────────────┘
        ↑                          ↓
   网络数据流入               解析后的消息
   (QTcpSocket)              (QJsonObject)
*/
