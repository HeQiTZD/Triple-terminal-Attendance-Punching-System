#pragma once

#include <QJsonObject>
#include <QObject>
#include <QTime>

class AttendanceConfigParser : public QObject {
    Q_OBJECT

public:
    static AttendanceConfigParser* instance();

    bool parseAttendanceConfig(const QJsonObject &json);
    QJsonObject exportCurrentConfig() const;

signals:
    void configParsed();
    void configApplyFailed(const QString &reason);

private:
    explicit AttendanceConfigParser(QObject *parent = nullptr);

    QJsonObject attendanceObject(const QJsonObject &json) const;
    QTime parseTime(const QString &timeStr, const QTime &fallback) const;
    QString formatTime(const QTime &time) const;
};
