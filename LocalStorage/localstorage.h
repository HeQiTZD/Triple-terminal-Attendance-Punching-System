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
#include "../NetworkClient/protocol.h"
class LocalStorage : public QObject
{
    Q_OBJECT
public:
    //单例模式
    static LocalStorage* instance();

    //连接数据库
    bool connectDatabse();

    //人员同步方式（事务）
    bool syncPersons(const QVector<Protocol::PersonData> &person);

    //打卡记录操作
    bool addAttendanceRecord(const QString &employeeId,const QString &status);
    QVector<Protocol::AttendanceRecord> getUnuploadedRecords();
    bool markAsUploaded(int recordId);
    bool markBatchAsUploaded(const QVector<int> &recordIds);

signals:
    //同步完成信号
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
