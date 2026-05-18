#include "UserServer.h"
#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"
#include <QJsonArray>
#include <QJsonObject>

using namespace Protocol;

UserServer::UserServer(QObject *parent)
    : QObject(parent)
{
}

void UserServer::setTcpManager(TcpConnectionManager *tcp)
{
    m_tcp = tcp;
}

void UserServer::setBusy(bool v)
{
    if (m_busy == v)
        return;
    m_busy = v;
    emit busyChanged();
}

QVariantList UserServer::parseRecords(const QJsonObject &dataObj)
{
    QVariantList out;
    const QJsonArray arr = dataObj.value(kRecords).toArray();
    for (const QJsonValue &v : arr) {
        const QJsonObject o = v.toObject();
        QVariantMap row;
        row[QStringLiteral("id")] = o.value(QStringLiteral("id")).toInt();
        row[QStringLiteral("employeeId")] = o.value(kEmployeeId).toString();
        row[QStringLiteral("name")] = o.value(kName).toString();
        row[QStringLiteral("createdAt")] = o.value(kCreatedAt).toString();
        row[QStringLiteral("lastLoginTime")] = o.value(QStringLiteral("lastLoginTime")).toString();
        out.append(row);
    }
    return out;
}

void UserServer::createUser(const QString &employeeId,
                            const QString &password,
                            const QString &name)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kUserCreate, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kEmployeeId] = employeeId;
    data[QStringLiteral("password")] = password;
    if (!name.isEmpty())
        data[kName] = name;

    QJsonObject msg;
    msg[kType] = kUserCreate;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kUserCreate, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kUserCreate, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kUserCreate, code, text);
    });
}

void UserServer::queryUsers(const QString &employeeId, const QString &createdAt)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kUserQuery, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kEmployeeId] = employeeId;
    data[kCreatedAt] = createdAt;

    QJsonObject msg;
    msg[kType] = kUserQuery;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kUserQuery, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code != ErrorCode::kSuccess) {
            emit operationFailed(kUserQuery, code, text);
            return;
        }
        const QJsonObject dataObj = resp.value(kData).toObject();
        m_records = parseRecords(dataObj);
        emit recordsChanged();
        emit operationSucceeded(kUserQuery, text.isEmpty() ? QStringLiteral("ok") : text);
    });
}

void UserServer::updateUser(const QString &employeeId,
                            const QString &name,
                            const QString &password)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kUserUpdate, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kEmployeeId] = employeeId;
    if (!name.isEmpty())
        data[kName] = name;
    if (!password.isEmpty())
        data[QStringLiteral("password")] = password;

    QJsonObject msg;
    msg[kType] = kUserUpdate;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kUserUpdate, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kUserUpdate, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kUserUpdate, code, text);
    });
}

void UserServer::deleteUser(const QString &employeeId)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kUserDelete, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kEmployeeId] = employeeId;

    QJsonObject msg;
    msg[kType] = kUserDelete;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject &resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kUserDelete, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kUserDelete, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kUserDelete, code, text);
    });
}
