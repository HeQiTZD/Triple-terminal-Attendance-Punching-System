#include "LogWriter.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDate>
#include <QFileInfo>
#include <QDirIterator>

LogWriter::LogWriter(QObject *parent)
    : QObject(parent)
{
    m_logDir = QDir(QCoreApplication::applicationDirPath() + "/logs");
    if (!m_logDir.exists()) {
        m_logDir.mkpath(".");
    }
}

void LogWriter::logOperation(const QString &level, const QString &type,
                              const QString &message, const QVariantMap &details)
{
    writeLog(level, type, message, details);
}

void LogWriter::logError(const QString &type, const QString &message,
                          const QVariantMap &details)
{
    writeLog("ERROR", type, message, details);
}

void LogWriter::writeLog(const QString &level, const QString &type,
                          const QString &message, const QVariantMap &details)
{
    QMutexLocker locker(&m_mutex);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString line = QString("[%1] [%2] [%3] %4")
                       .arg(timestamp, level, type, message);

    // 添加详细信息
    if (!details.isEmpty()) {
        QStringList detailParts;
        for (auto it = details.begin(); it != details.end(); ++it) {
            detailParts << QString("%1=%2").arg(it.key(), it.value().toString());
        }
        line += " | " + detailParts.join(" | ");
    }

    QString filePath = getLogFilePath();
    QFile file(filePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << line << "\n";
        file.close();
    } else {
        emit logWriteFailed(QString("Failed to open log file: %1").arg(filePath));
    }

    ++m_writeCount;
    locker.unlock();

    // 每次写入时检查是否需要清理旧日志（在锁外执行，避免阻塞其他线程）
    if (m_writeCount % 100 == 0) {
        cleanOldLogs();
    }
}

QString LogWriter::getLogFilePath()
{
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    return m_logDir.filePath(QString("app-%1.log").arg(dateStr));
}

void LogWriter::rotateLogs()
{
    // 按日期轮转由 getLogFilePath() 自动实现
    // 每天创建新文件
}

void LogWriter::cleanOldLogs()
{
    QDate cutoff = QDate::currentDate().addDays(-30);
    QDirIterator it(m_logDir.path(), {"app-*.log"}, QDir::Files);
    while (it.hasNext()) {
        it.next();
        QFileInfo fi = it.fileInfo();
        // 从文件名解析日期（格式：app-yyyy-MM-dd.log）
        QString baseName = fi.completeBaseName(); // "app-yyyy-MM-dd"
        QString dateStr = baseName.mid(4);        // "yyyy-MM-dd"
        QDate fileDate = QDate::fromString(dateStr, "yyyy-MM-dd");
        if (fileDate.isValid() && fileDate < cutoff) {
            QFile::remove(fi.filePath());
        }
    }
}
