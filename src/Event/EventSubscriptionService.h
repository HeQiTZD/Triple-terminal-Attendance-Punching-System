#ifndef EVENTSUBSCRIPTIONSERVICE_H
#define EVENTSUBSCRIPTIONSERVICE_H

#include <QObject>
#include <QStringList>//存储订阅主题列表
#include <QJsonArray>//解析JSON数组
#include <QJsonDocument>//解析JSON文档
#include <QJsonObject>//解析JSON对象
#include <QSet>//解析Set，去重操作
class TcpConnectionManager;

class EventSubscriptionService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
	Q_PROPERTY(QStringList subscribedTopics READ subscribedTopics NOTIFY subscribedTopicsChanged)

public:
	explicit EventSubscriptionService(QObject* parent = nullptr);
	void setTcpManager(TcpConnectionManager* tcp);//设置TCP连接管理器

	bool busy() const { return m_busy; }//获取繁忙状态
	QStringList subscribedTopics() const { return m_subscribedTopics; }//获取订阅主题列表

	Q_INVOKABLE void subscribe(const QStringList& topice);//订阅指定主题列表
	Q_INVOKABLE void unsubscribe(const QStringList& topice);//取消订阅指定主题列表	

signals:
	void busyChanged();//繁忙状态改变信号
	void subscribedTopicsChanged();//订阅主题列表改变信号

	void operationSucceeded(const QString& apiType, const QString &message);//操作成功信号
	void operationFailed(const QString& apiType, int code, const QString& message);//操作失败信号

	/// 服务端推送（如 attendance.push）；messageType 为 JSON 的 type 字段
	void serverPushReceived(const QString& messageType, const QVariantMap &data);//收到服务端推送消息

private slots:
	void onTcpMessageReceived(const QJsonObject& messageType);//收到TCP消息
	void onAuthenticatedChanged();//认证状态改变

private:
	void setBusy(bool v);//设置繁忙状态
	static QStringList uniqueStrings(const QStringList& in, int maxCount, QString* errorOut);//去重并限制数量（静态工具方法）
	void mergeSubscribed(const QStringList& add);//合并订阅主题列表
	void removeSubscribed(const QStringList& remove);//移除订阅主题列表	

	TcpConnectionManager* m_tcp = nullptr;//TCP连接管理器
		bool m_busy = false;//繁忙状态
		QStringList m_subscribedTopics;//订阅主题列表

};

#endif