#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QMutex>

class Logger
{
public:
    enum Level { Debug, Info, Warning, Error };

    static Logger *instance();

    void log(Level level, const QString &message,
             const char *file = nullptr, int line = 0, const char *func = nullptr);

    void setLogFilePath(const QString &dirPath);

private:
    Logger();
    ~Logger();

    Q_DISABLE_COPY(Logger)

    void openLogFile();
    static QString levelToString(Level level);

    QFile   m_logFile;
    QMutex  m_mutex;
    QString m_logDir;
};

#define LOG_DEBUG(msg)   Logger::instance()->log(Logger::Debug,   msg, __FILE__, __LINE__, Q_FUNC_INFO)
#define LOG_INFO(msg)    Logger::instance()->log(Logger::Info,    msg, __FILE__, __LINE__, Q_FUNC_INFO)
#define LOG_WARNING(msg) Logger::instance()->log(Logger::Warning, msg, __FILE__, __LINE__, Q_FUNC_INFO)
#define LOG_ERROR(msg)   Logger::instance()->log(Logger::Error,   msg, __FILE__, __LINE__, Q_FUNC_INFO)

#endif // LOGGER_H
