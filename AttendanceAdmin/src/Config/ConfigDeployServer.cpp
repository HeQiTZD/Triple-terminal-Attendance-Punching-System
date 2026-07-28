#include "ConfigDeployServer.h"

#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QSet>
#include <QVariantMap>

using namespace Protocol;

ConfigDeployServer::ConfigDeployServer(QObject *parent)
    : QObject(parent)
{
}

void ConfigDeployServer::setTcpManager(TcpConnectionManager *tcp)
{
    m_tcp = tcp;
}

void ConfigDeployServer::setBusy(bool value)
{
    if (m_busy == value)
        return;
    m_busy = value;
    emit busyChanged();
}

void ConfigDeployServer::clearDeployResult()
{
    m_deployTargets.clear();
    m_lastDeployId.clear();
    m_configVersion.clear();
    m_configHash.clear();
    emit deployResultChanged();
}

QVariantList ConfigDeployServer::parseTargets(const QJsonObject &dataObj)
{
    QVariantList out;
    const QJsonArray arr = dataObj.value(kTargets).toArray();
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        QVariantMap row;
        row[QStringLiteral("deviceId")] = o.value(kDeviceId).toString();
        row[QStringLiteral("status")] = o.value(kStatus).toString();
        row[QStringLiteral("forwardMsgId")] = o.value(kForwardMsgId).toString();
        out.append(row);
    }
    return out;
}

void ConfigDeployServer::deployConfig(const QVariantList &targetDevices,
                                      const QString &configContent,
                                      const QString &configVersion,
                                      const QString &description)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kConfigDeploy, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonArray devices;
    QSet<QString> seen;
    for (const QVariant &value : targetDevices) {
        const QString deviceId = value.toString().trimmed();
        if (deviceId.isEmpty() || seen.contains(deviceId))
            continue;
        seen.insert(deviceId);
        devices.append(deviceId);
    }

    if (devices.isEmpty()) {
        emit operationFailed(kConfigDeploy, -1, QStringLiteral("至少选择一台目标设备"));
        return;
    }
    if (configContent.trimmed().isEmpty()) {
        emit operationFailed(kConfigDeploy, -1, QStringLiteral("配置内容不能为空"));
        return;
    }

    QJsonObject data;
    data[kTargetDevices] = devices;
    data[kConfigContent] = configContent;
    if (!configVersion.trimmed().isEmpty())
        data[kConfigVersion] = configVersion.trimmed();
    if (!description.trimmed().isEmpty())
        data[kDescription] = description.trimmed();

    QJsonObject msg;
    msg[kType] = kConfigDeploy;
    msg[kData] = data;

    setBusy(true);
    clearDeployResult();
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kConfigDeploy, -1, QStringLiteral("请求超时"));
            return;
        }

        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code != ErrorCode::kSuccess) {
            emit operationFailed(kConfigDeploy, code, text);
            return;
        }

        const QJsonObject dataObj = resp.value(kData).toObject();
        const QJsonValue deployId = dataObj.value(kDeployId);
        if (deployId.isDouble())
            m_lastDeployId = QString::number(static_cast<qint64>(deployId.toDouble()));
        else
            m_lastDeployId = deployId.toString();
        m_configVersion = dataObj.value(kConfigVersion).toString();
        m_configHash = dataObj.value(kConfigHash).toString();
        m_deployTargets = parseTargets(dataObj);
        emit deployResultChanged();
        emit operationSucceeded(kConfigDeploy,
                                text.isEmpty() ? QStringLiteral("配置下发请求已提交") : text);
    });
}
