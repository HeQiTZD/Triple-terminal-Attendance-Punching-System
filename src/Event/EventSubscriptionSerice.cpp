#include "EventSubscriptionSerice.h"
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