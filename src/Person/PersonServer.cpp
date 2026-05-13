#include "PersonServer.h"
#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"
#include <QJsonObject>
#include <QJsonArray>

using namespace Protocol;

PersonServer::PersonServer(QObject *parent) : QObject(parent) {}

void PersonServer::setTcpManager(TcpConnectionManager* tcp){
    m_tcp = tcp;
}

void PersonServer::setBusy(bool v) {
    if (m_busy == v) {
        return;
    }
    m_busy = v;
    emit busyChanged();
}

QVariantList PersonServer::parseRecords(const QJsonObject &dataObj){
    QVariantList out;
    const QJsonArray arr = dataObj.value(kRecords).toArray();
    for(const QJsonValue &v:arr){
        const QJsonObject o = v.toObject();
        QVariantMap row;
        row[QStringLiteral("id")] = o.value(QStringLiteral("id")).toInt();
        row[QStringLiteral("name")] = o.value(kName).toString();
        row[QStringLiteral("employeeId")] = o.value(kEmployeeId).toString();
        row[QStringLiteral("department")] = o.value(kDepartment).toString();
        row[QStringLiteral("position")] = o.value(kPosition).toString();
        row[QStringLiteral("createdAt")] = o.value(kCreatedAt).toString();
        row[QStringLiteral("updatedAt")] = o.value(kUpdatedAt).toString();
        out.append(row);
    }
    return out;
}

void PersonServer::createPerson(const QString& name, const QString& employeeId, const QString& department, const QString& position)
{
    if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kPersonCreate, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject data;
    data[kName] = name;
    data[kEmployeeId] = employeeId;
    data[kDepartment] = department;
    data[kPosition] = position;

    QJsonObject msg;
    msg[kType] = kPersonCreate;
    msg[kData] = data;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kPersonCreate, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code == ErrorCode::kSuccess)
            emit operationSucceeded(kPersonCreate, text.isEmpty() ? QStringLiteral("ok") : text);
        else
            emit operationFailed(kPersonCreate, code, text);
        });
}

void PersonServer::queryPersons(const QString& name,
    const QString& employeeId,
    const QString& department,
    const QString& position,
    const QString& createdAt,
    const QString& updatedAt)
{
    if (m_tcp || m_tcp->isAuthenticated()) {
        emit operationFailed(kPersonQuery, -1, QStringLiteral("未连接或未认证"));
        return;
    }

    QJsonObject obj;
    obj[kName] = name;
    obj[kEmployeeId] = employeeId;
    obj[kDepartment] = department;
    obj[kPosition] = position;
    obj[kCreatedAt] = createdAt;
    obj[kUpdatedAt] = updatedAt;

    QJsonObject msg;
    msg[kType] = kPersonQuery;
    msg[kData] = obj;

    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kPersonQuery, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();



        });
}