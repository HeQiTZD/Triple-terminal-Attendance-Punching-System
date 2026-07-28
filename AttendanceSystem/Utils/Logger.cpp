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
    QString logDir = QCoreApplication::applicationDirPath() + QStringLiteral("/logs");
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

void Logger::setRetentionDays(int days)
{
    QMutexLocker locker(&m_mutex);
    m_retentionDays = days;
}

void Logger::cleanOldLogs()
{
    QMutexLocker locker(&m_mutex);

    QDir logDir(m_logDir);
    if (!logDir.exists())
        return;

    const QDate cutoffDate = QDate::currentDate().addDays(-m_retentionDays);
    const QStringList filters = {QStringLiteral("attendance_*.log")};
    const QFileInfoList fileList = logDir.entryInfoList(filters, QDir::Files, QDir::Name);

    for (const QFileInfo &fileInfo : fileList) {
        const QString baseName = fileInfo.baseName();
        const QString dateStr = baseName.mid(11);
        const QDate fileDate = QDate::fromString(dateStr, QStringLiteral("yyyy-MM-dd"));

        if (fileDate.isValid() && fileDate < cutoffDate) {
            if (QFile::remove(fileInfo.absoluteFilePath())) {
                qDebug() << "已删除过期日志文件:" << fileInfo.fileName();
            } else {
                qWarning() << "删除日志文件失败:" << fileInfo.fileName();
            }
        }
    }
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
    case Info:  return QStringLiteral("INFO");
    case Warn:  return QStringLiteral("WARN");
    case Error: return QStringLiteral("ERROR");
    }
    return QStringLiteral("UNKNOWN");
}

QString Logger::categoryToString(Category category)
{
    switch (category) {
    case System:     return QStringLiteral("SYSTEM");
    case Login:      return QStringLiteral("LOGIN");
    case Data:       return QStringLiteral("DATA");
    case Config:     return QStringLiteral("CONFIG");
    case Network:    return QStringLiteral("NETWORK");
    case Attendance: return QStringLiteral("ATTENDANCE");
    case Sync:       return QStringLiteral("SYNC");
    case Recognition:return QStringLiteral("RECOGNITION");
    case Command:    return QStringLiteral("COMMAND");
    }
    return QStringLiteral("UNKNOWN");
}

void Logger::log(Level level, Category category, const QString &message,
                 const QString &detail)
{
    QMutexLocker locker(&m_mutex);

    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
    const QString levelStr  = levelToString(level);
    const QString categoryStr = categoryToString(category);

    QString formatted;
    if (detail.isEmpty()) {
        formatted = QStringLiteral("[%1] [%2] [%3] %4")
                        .arg(timestamp, levelStr, categoryStr, message);
    } else {
        formatted = QStringLiteral("[%1] [%2] [%3] %4 | %5")
                        .arg(timestamp, levelStr, categoryStr, message, detail);
    }

    // Write to console
    switch (level) {
    case Info:
        qInfo().noquote() << formatted;
        break;
    case Warn:
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
        openLogFile();
    }

    if (m_logFile.isOpen()) {
        m_logFile.write(formatted.toUtf8());
        m_logFile.write("\n");
        m_logFile.flush();
    }
}
