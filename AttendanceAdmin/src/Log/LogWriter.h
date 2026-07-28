#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QDir>
#include <QMutex>

class LogWriter : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(LogWriter)
public:
    explicit LogWriter(QObject *parent = nullptr);

    // 写入业务操作日志
    Q_INVOKABLE void logOperation(const QString &level, const QString &type,
                                   const QString &message, const QVariantMap &details = {});

    // 写入错误日志
    Q_INVOKABLE void logError(const QString &type, const QString &message,
                               const QVariantMap &details = {});

signals:
    void logWriteFailed(const QString &error);

private:
    void writeLog(const QString &level, const QString &type,
                  const QString &message, const QVariantMap &details);
    QString getLogFilePath();
    void rotateLogs();
    void cleanOldLogs();

    QDir m_logDir;
    QMutex m_mutex;
    int m_writeCount = 0;
};

#endif // LOGWRITER_H
