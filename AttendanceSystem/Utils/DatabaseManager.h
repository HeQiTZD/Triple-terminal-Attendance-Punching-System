#pragma once

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QString>

class DatabaseManager {
public:
    static QSqlDatabase getDatabase(const QString &dbPath) {
        const QString connName = QStringLiteral("attendance_0x%1")
            .arg(reinterpret_cast<quintptr>(QThread::currentThread()), 0, 16);

        if (QSqlDatabase::contains(connName)) {
            return QSqlDatabase::database(connName);
        }

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(dbPath);
        if (!db.open()) {
            return QSqlDatabase();
        }

        QSqlQuery q(db);
        q.exec("PRAGMA journal_mode=WAL");
        q.exec("PRAGMA foreign_keys=ON");
        q.exec("PRAGMA encoding='UTF-8'");

        return db;
    }
};
