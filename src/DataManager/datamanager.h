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
#include <QString>

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
    bool initialize(const QString &host,const QString &database,const QString &username,const QString &password);
    void close();
    bool isConnected() const;

    // ========== 人员管理 CRUD ==========
    Q_INVOKABLE bool addPerson(const QString &name,const QString &employeeId,const QString &department,const QString &position);
    bool updatedPerson(const QString &employeeId, const QVariantMap &updates);
    Q_INVOKABLE bool deletePerson(const QString &employeeId);
    Q_INVOKABLE QList<QObject*> getAllPerson();
    Q_INVOKABLE QObject* getPersonById(int id);
    Q_INVOKABLE QObject* getPersonByEmployeeId(const QString &employeeId);
    Q_INVOKABLE QList<QObject*> selectPersons(const QString &name,const QString &employeeId,const QString &department,const QString &position);

    // ========== 打卡记录管理 ==========
    Q_INVOKABLE bool addAttendanceRecord(const QString &employeeId, const QDateTime &checkTime,const QString &deviceId, const QString &status);
    Q_INVOKABLE QList<QObject*> selectAttendanceRecord(const QString& employee_id, const QDateTime& startTime, const QDateTime& endTime, const QString& deviceId, const QString& status, const QString& personName);

    // ========== 设备管理 ==========
    Q_INVOKABLE bool addOrUpdateDevice(const QString &deviceId, const QString &deviceName,
                                       const QString &ipAddress, const QString &status);
    bool updateDevice(const QString &deviceId, const QVariantMap &updates);
    Q_INVOKABLE bool updateDeviceStatus(const QString &deviceId, const QString &status);
    Q_INVOKABLE QList<QObject*> getAllDevices();
    Q_INVOKABLE QObject* getDeviceById(const QString &deviceId);
    Q_INVOKABLE QList<QObject*> selectDevice(const QString &deviceId, const QString &deviceName,
                                            const QString &ipAddress, const QString &status);

    // ========== 人脸数据管理（基于employee_id） ==========
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
    // 操作结果信号：承载“数据库操作”的成功/失败结果信息（包含成功与失败）
    void operationResult(bool ok, const QString &message);
    // 操作提示信号：承载面向用户的操作提示/引导信息（不代表数据库操作结果）
    void operationTip(const QString &message);

    void personAdded(int id);
    void personDeleted(int id);
    void attendanceRecordAdded(int id);
    void deviceStatusChanged(const QString &devicdId,const QString &status);
    void deviceRecordChanged(const QString &deviceId);

    void faceDataAdded(int id, const QString &employeeId);
    void faceDataUpdated(int id, const QString &employeeId);
    void faceDataDeleted(int id);

private:
    QSqlDatabase m_db;
    bool m_isConnected;

    bool createTables();
    bool createPersonTable();
    bool createAttendanceRecordTable();
    bool createAttendanceRecordArchiveTable();
    bool createDeviceTable();
    bool createFaceDataTable();
};

#endif // DATAMANAGER_H
