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
class LocalStorage : public QObject
{
    Q_OBJECT
public:
    LocalStorage();

    //连接数据库
    bool connectDatabse();
};

#endif // LOCALSTORAGE_H
