#include "CommandHandler.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QTimer>

#include "../LocalStorage/localstorage.h"
#include "../NetworkClient/serverprotocol.h"

CommandHandler::CommandHandler(QObject *parent)
    : QObject(parent)
{
}

void CommandHandler::handleCommand(const QJsonObject &message)
{
    // 提取转发 msgId（服务端分配的，不是管理端原始 ID）
    const QString forwardMsgId = message.value(QStringLiteral("msgId")).toString();
    const QString deviceId     = LocalStorage::instance()->deviceLocal().get().deviceId;

    // 提取 data.command（兼容 op 字段）
    const QJsonObject data = message.value(QStringLiteral("data")).toObject();
    QString command = data.value(QStringLiteral("command")).toString();
    if (command.isEmpty())
        command = data.value(QStringLiteral("op")).toString();

    const QJsonObject params = data.value(QStringLiteral("params")).toObject();

    qDebug() << "CommandHandler: 收到指令" << command
             << "forwardMsgId:" << forwardMsgId;

    if (command.isEmpty()) {
        sendAck(forwardMsgId, QString(), false,
                QStringLiteral("missing command"), ServerProtocol::kCodeParseError);
        return;
    }

    // -----------------------------------------------------------------------
    // reboot — 安全重启应用
    // -----------------------------------------------------------------------
    if (command == QLatin1String("reboot")) {
        qDebug() << "CommandHandler: 执行 reboot";

        // 先回复 ack，再重启
        sendAck(forwardMsgId, command, true, QStringLiteral("success"));

        // 延迟重启，确保 ack 已发出
        QTimer::singleShot(500, []() {
            qDebug() << "CommandHandler: 正在重启...";
            QCoreApplication::quit();
        });
        emit rebootRequested();
        return;
    }

    // -----------------------------------------------------------------------
    // resync — 触发同步
    // -----------------------------------------------------------------------
    if (command == QLatin1String("resync")) {
        qDebug() << "CommandHandler: 执行 resync";

        sendAck(forwardMsgId, command, true, QStringLiteral("success"));
        emit resyncRequested();
        return;
    }

    // -----------------------------------------------------------------------
    // set_time — NTP 校时（可选）
    // -----------------------------------------------------------------------
    if (command == QLatin1String("set_time")) {
        const qint64 serverTime = static_cast<qint64>(
            params.value(QStringLiteral("serverTime")).toDouble());

        qDebug() << "CommandHandler: set_time serverTime=" << serverTime;

        if (serverTime > 0) {
            // 仅记录偏差，不修改系统时间（通常需要管理员权限）
            const qint64 localTime = QDateTime::currentMSecsSinceEpoch();
            const qint64 offset = serverTime - localTime;
            qDebug() << "CommandHandler: 时间偏差" << offset << "ms";

            sendAck(forwardMsgId, command, true,
                    QStringLiteral("offset_ms=%1").arg(offset));
        } else {
            sendAck(forwardMsgId, command, false,
                    QStringLiteral("missing serverTime"));
        }
        return;
    }

    // -----------------------------------------------------------------------
    // 未知指令
    // -----------------------------------------------------------------------
    qWarning() << "CommandHandler: 未知指令" << command;
    sendAck(forwardMsgId, command, false,
            QStringLiteral("unknown command"));
}

void CommandHandler::sendAck(const QString &inReplyTo,
                              const QString &command,
                              bool success,
                              const QString &result,
                              int code,
                              const QString &msg)
{
    const QString deviceId = LocalStorage::instance()->deviceLocal().get().deviceId;

    ServerProtocol::Envelope env = ServerProtocol::buildEnvelope(
        ServerProtocol::kTypeDeviceCommandAck,
        deviceId,
        {},        // data
        inReplyTo,
        /*ack=*/true);

    QJsonObject data;
    data[QStringLiteral("command")] = command;
    data[QStringLiteral("result")]  = result;
    data[QStringLiteral("success")] = success;
    env.data = data;

    if (code != 0)
        env.code = code;
    if (!msg.isEmpty())
        env.msg = msg;

    qDebug() << "CommandHandler: 发送 device.command.ack, command=" << command
             << "success=" << success << "result=" << result;

    emit sendMessage(env.toJson());
}
