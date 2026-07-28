#ifndef USERSERVER_H
#define USERSERVER_H

#include <QObject>
#include <QVariantList>
#include <QString>

class TcpConnectionManager;

class UserServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QVariantList records READ records NOTIFY recordsChanged)

public:
    explicit UserServer(QObject *parent = nullptr);
    void setTcpManager(TcpConnectionManager *tcp);

    bool busy() const { return m_busy; }
    QVariantList records() const { return m_records; }

    Q_INVOKABLE void createUser(const QString &employeeId,
                                const QString &password,
                                const QString &name);
    Q_INVOKABLE void queryUsers(const QString &employeeId,
                                const QString &createdAt);
    Q_INVOKABLE void updateUser(const QString &employeeId,
                                const QString &name,
                                const QString &password);
    Q_INVOKABLE void deleteUser(const QString &employeeId);

signals:
    void busyChanged();
    void recordsChanged();
    void operationSucceeded(const QString &apiType, const QString &message);
    void operationFailed(const QString &apiType, int code, const QString &message);

private:
    void setBusy(bool v);
    static QVariantList parseRecords(const QJsonObject &dataObj);

    TcpConnectionManager *m_tcp = nullptr;
    bool m_busy = false;
    QVariantList m_records;
};

#endif // USERSERVER_H
