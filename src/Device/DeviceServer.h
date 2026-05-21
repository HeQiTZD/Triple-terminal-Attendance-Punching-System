#ifndef DEVICESERVER_H
#define DEVICESERVER_H

#include <QObject>
#include <QVariantList>
#include <QString>

class TcpConnectionManager;

class DeviceServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QVariantList records READ records NOTIFY recordsChanged)

public:
    explicit DeviceServer(QObject *parent = nullptr);
    void setTcpManager(TcpConnectionManager *tcp);

    bool busy() const { return m_busy; }
    QVariantList records() const { return m_records; }

    Q_INVOKABLE void createDevice(const QString &deviceId,
                                  const QString &deviceName,
                                  const QString &ipAddress,
                                  const QString &status);
    Q_INVOKABLE void queryDevices(const QString &deviceId,
                                  const QString &deviceName,
                                  const QString &ipAddress);
    Q_INVOKABLE void updateDevice(const QString &deviceId,
                                  const QString &deviceName,
                                  const QString &ipAddress,
                                  const QString &status);
    Q_INVOKABLE void deleteDevice(const QString &deviceId);
    Q_INVOKABLE void approveDevice(const QString &deviceId);
    Q_INVOKABLE void sendCommand(const QString &deviceId,
                                 const QString &command,
                                 const QString &paramsJson);

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

#endif // DEVICESERVER_H
