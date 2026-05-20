#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>

/// 远程指令处理器 — 接收 device.command，执行，回复 device.command.ack
class CommandHandler : public QObject
{
    Q_OBJECT

public:
    explicit CommandHandler(QObject *parent = nullptr);

public slots:
    /// 接收来自服务端的 device.command 消息
    void handleCommand(const QJsonObject &message);

signals:
    /// 需要发送 JSON 消息（连接到 Networkclient）
    void sendMessage(const QJsonObject &message);

    /// 触发同步（resync 指令）
    void resyncRequested();

    /// 触发安全重启（reboot 指令）
    void rebootRequested();

private:
    void sendAck(const QString &inReplyTo,
                 const QString &command,
                 bool success,
                 const QString &result = {},
                 int code = 0,
                 const QString &msg = {});
};
