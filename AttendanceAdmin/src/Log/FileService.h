#ifndef FILESERVICE_H
#define FILESERVICE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>

class FileService : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(FileService)
public:
    explicit FileService(QObject *parent = nullptr);

    Q_INVOKABLE QString readLogFile(const QString &date);
    Q_INVOKABLE QStringList getLogFileDates();
    Q_INVOKABLE bool exportLogs(const QString &content, const QString &filePath);

private:
    QDir m_logDir;
};

#endif // FILESERVICE_H
