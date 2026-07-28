#pragma once

#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "../Utils/DatabaseManager.h"

class BaseRepository {
public:
    explicit BaseRepository(const QString &dbPath)
        : m_dbPath(dbPath) {}
    virtual ~BaseRepository() = default;

protected:
    // INSERT / UPDATE / DELETE — 返回是否成功
    bool executeNonQuery(const QString &sql,
                        const QVariantMap &params = {})
    {
        QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
        QSqlQuery query(db);
        query.prepare(sql);
        for (auto it = params.begin(); it != params.end(); ++it)
            query.bindValue(it.key(), it.value());
        return query.exec();
    }

    // SELECT 单值（如 COUNT(*)）
    QVariant executeScalar(const QString &sql,
                          const QVariantMap &params = {},
                          const QVariant &defaultValue = {})
    {
        QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
        QSqlQuery query(db);
        query.prepare(sql);
        for (auto it = params.begin(); it != params.end(); ++it)
            query.bindValue(it.key(), it.value());
        if (query.exec() && query.next())
            return query.value(0);
        return defaultValue;
    }

    // SELECT 多行 — 回调模式，QSqlDatabase 生命周期由 executeReader 保证
    template<typename RowFunc>
    bool executeReader(const QString &sql,
                      const QVariantMap &params,
                      RowFunc &&rowHandler)
    {
        QSqlDatabase db = DatabaseManager::getDatabase(m_dbPath);
        QSqlQuery query(db);
        query.prepare(sql);
        for (auto it = params.begin(); it != params.end(); ++it)
            query.bindValue(it.key(), it.value());
        if (!query.exec())
            return false;
        while (query.next())
            rowHandler(query);   // QSqlDatabase db 在此作用域内始终存活
        return true;
    }

    QString m_dbPath;
};
