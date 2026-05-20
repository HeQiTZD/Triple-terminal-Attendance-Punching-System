#pragma once

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QString>
#include <QDebug>

class DatabaseManager {
public:
    /// 获取或创建当前线程的独立数据库连接
    static QSqlDatabase getDatabase(const QString &dbPath) {
        const QString connName = QStringLiteral("attendance_0x%1")
            .arg(reinterpret_cast<quintptr>(QThread::currentThread()), 0, 16);

        if (QSqlDatabase::contains(connName)) {
            return QSqlDatabase::database(connName);
        }

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(dbPath);
        if (!db.open()) {
            qWarning() << "DatabaseManager: 线程" << connName << "打开数据库失败";
            return QSqlDatabase();
        }

        QSqlQuery q(db);
        q.exec("PRAGMA journal_mode=WAL");
        q.exec("PRAGMA foreign_keys=ON");
        q.exec("PRAGMA encoding='UTF-8'");

        qDebug() << "DatabaseManager: 为线程" << connName << "创建数据库连接";
        return db;
    }
};
