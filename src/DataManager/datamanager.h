#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QJsonObject>
#include <QSet>
#include <QVariantMap>
#include <cmath>

#include "../Models/faceData.h"
#include "../Models/Person.h"
#include "../Models/device.h"
#include "../Models/attendancerecord.h"

class DataManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStateChanged)

public:
    explicit DataManager(QObject *parent = nullptr);
    ~DataManager();
    bool initialize(const QString &host,const QString &databaxe,const QString &username,const QString &password);
    void close();
    bool isConnected() const;

    // ========== 人员管理 CRUD ==========
    Q_INVOKABLE bool addPerson(const QString &name,const QString employeeId,const QString &department,const QString &position);
    Q_INVOKABLE bool updatedPerson(const QString &name,const QString &employeeId,const QString &department,const QString &position);
    Q_INVOKABLE bool deletePerson(const QString &name,const QString &employeeId);
    Q_INVOKABLE QList<QObject*> getAllPerson();
    Q_INVOKABLE QObject* getPersonById(int id);
    Q_INVOKABLE QObject* getPersonByEmployeeId(const QString &employeeId);
    Q_INVOKABLE bool updatePersonFaceFeature(int id,const QByteArray &faceFeature);

    // ========== 打卡记录管理 ==========
    Q_INVOKABLE bool addAttendanceRecore(int personId, const QDateTime &checkTime,const QString &deviceId, const QString &status);
    Q_INVOKABLE QList<QObject*> getAttendanceRecords(const QDateTime &startTime,const QDateTime &endTime);
    Q_INVOKABLE QList<QObject*> getAttendanceRecordsByPerson(int personId,const QDateTime &startTime,const QDateTime &endTime);

    // ========== 设备管理 ==========
    Q_INVOKABLE bool addOrUpdateDevice(const QString &deviceId, const QString &deviceName,
                                       const QString &ipAddress, const QString &status);
    Q_INVOKABLE bool updateDeviceStatus(const QString &deviceId, const QString &status);
    Q_INVOKABLE QList<QObject*> getAllDevices();
    Q_INVOKABLE QObject* getDeviceById(const QString &deviceId);

    // ========== 人脸数据管理（基于employee_id） ==========
    Q_INVOKABLE bool addFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector);
    Q_INVOKABLE bool updateFaceDataByEmployeeId(const QString& employeeId, const QByteArray& featureVector);
    Q_INVOKABLE bool deleteFaceDataByEmployeeId(const QString& employeeId);
    Q_INVOKABLE QObject* getFaceDataByEmployeeId(const QString& employeeId);
    Q_INVOKABLE QList<QObject*> getAllFaceData();

signals:
    void connectionStateChanged();
    void errorOccurred(const QString &errorString);
    void personAdded(int id);
    void personUpdated(int id);
    void personDeleted(int id);
    void attendanceRecordAdded(int id);
    void deviceStatusChanged(const QString &devicdId,const QString &status);

    void faceDataAdded(int id, int personId);
    void faceDataUpdated(int id, int personId);
    void faceDataDeleted(int id);

private:
    QSqlDatabase m_db;
    bool m_isConnected;

    bool createTables();
    bool createPersonTable();
    bool createAttendanceRecordTable();
    bool createDeviceTable();
    bool createFaceDataTable();
};

#endif // DATAMANAGER_H
