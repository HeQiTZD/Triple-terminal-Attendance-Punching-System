#ifndef EVENTSUBSCRIPTIONSERVICE_H
#define EVENTSUBSCRIPTIONSERVICE_H

#include <QObject>
#include <QStringList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
class TcpConnectionManager;

class EventSubscriptionService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
	Q_PROPERTY(QStringList subscribedTopics READ subscribedTopics NOTIFY subscribedTopicsChanged)

public:
	explicit EventSubscriptionService(QObject* parent = nullptr);
	void setTcpManager(TcpConnectionManager* tcp);

	bool busy() const { return m_busy; }
	QStringList subscribedTopics() const { return m_subscribedTopics; }

	Q_INVOKABLE void subscribe(const QStringList& topice);
	Q_INVOKABLE void unsubscribe(const QStringList& topice);

signals:
	void busyChanged();
	void subscribedTopicsChanged();

	void operationSuccessded(const QString& apiType, const QString &message);
	void operationFailed(const QString& apiTYpe, int code, QString& message);

	/// 服务端推送（如 attendance.push）；messageType 为 JSON 的 type 字段
	void serverPushReceived(const QString& messagetype, const QVariantMap &data);

private slots:
	void onTcpMessageReceived(const QString& messagetype);
	void onAuthenticatedChanged();

private:
	void setBusy(bool v);
	static QStringList uniqueStrings(const QStringList& in);
	void mergeSubscribed(const QStringList& add);
	void removeSubscribed(const QStringList& remove);

	TcpConnectionManager* m_tcp = nullptr;
	bool m_busy = false;
	QStringList m_subscribedTopics;

};

#endif