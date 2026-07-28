#include "FileService.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDirIterator>
#include <QUrl>
#include <QDate>

FileService::FileService(QObject *parent)
    : QObject(parent)
{
    m_logDir = QDir(QCoreApplication::applicationDirPath() + "/logs");
}

QString FileService::readLogFile(const QString &date)
{
    QString filePath = m_logDir.filePath(QString("app-%1.log").arg(date));
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}

QStringList FileService::getLogFileDates()
{
    QStringList dates;
    QDirIterator it(m_logDir.path(), {"app-*.log"}, QDir::Files);
    while (it.hasNext()) {
        it.next();
        QFileInfo fi = it.fileInfo();
        QString baseName = fi.completeBaseName(); // "app-yyyy-MM-dd"
        QString dateStr = baseName.mid(4);        // "yyyy-MM-dd"
        if (QDate::fromString(dateStr, "yyyy-MM-dd").isValid()) {
            dates.append(dateStr);
        }
    }

    dates.sort();
    return dates;
}

bool FileService::exportLogs(const QString &content, const QString &filePath)
{
    // 处理 file:/// URL 前缀
    QString localPath = filePath;
    if (localPath.startsWith("file:///")) {
        localPath = QUrl(localPath).toLocalFile();
    } else if (localPath.startsWith("file://")) {
        localPath = QUrl(localPath).toLocalFile();
    }

    QFile file(localPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << content;
    file.close();
    return true;
}
