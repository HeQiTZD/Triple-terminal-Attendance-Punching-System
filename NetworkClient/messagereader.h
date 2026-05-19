#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H

#include <QObject>
#include <QJsonObject>
#include <QTcpSocket>

class Messagereader : public QObject
{
    Q_OBJECT

public:
    explicit Messagereader(QTcpSocket *socket, QObject *parent = nullptr);

    void start();
    void stop();

signals:
    /// 成功解析一条 JSON 行消息（LineMode）
    void messageReceived(const QJsonObject &message);

    /// 接收到完整二进制帧：header 为 face.sync.item.header JSON，payload 为特征 BLOB
    void binaryFrameReceived(const QJsonObject &header, const QByteArray &payload);

    /// JSON 解析失败
    void parseError(const QString error);

private slots:
    void onReadyRead();

private:
    enum class Mode {
        Line,   // 默认：按 \n 分隔解析 JSON 行
        Binary  // 等待 4 字节 BE 长度前缀 + payload
    };

    bool tryParseLine(QJsonObject *outMessage);
    bool tryParseBinaryFrame();

    QTcpSocket *m_socket;
    QByteArray m_buffer;
    Mode       m_mode = Mode::Line;
    QJsonObject m_pendingBinaryHeader; // 收到 face.sync.item.header 后暂存

    static constexpr int kMaxBufferSize   = 1048576 + 4096; // 1 MiB + 4 KiB
    static constexpr int kLengthPrefixLen = 4;              // BE uint32
};

#endif // MESSAGEREADER_H
