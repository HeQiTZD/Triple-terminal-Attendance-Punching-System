#include "tokenmanager.h"

TokenManager::TokenManager(QObject *parent)
    : QObject(parent)
    , m_settings(std::make_unique<QSettings>("AttendanceSystem", "DeviceToken"))
{
    loadFromSettings();
}

void TokenManager::storeTokens(const TokenPair &tokens)
{
    m_accessToken = tokens.accessToken;
    m_refreshToken = tokens.refreshToken;
    m_expiresAt = QDateTime::currentDateTime().addSecs(tokens.expiresIn);
    saveToSettings();
    emit tokenRefreshed();
}

QString TokenManager::accessToken() const
{
    return m_accessToken;
}

QString TokenManager::refreshToken() const
{
    return m_refreshToken;
}

bool TokenManager::isAccessTokenExpired() const
{
    if (m_accessToken.isEmpty()) {
        return true;
    }
    return QDateTime::currentDateTime() >= m_expiresAt;
}

bool TokenManager::needsRefresh() const
{
    if (m_accessToken.isEmpty()) {
        return false;
    }
    // 提前5分钟刷新
    QDateTime refreshTime = m_expiresAt.addSecs(-300);
    return QDateTime::currentDateTime() >= refreshTime;
}

void TokenManager::clearTokens()
{
    m_accessToken.clear();
    m_refreshToken.clear();
    m_expiresAt = QDateTime();
    saveToSettings();
}

void TokenManager::loadFromSettings()
{
    m_accessToken = m_settings->value("accessToken").toString();
    m_refreshToken = m_settings->value("refreshToken").toString();
    m_expiresAt = m_settings->value("expiresAt").toDateTime();
}

void TokenManager::saveToSettings()
{
    m_settings->setValue("accessToken", m_accessToken);
    m_settings->setValue("refreshToken", m_refreshToken);
    m_settings->setValue("expiresAt", m_expiresAt);
}
