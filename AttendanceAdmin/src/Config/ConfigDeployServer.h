#ifndef CONFIGDEPLOYSERVER_H
#define CONFIGDEPLOYSERVER_H

#include <QObject>
#include <QString>
#include <QVariantList>

class TcpConnectionManager;

class ConfigDeployServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QVariantList deployTargets READ deployTargets NOTIFY deployResultChanged)
    Q_PROPERTY(QString lastDeployId READ lastDeployId NOTIFY deployResultChanged)
    Q_PROPERTY(QString configVersion READ configVersion NOTIFY deployResultChanged)
    Q_PROPERTY(QString configHash READ configHash NOTIFY deployResultChanged)

public:
    explicit ConfigDeployServer(QObject *parent = nullptr);
    void setTcpManager(TcpConnectionManager *tcp);

    bool busy() const { return m_busy; }
    QVariantList deployTargets() const { return m_deployTargets; }
    QString lastDeployId() const { return m_lastDeployId; }
    QString configVersion() const { return m_configVersion; }
    QString configHash() const { return m_configHash; }

    Q_INVOKABLE void deployConfig(const QVariantList &targetDevices,
                                  const QString &configContent,
                                  const QString &configVersion,
                                  const QString &description);

signals:
    void busyChanged();
    void deployResultChanged();
    void operationSucceeded(const QString &apiType, const QString &message);
    void operationFailed(const QString &apiType, int code, const QString &message);

private:
    void setBusy(bool value);
    void clearDeployResult();
    static QVariantList parseTargets(const QJsonObject &dataObj);

    TcpConnectionManager *m_tcp = nullptr;
    bool m_busy = false;
    QVariantList m_deployTargets;
    QString m_lastDeployId;
    QString m_configVersion;
    QString m_configHash;
};

#endif // CONFIGDEPLOYSERVER_H
