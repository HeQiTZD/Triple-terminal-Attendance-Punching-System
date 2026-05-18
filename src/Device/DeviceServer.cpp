#include "DeviceServer.h"
#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

using namespace Protocol;

DeviceServer::DeviceServer(QObject *parent)
    : QObject(parent)
{
}

void DeviceServer::setTcpManager(TcpConnectionManager *tcp)
{
    m_tcp = tcp;
}

void DeviceServer::setBusy(bool v)
{
    if (m_busy == v)
        return;
    m_busy = v;
    emit busyChanged();
}

QVariantList DeviceServer::parseRecords(const QJsonObject &dataObj)
{
    QVariantList out;
    const QJsonArray arr = dataObj.value(kRecords).toArray();
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        QVariantMap row;
        row[QStringLiteral("id")] = o.value(QStringLiteral("id")).toInt();
        row[QStringLiteral("deviceId")] = o.value(kDeviceId).toString();
        row[QStringLiteral("deviceName")] = o.value(kDeviceName).toString();
        row[QStringLiteral("ipAddress")] = o.value(kIpAddress).toString();
        row[QStringLiteral("lastOnline")] = o.value(kLastOnline).toString();
        row[QStringLiteral("status")] = o.value(kStatus).toString();
        out.append(row);
    }
    return out;
}

void DeviceServer::createDevice(const QString &deviceId,
                                const QString &deviceName,
                                const QString &ipAddress,
                                const QString &status)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kDeviceCreate, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kDeviceId] = deviceId;
    data[kDeviceName] = deviceName;
    data[kIpAddress] = ipAddress;
    data[kStatus] = status.isEmpty() ? QStringLiteral("offline") : status;

    QJsonObject msg;
    msg[kType] = kDeviceCreate;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kDeviceCreate, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kDeviceCreate, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kDeviceCreate, code, text);
    });
}

void DeviceServer::queryDevices(const QString &deviceId,
                                const QString &deviceName,
                                const QString &ipAddress)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kDeviceQuery, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kDeviceId] = deviceId;
    data[kDeviceName] = deviceName;
    data[kIpAddress] = ipAddress;

    QJsonObject msg;
    msg[kType] = kDeviceQuery;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kDeviceQuery, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code != ErrorCode::kSuccess) {
            emit operationFailed(kDeviceQuery, code, text);
            return;
        }
        m_records = parseRecords(resp.value(kData).toObject());
        emit recordsChanged();
        emit operationSucceeded(kDeviceQuery, text.isEmpty() ? QStringLiteral("ok") : text);
    });
}

void DeviceServer::updateDevice(const QString &deviceId,
                                const QString &deviceName,
                                const QString &ipAddress,
                                const QString &status)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kDeviceUpdate, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kDeviceId] = deviceId;
    if (!deviceName.isEmpty())
        data[kDeviceName] = deviceName;
    if (!ipAddress.isEmpty())
        data[kIpAddress] = ipAddress;
    if (!status.isEmpty())
        data[kStatus] = status;

    if (data.size() <= 1) {
        emit operationFailed(kDeviceUpdate, -1, QStringLiteral("至少填写一项要修改的内容"));
        return;
    }

    QJsonObject msg;
    msg[kType] = kDeviceUpdate;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kDeviceUpdate, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kDeviceUpdate, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kDeviceUpdate, code, text);
    });
}

void DeviceServer::deleteDevice(const QString &deviceId)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kDeviceDelete, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kDeviceId] = deviceId;

    QJsonObject msg;
    msg[kType] = kDeviceDelete;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kDeviceDelete, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kDeviceDelete, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kDeviceDelete, code, text);
    });
}

void DeviceServer::sendCommand(const QString &deviceId,
                               const QString &command,
                               const QString &paramsJson)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kDeviceCommand, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject params;
    if (!paramsJson.trimmed().isEmpty()) {
        const QJsonDocument doc = QJsonDocument::fromJson(paramsJson.toUtf8());
        if (!doc.isObject()) {
            emit operationFailed(kDeviceCommand, -1, QStringLiteral("params 须为 JSON 对象"));
            return;
        }
        params = doc.object();
    }

    QJsonObject data;
    data[kCommand] = command;
    data[kParams] = params;

    QJsonObject msg;
    msg[kType] = kDeviceCommand;
    msg[kTo] = deviceId;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kDeviceCommand, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kDeviceCommand, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kDeviceCommand, code, text);
    });
}
