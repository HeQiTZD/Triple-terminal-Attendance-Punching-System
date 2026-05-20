#include "Logger.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>

Logger *Logger::instance()
{
    static Logger inst;
    return &inst;
}

Logger::Logger()
{
    // Log path will be set later via setLogFilePath() or fall back to default
    QString logDir;
    // Try to read from ConfigManager, but since ConfigManager may not be initialized yet,
    // use a reasonable default and let setLogFilePath() be called after config loads.
    logDir = QCoreApplication::applicationDirPath() + QStringLiteral("/logs");
    m_logDir = logDir;
}

Logger::~Logger()
{
    if (m_logFile.isOpen())
        m_logFile.close();
}

void Logger::setLogFilePath(const QString &dirPath)
{
    QMutexLocker locker(&m_mutex);
    if (dirPath.isEmpty())
        return;

    if (m_logFile.isOpen())
        m_logFile.close();

    m_logDir = dirPath;
    openLogFile();
}

void Logger::openLogFile()
{
    QDir dir;
    if (!dir.exists(m_logDir))
        dir.mkpath(m_logDir);

    const QString date = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd"));
    const QString fileName = m_logDir + QStringLiteral("/attendance_") + date + QStringLiteral(".log");

    m_logFile.setFileName(fileName);
    if (!m_logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Logger: failed to open log file:" << fileName;
    }
}

QString Logger::levelToString(Level level)
{
    switch (level) {
    case Debug:   return QStringLiteral("DEBUG");
    case Info:    return QStringLiteral("INFO");
    case Warning: return QStringLiteral("WARNING");
    case Error:   return QStringLiteral("ERROR");
    }
    return QStringLiteral("UNKNOWN");
}

void Logger::log(Level level, const QString &message,
                 const char *file, int line, const char *func)
{
    QMutexLocker locker(&m_mutex);

    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz"));
    const QString levelStr  = levelToString(level);

    QString formatted;
    if (file && func) {
        // Extract just the filename (without full path)
        const char *shortFile = strrchr(file, '/');
        if (!shortFile) shortFile = strrchr(file, '\\');
        formatted = QStringLiteral("[%1] [%2] [%3:%4 %5] %6")
                        .arg(timestamp, levelStr)
                        .arg(shortFile ? shortFile + 1 : file)
                        .arg(line)
                        .arg(QString::fromLatin1(func))
                        .arg(message);
    } else {
        formatted = QStringLiteral("[%1] [%2] %3")
                        .arg(timestamp, levelStr, message);
    }

    // Write to console
    switch (level) {
    case Debug:
        qDebug().noquote() << formatted;
        break;
    case Info:
        qInfo().noquote() << formatted;
        break;
    case Warning:
        qWarning().noquote() << formatted;
        break;
    case Error:
        qCritical().noquote() << formatted;
        break;
    }

    // Write to file (check if we need to roll to a new day)
    if (m_logFile.isOpen()) {
        const QString today = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd"));
        const QString currentFile = m_logDir + QStringLiteral("/attendance_") + today + QStringLiteral(".log");
        if (m_logFile.fileName() != currentFile) {
            m_logFile.close();
            m_logFile.setFileName(currentFile);
            (void)m_logFile.open(QIODevice::Append | QIODevice::Text);
        }
    } else {
        // Try to reopen
        openLogFile();
    }

    if (m_logFile.isOpen()) {
        m_logFile.write(formatted.toUtf8());
        m_logFile.write("\n");
        m_logFile.flush();
    }
}
