#pragma once

#include <QObject>
#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QVariantMap>

class DataManager;

class DataService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStateChanged)

public:
    explicit DataService(DataManager* dataManager, QObject* parent = nullptr);

    bool isConnected() const;

    // ========== Person ==========
    Q_INVOKABLE bool addPerson(const QString& name, const QString& employeeId,
                               const QString& department, const QString& position);
    Q_INVOKABLE bool updatedPerson(const QString& name, const QString& employeeId,
                                   const QString& department, const QString& position);
    Q_INVOKABLE bool deletePerson(const QString& employeeId);
    Q_INVOKABLE QList<QObject*> getAllPerson();
    Q_INVOKABLE QObject* getPersonById(int id);
    Q_INVOKABLE QObject* getPersonByEmployeeId(const QString& employeeId);
    Q_INVOKABLE QList<QObject*> selectPersons(const QString& name, const QString &employeeId,
                                        const QString& department, const QString& position);

    // ========== Attendance ==========
    Q_INVOKABLE bool addAttendanceRecord(const QString& employeeId, const QDateTime& checkTime,
                                         const QString& deviceId, const QString& status);
    Q_INVOKABLE QList<QObject*> selectAttendanceRecord(const QString& employee_id, const QDateTime& startTime, const QDateTime& endTime, const QString& deviceId, const QString& status, const QString& personName);


    // ========== Device ==========
    Q_INVOKABLE bool addOrUpdateDevice(const QString& deviceId, const QString& deviceName,
                                       const QString& ipAddress, const QString& status);
    Q_INVOKABLE bool updateDevice(const QString& deviceId, const QVariantMap& updates);
    Q_INVOKABLE bool updateDeviceStatus(const QString& deviceId, const QString& status);
    Q_INVOKABLE QList<QObject*> getAllDevices();
    Q_INVOKABLE QObject* getDeviceById(const QString& deviceId);
    Q_INVOKABLE QList<QObject*> selectDevice(const QString& deviceId, const QString& deviceName,
                                             const QString& ipAddress, const QString& status);

    // ========== Face data ==========
    Q_INVOKABLE bool addFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector);
    Q_INVOKABLE bool updateFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector);
    Q_INVOKABLE bool deleteFaceDataByEmployeeId(const QString& employeeId);
    Q_INVOKABLE QObject* getFaceDataByEmployeeId(const QString& employeeId);
    Q_INVOKABLE QList<QObject*> getAllFaceData();
    Q_INVOKABLE QList<QObject*> selectFaceData(const QString& employeeId, const QDateTime& createdStart,
                                               const QDateTime& createdEnd, const QDateTime& updatedStart,
                                               const QDateTime& updatedEnd);

signals:
    void connectionStateChanged();
    void operationResult(bool ok, const QString& message);
    void operationTip(const QString& message);
    void deviceStatusChanged(const QString& deviceId, const QString& status);
    void deviceRecordChanged(const QString& deviceId);
    void Data_Validation(const QString& data_Validation);

private:
    DataManager* m_dataManager = nullptr;

    bool ensureConnected();
};

