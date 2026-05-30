#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class MessageWriter : public QObject
{
    Q_OBJECT

public:
    explicit MessageWriter(QTcpSocket *socket, QObject *parent = nullptr);

    // ---- JSON 消息（自动追加 \n 分隔符） ----
    bool send(const QJsonObject &message);
    int  sendBatch(const QVector<QJsonObject> &messages);

    // ---- 原始字节（自动追加 \n 分隔符，用于心跳等已序列化消息） ----
    bool send(const QByteArray &data);

    // ---- 二进制帧：4 字节 BE 长度前缀 + payload（4+L 格式） ----
    bool sendBinaryFrame(const QByteArray &payload);

    // ---- 原始字节，无前缀、无分隔符（用于照片上传） ----
    bool sendRawBytes(const QByteArray &data);

signals:
    void messageSent(const QJsonObject &message);
    void sendError(const QString &error);

private:
    bool sendRawJson(const QJsonObject &msg); // JSON 序列化，不追加 \n

    QTcpSocket *m_socket;
};

#endif // MESSAGEWRITER_H
