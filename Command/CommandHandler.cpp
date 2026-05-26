#include "CommandHandler.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QTimer>

#include "../Attendance/AttendanceConfigSyncHandler.h"
#include "../Config/configmanager.h"
#include "../LocalStorage/localstorage.h"
#include "../NetworkClient/serverprotocol.h"

CommandHandler::CommandHandler(QObject *parent)
    : QObject(parent)
{
}

void CommandHandler::handleCommand(const QJsonObject &message)
{
    const QString forwardMsgId = message.value(QStringLiteral("msgId")).toString();
    const QString deviceId     = LocalStorage::instance()->deviceLocal().get().deviceId;

    const QJsonObject data = message.value(QStringLiteral("data")).toObject();
    QString command = data.value(QStringLiteral("command")).toString();
    if (command.isEmpty())
        command = data.value(QStringLiteral("op")).toString();

    const QJsonObject params = data.value(QStringLiteral("params")).toObject();

    if (command.isEmpty()) {
        sendAck(forwardMsgId, QString(), false,
                QStringLiteral("missing command"), ServerProtocol::kCodeParseError);
        return;
    }

    if (command == QLatin1String("reboot")) {
        sendAck(forwardMsgId, command, true, QStringLiteral("success"));

        QTimer::singleShot(500, []() {
            QCoreApplication::quit();
        });
        emit rebootRequested();
        return;
    }

    if (command == QLatin1String("resync")) {
        sendAck(forwardMsgId, command, true, QStringLiteral("success"));
        emit resyncRequested();
        return;
    }

    if (command == QLatin1String("update_attendance_config")
        || command == QLatin1String("reload_rules")) {
        QJsonObject ruleConfig = params;
        if (params.contains(QStringLiteral("config")) || params.contains(QStringLiteral("attendance"))) {
            ruleConfig = params;
        } else if (data.contains(QStringLiteral("config")) || data.contains(QStringLiteral("attendance"))) {
            ruleConfig = data;
        }

        const bool applied = AttendanceConfigSyncHandler::instance()->applyConfig(ruleConfig);
        sendAck(forwardMsgId, command, applied,
                applied ? QStringLiteral("success") : QStringLiteral("attendance config apply failed"));
        return;
    }

    if (command == QLatin1String("update_config")
        || command == QLatin1String("reload_config")) {
        const QString configContent = params.value(QStringLiteral("config")).toString();
        const QString configVersion = params.value(QStringLiteral("configVersion")).toString();
        const QString configHash = params.value(QStringLiteral("configHash")).toString();

        if (configContent.isEmpty()) {
            sendAck(forwardMsgId, command, false,
                    QStringLiteral("missing config content"));
            return;
        }

        QString errorMessage;
        const bool applied = ConfigManager::instance()->applyRemoteConfig(
            configContent, configVersion, configHash, &errorMessage);

        sendAck(forwardMsgId, command, applied,
                applied ? QStringLiteral("success") : errorMessage);

        if (applied) {
            emit configApplied(configVersion);
        }
        return;
    }

    if (command == QLatin1String("set_time")) {
        const qint64 serverTime = static_cast<qint64>(
            params.value(QStringLiteral("serverTime")).toDouble());

        if (serverTime > 0) {
            const qint64 localTime = QDateTime::currentMSecsSinceEpoch();
            const qint64 offset = serverTime - localTime;

            sendAck(forwardMsgId, command, true,
                    QStringLiteral("offset_ms=%1").arg(offset));
        } else {
            sendAck(forwardMsgId, command, false,
                    QStringLiteral("missing serverTime"));
        }
        return;
    }

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
        {},
        inReplyTo,
        true);

    QJsonObject data;
    data[QStringLiteral("command")] = command;
    data[QStringLiteral("result")]  = result;
    data[QStringLiteral("success")] = success;
    env.data = data;

    if (code != 0)
        env.code = code;
    if (!msg.isEmpty())
        env.msg = msg;

    emit sendMessage(env.toJson());
}
