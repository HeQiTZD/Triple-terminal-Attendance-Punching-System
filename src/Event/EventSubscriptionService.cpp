#include "EventSubscriptionService.h"
#include "../Network/TcpConnectionManager.h"
#include "../Protocol/protocol.h"
using namespace Protocol;
namespace {
	constexpr int kMaxTopicsPerRequest = 32;

	bool isPushType(const QString& type) 
	{
		return type.endsWith(QStringLiteral(".push"));
	}
}

EventSubscriptionService::EventSubscriptionService(QObject *parent) :QObject(parent) {}

void EventSubscriptionService::setTcpManager(TcpConnectionManager* tcp)
{
	if (m_tcp == tcp) return;

	if (m_tcp) {
		QObject::disconnect(m_tcp, nullptr, this, nullptr);
	}
	
	m_tcp = tcp;

	if (m_tcp) {
		connect(m_tcp, &TcpConnectionManager::messageReceived,
				this, &EventSubscriptionService::onTcpMessageReceived);
		connect(m_tcp, &TcpConnectionManager::authenticatedChanged,
				this, &EventSubscriptionService::onAuthenticatedChanged);
	}
}

void EventSubscriptionService::setBusy(bool v)
{
	if (m_busy == v) return;
	m_busy = v;
	emit busyChanged();
}

QStringList EventSubscriptionService::uniqueStrings(const QStringList& in, int maxCount, QString* errorOut)
{
	QStringList out;
	QSet<QString> seen;
	for (const QString& s : in) {
		const QString t = s.trimmed();
		if (t.isEmpty()) continue;
		if (seen.contains(t)) continue;
		seen.insert(t);
		out.append(t);
		if (out.size() >= maxCount) break;
	}
	if (in.size() > maxCount && errorOut)
		*errorOut = QStringLiteral("单次最多 %1 个主题").arg(maxCount);
	return out;
}

void EventSubscriptionService::mergeSubscribed(const QStringList &add)
{
	QSet<QString> set(m_subscribedTopics.begin(), m_subscribedTopics.end());
	for (const QString& t : add) {
		if (set.size() >= kMaxTopicsPerRequest)
			break;
		if (!set.contains(t)) set.insert(t);
	}

	const QStringList merged = QStringList(set.begin(), set.end());
	if (merged == m_subscribedTopics) return;
	m_subscribedTopics = merged;
	emit subscribedTopicsChanged();
}

void EventSubscriptionService::removeSubscribed(const QStringList& remove)
{
	QSet<QString> rser(remove.begin(), remove.end());
	QStringList next;
	next.reserve(m_subscribedTopics.size());
	for (const QString& t : m_subscribedTopics) {
		if (!rser.contains(t)) next.append(t);
	}
	if (next == m_subscribedTopics) return;
	m_subscribedTopics = next;
	emit subscribedTopicsChanged();
}

void EventSubscriptionService::subscribe(const QStringList& topics)
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kSubscribe, -1, QStringLiteral("未连接或未认证"));
		return;
	}

	QString err;
	const QStringList cleaned = uniqueStrings(topics, kMaxTopicsPerRequest, &err);
	if (cleaned.isEmpty()) {
		emit operationFailed(kSubscribe, -1, QStringLiteral("topics 不能为空"));
		return;
	}

	if (!err.isEmpty()) {
		emit operationFailed(kSubscribe, -1, err);
		return;
	}

	QJsonArray arr;
	for (const QString& t : cleaned) {
		arr.append(t);
	}

	QJsonObject data;
	data[kTopics] = arr;

	QJsonObject msg;
	msg[kType] = kSubscribe;
	msg[kData] = data;
	setBusy(true);
	m_tcp->sendMessage(msg, [this, cleaned](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kSubscribe, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code != ErrorCode::kSuccess) {
			emit operationFailed(kSubscribe, code, text);
			return;
		}
		mergeSubscribed(cleaned);
		emit operationSucceeded(kSubscribe, text.isEmpty() ? QStringLiteral("ok") : text);
		return;
	});
}

void EventSubscriptionService::unsubscribe(const QStringList &topice)
{
	if (!m_tcp || !m_tcp->isAuthenticated()) {
		emit operationFailed(kUnsubscribe, -1, QStringLiteral("未连接或未认证"));
		return;
	}

	QString err;
	const QStringList cleaned = uniqueStrings(topice, kMaxTopicsPerRequest, &err);
	if (cleaned.isEmpty()) {
		emit operationFailed(kUnsubscribe, -1, QStringLiteral("topics 不能为空"));
		return;
	}
	if (!err.isEmpty()) {
		emit operationFailed(kUnsubscribe, -1, err);
		return;
	}

	QJsonArray  arr;
	for (const QString& t : cleaned) arr.append(t);

	QJsonObject data;
	data[kTopics] = arr;

	QJsonObject msg;
	msg[kType] = kUnsubscribe;
	msg[kData] = data;

	setBusy(true);
	m_tcp->sendMessage(msg, [this, cleaned](const QJsonObject& resp) {
		setBusy(false);
		if (resp.isEmpty()) {
			emit operationFailed(kUnsubscribe, -1, QStringLiteral("请求超时"));
			return;
		}
		const int code = resp.value(kCode).toInt(-1);
		const QString text = resp.value(kMsg).toString();
		if (code != ErrorCode::kSuccess) {
			emit operationFailed(kUnsubscribe, code, text);
			return;
		}
		removeSubscribed(cleaned);
		emit operationSucceeded(kUnsubscribe, text.isEmpty() ? QStringLiteral("ok") : text);
		return;
		});
}

void EventSubscriptionService::onTcpMessageReceived(const QJsonObject& message)
{
	const QString type = message.value(kType).toString();
	if (!isPushType(type)) return;

	const QJsonObject dataObj = message.value(kData).toObject();
	emit serverPushReceived(type, dataObj.toVariantMap());
}

void EventSubscriptionService::onAuthenticatedChanged()
{
	if (m_tcp && m_tcp->isAuthenticated()) {
		// 认证成功后自动订阅设备状态和考勤推送
		QStringList autoSubscribeTopics;
		autoSubscribeTopics << QStringLiteral("device") << QStringLiteral("attendance");
		subscribe(autoSubscribeTopics);
		return;
	}

	// 断开连接时清理订阅列表
	if (!m_subscribedTopics.isEmpty()) {
		m_subscribedTopics.clear();
		emit subscribedTopicsChanged();
	}
}
