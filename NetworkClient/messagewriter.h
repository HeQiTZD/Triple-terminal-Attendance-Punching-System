#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H

#include <Qobject>
#include <QTcpSocket>
#include <QJsonObject>

class Messagewriter : public QObject
{
    Q_OBJECT

public:
    //外部传入TCP连接，用于实际发送数据
    explicit Messagewriter(QTcpSocket *socket,QObject *parent = nullptr);

    //发送消息
    bool send(const QJsonObject &message);//发送Json对象，发送结构化业务数据
    bool send(const QByteArray &data);//发送原始信息二进制数据 提供该接口原因：灵活性、性能优化、支持非 JSON 协议

    //批量发送
    int sendBatch(const QVector<QJsonObject> &message);//网络恢复后批量发送队列中的数据

signals:
    //使用信号：异步通知，不阻塞发送流程

    //消息发送成功后触发：通知调用者发送完成
    void messageSent(const QJsonObject &message);
    //消息发送失败后触发：传递错误信息，便于处理
    void sendError(const QString &error);

private:
    QTcpSocket *m_socket;
};

#endif // MESSAGEWRITER_H

/*
┌─────────────────────────────────────┐
│         Messagewriter               │
│           消息写入器                 │
├─────────────────────────────────────┤
│  输入：QJsonObject / QByteArray     │
│  处理：序列化 + 添加分隔符           │
│  输出：通过 QTcpSocket 发送          │
├─────────────────────────────────────┤
│  成功 → 发射 messageSent 信号        │
│  失败 → 发射 sendError 信号          │
└─────────────────────────────────────┘
*/
