#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QMutex>

class Logger
{
public:
    enum Level { Info, Warn, Error };
    enum Category { System, Login, Data, Config, Network, Attendance, Sync, Recognition, Command };

    static Logger *instance();

    void log(Level level, Category category, const QString &message,
             const QString &detail = QString());

    void setLogFilePath(const QString &dirPath);
    void setRetentionDays(int days);
    void cleanOldLogs();

private:
    Logger();
    ~Logger();

    Q_DISABLE_COPY(Logger)

    void openLogFile();
    static QString levelToString(Level level);
    static QString categoryToString(Category category);

    QFile   m_logFile;
    QMutex  m_mutex;
    QString m_logDir;
    int     m_retentionDays = 30;
};

#define LOG_SYS(level, msg, ...)    Logger::instance()->log(level, Logger::System, msg, ##__VA_ARGS__)
#define LOG_LOGIN(level, msg, ...)  Logger::instance()->log(level, Logger::Login, msg, ##__VA_ARGS__)
#define LOG_DATA(level, msg, ...)   Logger::instance()->log(level, Logger::Data, msg, ##__VA_ARGS__)
#define LOG_CFG(level, msg, ...)    Logger::instance()->log(level, Logger::Config, msg, ##__VA_ARGS__)
#define LOG_NET(level, msg, ...)    Logger::instance()->log(level, Logger::Network, msg, ##__VA_ARGS__)
#define LOG_ATT(level, msg, ...)    Logger::instance()->log(level, Logger::Attendance, msg, ##__VA_ARGS__)
#define LOG_SYNC(level, msg, ...)   Logger::instance()->log(level, Logger::Sync, msg, ##__VA_ARGS__)
#define LOG_FACE(level, msg, ...)   Logger::instance()->log(level, Logger::Recognition, msg, ##__VA_ARGS__)
#define LOG_CMD(level, msg, ...)    Logger::instance()->log(level, Logger::Command, msg, ##__VA_ARGS__)

#endif // LOGGER_H
