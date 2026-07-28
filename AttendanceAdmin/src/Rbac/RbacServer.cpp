#include "RbacServer.h"
#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"
using namespace Protocol;

RbacServer::RbacServer(QObject* parent) :QObject(parent) {}

void RbacServer::setTcpManager(TcpConnectionManager* tcp) {
	m_tcp = tcp;
}

void RbacServer::setBusy(bool v)
{
	if (m_busy == v) return;
	m_busy = v;
	emit busyChanged();
}

QVariantList RbacServer::parseRoleRecords(const QJsonObject& dataObj) {
	QVariantList out;
	const QJsonArray arr = dataObj.value(kRecords).toArray();
	for (const QJsonValue& v : arr) {
		const QJsonObject o = v.toObject();
		QVariantMap row;
		row[QStringLiteral("id")] = o.value(QStringLiteral("id")).toVariant();
		row[kRoleKey] = o.value(kRoleKey).toString();
		row[kRoleName] = o.value(kRoleName).toString();
		row[kDescription] = o.value(kDescription).toString();
		row[kIsSystem] = o.value(kIsSystem).toBool();
		row[kCreatedAt] = o.value(kCreatedAt).toString();
		row[kUpdatedAt] = o.value(kUpdatedAt).toString();
		if (o.contains(kPermissions)) {
			QVariantList perms;
			const QJsonArray permArr = o.value(kPermissions).toArray();
			for (const QJsonValue &pv : permArr)
				perms.append(pv.toString());
			row[kPermissions] = perms;
		}
		out.append(row);
	}
	return out;
}

QVariantList RbacServer::parsePermissionRecords(const QJsonObject& dataObj)
{
	QVariantList out;
	const QJsonArray arr = dataObj.value(kRecords).toArray();
	for (const QJsonValue& v : arr) {
		const QJsonObject o = v.toObject();
		QVariantMap row;
		row[QStringLiteral("id")] = o.value(QStringLiteral("id")).toVariant();
		row[kPermKey] = o.value(kPermKey).toString();
		row[kPermName] = o.value(kPermName).toString();
		row[kResource] = o.value(kResource).toString();
		row[kDescription] = o.value(kDescription).toString();
		out.append(row);
	}
	return out;
}

void RbacServer::queryRoles()
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kRoleQuery, -1, QStringLiteral("未连接或未认证"));
		return;
	}

	QJsonObject msg;
	msg[kType] = kRoleQuery;
	msg[kData] = QJsonObject();

	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kRoleQuery, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code != ErrorCode::kSuccess) {
			emit operationFailed(kRoleQuery, code, text);
			return;
		}
		const QJsonObject dataObj = resp.value(kData).toObject();
		m_roleRecords = parseRoleRecords(dataObj);
		emit roleRecordsChanged();
		emit operationSucceeded(kRoleQuery, text.isEmpty() ? QStringLiteral("ok") : text);
		});
}

void RbacServer::queryPermissions()
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kPermissionQuery, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject msg;
	msg[kType] = kPermissionQuery;
	msg[kData] = QJsonObject();
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kPermissionQuery, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code != ErrorCode::kSuccess) {
			emit operationFailed(kPermissionQuery, code, text);
			return;
		}
		const QJsonObject dataObj = resp.value(kData).toObject();
		m_permissionRecords = parsePermissionRecords(dataObj);
		emit permissionRecordsChanged();
		emit operationSucceeded(kPermissionQuery, text.isEmpty() ? QStringLiteral("ok") : text);
		});
}

void RbacServer::queryUserRoles(int userId)
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kUserRoleQuery, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject data;
	data[kUserId] = userId;
	QJsonObject msg;
	msg[kType] = kUserRoleQuery;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kUserRoleQuery, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code != ErrorCode::kSuccess) {
			emit operationFailed(kUserRoleQuery, code, text);
			return;
		}
		const QJsonObject dataObj = resp.value(kData).toObject();
		QStringList keys;
		const QJsonArray arr = dataObj.value(QStringLiteral("roles")).toArray();
		for (const QJsonValue& v : arr)
			keys.append(v.toString());
		m_userRoleKeys = keys;
		emit userRoleKeysChanged();
		emit operationSucceeded(kUserRoleQuery, text.isEmpty() ? QStringLiteral("ok") : text);
		});
}

void RbacServer::createRole(const QString& roleKey,
							const QString& roleName,
							const QString& description) 
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kRoleCreate, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject data;
	data[kRoleKey] = roleKey;
	data[kRoleName] = roleName;
	if (!description.isEmpty())
		data[kDescription] = description;
	QJsonObject msg;
	msg[kType] = kRoleCreate;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kRoleCreate, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code == ErrorCode::kSuccess)
			emit operationSucceeded(kRoleCreate, text.isEmpty() ? QStringLiteral("ok") : text);
		else
			emit operationFailed(kRoleCreate, code, text);
		});
}

void RbacServer::updateRole(const QString& roleKey, const QVariantMap& fields)
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kRoleUpdate, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject data;
	data[kRoleKey] = roleKey;
	if (fields.contains(kRoleName)) {
		const QString v = fields.value(kRoleName).toString();
		if (!v.isEmpty())
			data[kRoleName] = v;
	}
	if (fields.contains(kDescription)) {
		const QString v = fields.value(kDescription).toString();
		if (!v.isEmpty())
			data[kDescription] = v;
	}
	if (fields.contains(kPermissions)) {
		QJsonArray permArr;
		const QVariant permVar = fields.value(kPermissions);
		if (permVar.typeId() == QMetaType::QStringList) {
			for (const QString& p : permVar.toStringList())
				permArr.append(p);
		}
		else {
			const QVariantList list = permVar.toList();
			for (const QVariant& item : list)
				permArr.append(item.toString());
		}
		data[kPermissions] = permArr;
	}
	if (data.size() <= 1) {
		emit operationFailed(kRoleUpdate, -1,
			QStringLiteral("至少提供 roleName、description 或 permissions 之一"));
		return;
	}
	QJsonObject msg;
	msg[kType] = kRoleUpdate;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kRoleUpdate, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code == ErrorCode::kSuccess)
			emit operationSucceeded(kRoleUpdate, text.isEmpty() ? QStringLiteral("ok") : text);
		else
			emit operationFailed(kRoleUpdate, code, text);
		});
}

void RbacServer::deleteRole(const QString& roleKey)
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kRoleDelete, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject data;
	data[kRoleKey] = roleKey;
	QJsonObject msg;
	msg[kType] = kRoleDelete;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kRoleDelete, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code == ErrorCode::kSuccess)
			emit operationSucceeded(kRoleDelete, text.isEmpty() ? QStringLiteral("ok") : text);
		else
			emit operationFailed(kRoleDelete, code, text);
		});
}

void RbacServer::assignUserRole(int userId, const QString& roleKey)
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kUserRoleAssign, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject data;
	data[kUserId] = userId;
	data[kRoleKey] = roleKey;
	QJsonObject msg;
	msg[kType] = kUserRoleAssign;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kUserRoleAssign, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code == ErrorCode::kSuccess)
			emit operationSucceeded(kUserRoleAssign, text.isEmpty() ? QStringLiteral("ok") : text);
		else
			emit operationFailed(kUserRoleAssign, code, text);
		});
}

void RbacServer::revokeUserRole(int userId, const QString& roleKey)
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kUserRoleRevoke, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject data;
	data[kUserId] = userId;
	data[kRoleKey] = roleKey;
	QJsonObject msg;
	msg[kType] = kUserRoleRevoke;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kUserRoleRevoke, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code == ErrorCode::kSuccess)
			emit operationSucceeded(kUserRoleRevoke, text.isEmpty() ? QStringLiteral("ok") : text);
		else
			emit operationFailed(kUserRoleRevoke, code, text);
		});
}

void RbacServer::querySelfPermissions()
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
        emit operationFailed(kPermissionSelf, -1, QStringLiteral("未连接或未认证"));
        return;
    }

	QJsonObject msg;
    msg[kType] = kPermissionSelf;
    msg[kData] = QJsonObject();
    setBusy(true);
    m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
        setBusy(false);
        if (resp.isEmpty()) {
            emit operationFailed(kPermissionSelf, -1, QStringLiteral("请求超时"));
            return;
        }
        const int code = resp.value(kCode).toInt(-1);
        const QString text = resp.value(kMsg).toString();
        if (code != ErrorCode::kSuccess) {
            emit operationFailed(kPermissionSelf, code, text);
            return;
        }
        const QJsonObject dataObj = resp.value(kData).toObject();
        QStringList perms;
        const QJsonArray arr = dataObj.value(kPermissions).toArray();
        for (const QJsonValue& v : arr)
            perms.append(v.toString());
        m_selfPermissions = perms;
        emit selfPermissionsChanged();
        emit operationSucceeded(kPermissionSelf, text.isEmpty() ? QStringLiteral("ok") : text);
    });
}