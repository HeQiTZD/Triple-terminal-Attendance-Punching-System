#ifndef LOCALSTORAGE_H
#define LOCALSTORAGE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>
#include <QMutex>
#include "../NetworkClient/serverprotocol.h"

class LocalStorage : public QObject
{
    Q_OBJECT
public:
    static LocalStorage* instance();

    bool connectDatabse();

    //人员同步方式（事务）
    bool syncPersons(const QVector<ServerProtocol::PersonData> &person);

    //打卡记录操作
    bool addAttendanceRecord(const QString &employeeId, const QString &status);
    QVector<ServerProtocol::AttendanceRecord> getUnuploadedRecords();
    bool markAsUploaded(int recordId);
    bool markBatchAsUploaded(const QVector<int> &recordIds);

signals:
    void personsSyncCompleted(int count);
    void personsSyncFailed(const QString &error);

private:
    explicit LocalStorage(QObject *parent = nullptr);
    ~LocalStorage();

    QSqlDatabase m_db;
    static QMutex s_mutex;
    static LocalStorage* s_instance;
};

#endif // LOCALSTORAGE_H
