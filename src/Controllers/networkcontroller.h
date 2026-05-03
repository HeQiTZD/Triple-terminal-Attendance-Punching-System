#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include <QDateTime>

#include "../TcpServer/tcpserver.h"
#include "../Services/dataservice.h"
#include "../Protocol/protocol.h"
class TcpServer;
class DataService;

class networkcontroller : public QObject
{
    Q_OBJECT
public:
     explicit networkcontroller(TcpServer* tcpserver, DataService* dataService, QObject* parent = nullptr);

private slots:
    void onClientConnected(const QString &deviceId, const QString &ipAddress);
    void onClientDisconnected(const QString &deviceId);
    void onAttendanceRecordReceived(const QJsonObject &record);
    void onDeviceStatusReceiced(const QString &deviceId, const QJsonObject &status);

private:
    TcpServer* m_tcpServer;
    DataService* m_dataService;

    bool validateAttendanceRecord(const QJsonObject &obj, QString* error) const;
    bool validateDeviceStatus(const QJsonObject &obj,QString *error) const;
};

#endif // NETWORKCONTROLLER_H
