#ifndef PERSONSERVICE_H
#define PERSONSERVICE_H
#include <QObject>
#include <QVariantList>
#include <QString>

class TcpConnectionManager;

class PersonServer : public QObject {
    Q_OBJECT
        Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
        Q_PROPERTY(QVariantList records READ records NOTIFY recordsChanged)

public:
    explicit PersonServer(QObject* parant = nullptr);
    void setTcpManager(TcpConnectionManager* tcp);

    bool busy() const { return m_busy; }
    QVariantList records() const { return m_records; }

    Q_INVOKABLE void createPerson(const QString& name,
        const QString& employeeId,
        const QString& department,
        const QString& position);

    Q_INVOKABLE void queryPersons(const QString& name,
        const QString& employeeId,
        const QString& department,
        const QString& position,
        const QString& createdAt,
        const QString& updatedAt);

    Q_INVOKABLE void updatePerson(const QString& employeeId,
        const QString& name,
        const QString& department,
        const QString& position);

    Q_INVOKABLE void deletePerson(const QString& employeeId);

signals:
    void busyChanged();
    void recordsChanged();

    void operationSucceeded(const QString& apiType, const QString& message);
    void operationFailed(const QString& apiType, int code, const QString& message);

private:
    void setBusy(bool v);
    static QVariantList parseRecords(const QJsonObject& dataObj);

    TcpConnectionManager* m_tcp = nullptr;
    bool m_busy = false;
    QVariantList m_records;
};
#endif