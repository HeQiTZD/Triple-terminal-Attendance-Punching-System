#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>

struct AttendanceValidationResult {
    bool isValid = false;
    QString errorMessage;
    QStringList warnings;
};

class AttendanceConfigValidator : public QObject {
    Q_OBJECT

public:
    static AttendanceConfigValidator* instance();

    AttendanceValidationResult validate(const QJsonObject &config) const;
    AttendanceValidationResult validateSignature(const QJsonObject &config) const;

private:
    explicit AttendanceConfigValidator(QObject *parent = nullptr);

    bool validateTimeFormat(const QString &time) const;
    bool validateRange(int value, int min, int max) const;
    QJsonObject attendanceObject(const QJsonObject &config) const;
};
