#include "messagereader.h"

#include <QJsonDocument>
#include <QJsonParseError>

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

Messagereader::Messagereader(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
{
    if (!m_socket)
        qWarning() << "Messagereader: socket为空";
}

// ---------------------------------------------------------------------------
// start / stop
// ---------------------------------------------------------------------------

void Messagereader::start()
{
    if (!m_socket) {
        qWarning() << "消息接收功能无法启动, socket为空";
        return;
    }

    connect(m_socket, &QTcpSocket::readyRead, this, &Messagereader::onReadyRead);
    qDebug() << "Messagereader: 开始接收数据";
}

void Messagereader::stop()
{
    disconnect(m_socket, &QTcpSocket::readyRead, this, &Messagereader::onReadyRead);

    // 重置状态，确保下次 start 从干净状态开始
    m_buffer.clear();
    m_mode = Mode::Line;
    m_pendingBinaryHeader = QJsonObject();

    qDebug() << "Messagereader: 停止接收数据";
}

// ---------------------------------------------------------------------------
// onReadyRead — state machine
// ---------------------------------------------------------------------------

void Messagereader::onReadyRead()
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray newData = m_socket->readAll();
    m_buffer.append(newData);

    // ---------- 缓冲区上限 ----------
    if (m_buffer.size() > kMaxBufferSize) {
        qWarning() << "Messagereader: 缓冲区溢出" << m_buffer.size();
        emit parseError(QStringLiteral("接收缓冲区溢出"));
        m_buffer.clear();
        m_mode = Mode::Line;
        return;
    }

    // ---------- 主循环 ----------
    while (true) {
        if (m_mode == Mode::Line) {
            QJsonObject message;
            if (!tryParseLine(&message))
                break; // 需要更多数据

            // face.sync.item.header 触发二进制模式切换
            const QString type = message.value(QStringLiteral("type")).toString();
            if (type == QLatin1StringView("face.sync.item.header")) {
                m_pendingBinaryHeader = message;
                m_mode = Mode::Binary;
                // 不发射 messageReceived — 等二进制帧收完后统一发射
            } else {
                emit messageReceived(message);
            }
        } else {
            if (!tryParseBinaryFrame())
                break; // 需要更多数据
            // tryParseBinaryFrame 已发射 binaryFrameReceived 并切回 LineMode
        }
    }
}

// ---------------------------------------------------------------------------
// LineMode — 按 \n 分隔解析 JSON
// ---------------------------------------------------------------------------

bool Messagereader::tryParseLine(QJsonObject *outMessage)
{
    const int nl = m_buffer.indexOf('\n');
    if (nl == -1)
        return false; // 消息不完整

    QByteArray line = m_buffer.left(nl);
    m_buffer.remove(0, nl + 1);

    if (line.isEmpty())
        return tryParseLine(outMessage); // 跳过空行

    QJsonParseError parseErr;
    QJsonDocument doc = QJsonDocument::fromJson(line, &parseErr);

    if (parseErr.error != QJsonParseError::NoError) {
        const QString errStr = QStringLiteral("JSON解析失败：%1 (位置 %2)")
                                   .arg(parseErr.errorString())
                                   .arg(parseErr.offset);
        qWarning() << "Messagereader:" << errStr;
        qWarning() << "原始数据:" << line;
        emit parseError(errStr);
        return false;
    }

    if (!doc.isObject()) {
        qWarning() << "Messagereader: 解析结果不是JSON对象";
        return false;
    }

    *outMessage = doc.object();
    return true;
}

// ---------------------------------------------------------------------------
// BinaryMode — 4 字节 BE 长度前缀 + payload
// ---------------------------------------------------------------------------

bool Messagereader::tryParseBinaryFrame()
{
    // 需要至少 4 字节长度前缀
    if (m_buffer.size() < kLengthPrefixLen)
        return false;

    // 读取 4 字节大端无符号整数
    const auto *raw = reinterpret_cast<const quint8 *>(m_buffer.constData());
    const quint32 payloadLen = (static_cast<quint32>(raw[0]) << 24)
                             | (static_cast<quint32>(raw[1]) << 16)
                             | (static_cast<quint32>(raw[2]) << 8)
                             | (static_cast<quint32>(raw[3]));

    // 校验 featureSize（如 header 提供了该字段）
    const int expectedSize = m_pendingBinaryHeader.value(QStringLiteral("featureSize")).toInt();
    if (expectedSize > 0 && static_cast<int>(payloadLen) != expectedSize) {
        qWarning() << "Messagereader: 二进制载荷大小不匹配, 预期"
                   << expectedSize << "实际" << payloadLen;
    }

    // 载荷过大保护
    if (payloadLen > static_cast<quint32>(kMaxBufferSize)) {
        qWarning() << "Messagereader: 二进制载荷过大" << payloadLen;
        emit parseError(QStringLiteral("二进制载荷超出上限"));
        m_buffer.clear();
        m_mode = Mode::Line;
        return false;
    }

    const int totalNeeded = kLengthPrefixLen + static_cast<int>(payloadLen);
    if (m_buffer.size() < totalNeeded)
        return false; // 载荷未收完

    // 提取 payload
    const QByteArray payload = m_buffer.mid(kLengthPrefixLen, static_cast<int>(payloadLen));
    m_buffer.remove(0, totalNeeded);

    emit binaryFrameReceived(m_pendingBinaryHeader, payload);

    // 切回行模式
    m_pendingBinaryHeader = QJsonObject();
    m_mode = Mode::Line;
    return true;
}
