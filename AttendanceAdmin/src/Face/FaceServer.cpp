#include "FaceServer.h"
#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"

#include <QFile>
#include <QJsonObject>
#include <QUrl>

using namespace Protocol;

namespace {
	constexpr int kMaxPhotoBase64Chars = 2 * 1024 * 1024; //文件大小 <= 2MB

	QString faceRegisterErrorHint(int code) 
	{
		switch (code)
		{
		case 4001:return QStringLiteral("未检测到人脸");
		case 4002: return QStringLiteral("检测到多张人脸");
		case 4003: return QStringLiteral("人脸尺寸不足");
		case 4004: return QStringLiteral("特征提取失败");
		case 4005: return QStringLiteral("图片格式错误或损坏");
		case 4006: return QStringLiteral("员工不存在");
		case 4007: return QStringLiteral("人脸已存在，需设置 overwrite=true 覆盖");
		default:   return QString();
		}
	}
}//namespace

FaceServer::FaceServer(QObject* parent) : QObject(parent) {}

void FaceServer::setTcpManager(TcpConnectionManager* tcp)
{
	m_tcp = tcp;
}

void FaceServer::setBusy(bool v)
{
	if (m_busy == v) return;
	m_busy = v;
	emit busyChanged();
}

void FaceServer::setLastResult(bool found, const QVariantMap& record)
{
	if (m_lastFound != found) {
		m_lastFound = found;
		emit lastFoundChanged();
	}
	if (m_lastRecord != record) {
		m_lastRecord = record;
		emit lastRecordChanged();
	}
}

bool FaceServer::validateEmployeeId(const QString& employeeId, QString* errorOut)
{
	if (employeeId.trimmed().isEmpty()) {
		if (errorOut) *errorOut = QStringLiteral("工号不能为空");
		return false;
	}
	return true;
}

bool FaceServer::validatePhotoBase64(const QString& photoBast64, QString* errorOut)
{
	const QString b64 = photoBast64.trimmed();
	if (b64.isEmpty()) {
		if (errorOut) *errorOut = QStringLiteral("照片数据不能为空");
		return false;
	}
	return true;
}

QString FaceServer::normalizeLocalPath(const QString& filePathOrUrl)
{
	QString path = filePathOrUrl.trimmed();
	if (path.isEmpty()) return path;
	if (path.startsWith(QStringLiteral("file:"), Qt::CaseInsensitive))
		return QUrl(path).toLocalFile();
	return path;
}

QString FaceServer::readFileAsBase64(const QString& localPath, QString* errorOut)
{
	QFile file(localPath);
	if (!file.open(QIODevice::ReadOnly)) {
		if (errorOut) *errorOut = QStringLiteral("无法打开图片文件：%1").arg(localPath);
		return {};
	}
	const QByteArray bytes = file.readAll();
	if (bytes.isEmpty()) {
		if (errorOut) *errorOut = QStringLiteral("图片文件为空");
		return {};
	}
	const QString b64 = QString::fromLatin1(bytes.toBase64());
	if (b64.size() > kMaxPhotoBase64Chars) {
		if (errorOut) *errorOut = QStringLiteral("图片过大（建议不超过 2MB）");
		return {};
	}
	return b64;
}

QVariantMap FaceServer::parseQueryData(const QJsonObject& dataObj)
{
	QVariantMap row;
	row[QStringLiteral("employeeId")] = dataObj.value(kEmployeeId).toString();
	row[QStringLiteral("featureSize")] = dataObj.value(kFeatureSize).toInt();
	row[QStringLiteral("createdAt")] = dataObj.value(kCreatedAt).toString();
	row[QStringLiteral("updatedAt")] = dataObj.value(kUpdatedAt).toString();
	row[QStringLiteral("found")] = dataObj.value(kFound).toBool();
	return row;
}

QVariantMap FaceServer::parseRegisterData(const QJsonObject& dataObj)
{
	QVariantMap result;
	result[QStringLiteral("employeeId")] = dataObj.value(kEmployeeId).toString();
	result[QStringLiteral("featureSize")] = dataObj.value(kFeatureSize).toInt();
	result[QStringLiteral("isNew")] = dataObj.value(kIsNew).toBool();
	const QJsonObject rect = dataObj.value(kFaceRect).toObject();

	QVariantMap faceRect;
	faceRect[QStringLiteral("left")] = rect.value(QStringLiteral("left")).toInt();
	faceRect[QStringLiteral("top")] = rect.value(QStringLiteral("top")).toInt();
	faceRect[QStringLiteral("right")] = rect.value(QStringLiteral("right")).toInt();
	faceRect[QStringLiteral("bottom")] = rect.value(QStringLiteral("bottom")).toInt();
	result[QStringLiteral("faceRect")] = faceRect;
	return result;
}

void FaceServer::upsertRecord(const QVariantMap& row)
{
	const QString emp = row.value(QStringLiteral("employeeId")).toString();
	if (emp.isEmpty())
		return;
	int idx = -1;
	for (int i = 0; i < m_records.size(); ++i) {
		const QVariantMap existing = m_records.at(i).toMap();
		if (existing.value(QStringLiteral("employeeId")).toString() == emp) {
			idx = i;
			break;
		}
	}
	if (idx >= 0)
		m_records[idx] = row;
	else
		m_records.append(row);
	emit recordsChanged();
}

void FaceServer::removeRecordByEmployeeId(const QString& employeeId)
{
	for (int i = m_records.size() - 1; i >= 0; --i) {
		const QVariantMap row = m_records.at(i).toMap();
		if (row.value(QStringLiteral("employeeId")).toString() == employeeId) {
			m_records.removeAt(i);
			emit recordsChanged();
			return;
		}
	}
}

void FaceServer::clearRecords()
{
	if (m_records.isEmpty())
		return;
	m_records.clear();
	emit recordsChanged();
}

void FaceServer::queryFace(const QString& employeeId)
{
	QString err;
	if (!validateEmployeeId(employeeId, &err)) {
		emit operationFailed(kFaceQuery, -1, err);
		return;
	}
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kFaceQuery, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject data;
	data[kEmployeeId] = employeeId.trimmed();
	QJsonObject msg;
	msg[kType] = kFaceQuery;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kFaceQuery, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		QString text = resp.value(kMsg).toString();
		if (code != ErrorCode::kSuccess) {
			emit operationFailed(kFaceQuery, code, text);
			return;
		}
		const QJsonObject dataObj = resp.value(kData).toObject();
		const bool found = dataObj.value(kFound).toBool(false);
		QVariantMap row = parseQueryData(dataObj);
		setLastResult(found, found ? row : QVariantMap{});
		emit queryCompleted(found, found ? row : QVariantMap{});
		if (found) {
			row.remove(QStringLiteral("found"));
			upsertRecord(row);
			emit operationSucceeded(kFaceQuery,
				text.isEmpty() ? QStringLiteral("查询成功") : text);
		}
		else {
			emit operationSucceeded(kFaceQuery, QStringLiteral("未找到该工号的人脸数据"));
		}
		});
}

void FaceServer::deleteFace(const QString& employeeId)
{
	QString err;
	if (!validateEmployeeId(employeeId, &err)) {
		emit operationFailed(kFaceDelete, -1, err);
		return;
	}
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kFaceDelete, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	const QString emp = employeeId.trimmed();
	QJsonObject data;
	data[kEmployeeId] = emp;
	QJsonObject msg;
	msg[kType] = kFaceDelete;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this, emp](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kFaceDelete, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code != ErrorCode::kSuccess) {
			emit operationFailed(kFaceDelete, code, text);
			return;
		}
		removeRecordByEmployeeId(emp);
		if (m_lastRecord.value(QStringLiteral("employeeId")).toString() == emp)
			setLastResult(false, {});
		emit operationSucceeded(kFaceDelete,
			text.isEmpty() ? QStringLiteral("删除成功") : text);
		});
}

void FaceServer::registerFace(const QString& employeeId,
	const QString& photoBase64,
	bool overwrite)
{
	QString err;
	if (!validateEmployeeId(employeeId, &err)) {
		emit operationFailed(kFaceRegister, -1, err);
		return;
	}
	if (!validatePhotoBase64(photoBase64, &err)) {
		emit operationFailed(kFaceRegister, -1, err);
		return;
	}
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kFaceRegister, -1, QStringLiteral("未连接或未认证"));
		return;
	}
	QJsonObject data;
	data[kEmployeeId] = employeeId.trimmed();
	data[kPhotoBase64] = photoBase64.trimmed();
	data[kOverwrite] = overwrite;
	QJsonObject msg;
	msg[kType] = kFaceRegister;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kFaceRegister, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		QString text = resp.value(kMsg).toString();
		if (code != ErrorCode::kSuccess) {
			const QString hint = faceRegisterErrorHint(code);
			if (!hint.isEmpty() && text.isEmpty())
				text = hint;
			else if (!hint.isEmpty())
				text = hint + QStringLiteral("：") + text;
			emit operationFailed(kFaceRegister, code, text);
			return;
		}
		const QVariantMap result = parseRegisterData(resp.value(kData).toObject());
		emit registerCompleted(result);
		// 注册成功后自动再查一次元数据，刷新表格时间戳
		const QString emp = result.value(QStringLiteral("employeeId")).toString();
		if (!emp.isEmpty())
			queryFace(emp);
		else
			emit operationSucceeded(kFaceRegister,
				text.isEmpty() ? QStringLiteral("注册成功") : text);
		});
}

void FaceServer::registerFaceFromFile(const QString& employeeId,
	const QString& filePathOrUrl,
	bool overwrite)
{
	QString err;
	const QString localPath = normalizeLocalPath(filePathOrUrl);
	if (localPath.isEmpty()) {
		emit operationFailed(kFaceRegister, -1, QStringLiteral("请选择有效的图片文件"));
		return;
	}
	const QString b64 = readFileAsBase64(localPath, &err);
	if (b64.isEmpty()) {
		emit operationFailed(kFaceRegister, -1, err);
		return;
	}
	registerFace(employeeId, b64, overwrite);
}

