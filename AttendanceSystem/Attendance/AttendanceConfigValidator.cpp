#include "AttendanceConfigValidator.h"

#include <QTime>

AttendanceConfigValidator* AttendanceConfigValidator::instance()
{
    static AttendanceConfigValidator validator;
    return &validator;
}

AttendanceConfigValidator::AttendanceConfigValidator(QObject *parent)
    : QObject(parent)
{
}

AttendanceValidationResult AttendanceConfigValidator::validate(const QJsonObject &config) const
{
    AttendanceValidationResult result;
    const QJsonObject attendance = attendanceObject(config);
    if (attendance.isEmpty()) {
        result.errorMessage = QStringLiteral("缺少 config.attendance 配置");
        return result;
    }

    const QStringList timeKeys = {
        QStringLiteral("workStartTime"),
        QStringLiteral("workEndTime"),
        QStringLiteral("coreWorkStart"),
        QStringLiteral("coreWorkEnd"),
        QStringLiteral("lunchBreakStart"),
        QStringLiteral("lunchBreakEnd"),
    };
    for (const QString &key : timeKeys) {
        if (attendance.contains(key) && !validateTimeFormat(attendance.value(key).toString())) {
            result.errorMessage = QStringLiteral("%1 时间格式错误，应为 HH:mm").arg(key);
            return result;
        }
    }

    auto requireRange = [&](const QString &key, int min, int max) {
        if (!attendance.contains(key)) {
            return true;
        }
        const int value = attendance.value(key).toInt(min - 1);
        if (!validateRange(value, min, max)) {
            result.errorMessage = QStringLiteral("%1 超出范围 [%2, %3]").arg(key).arg(min).arg(max);
            return false;
        }
        return true;
    };

    if (!requireRange(QStringLiteral("checkInStartOffset"), 0, 1440)
        || !requireRange(QStringLiteral("lateAllowance"), 0, 1440)
        || !requireRange(QStringLiteral("earlyLeaveAllowance"), 0, 1440)
        || !requireRange(QStringLiteral("checkOutEndOffset"), 0, 1440)
        || !requireRange(QStringLiteral("flexibleRange"), 0, 720)
        || !requireRange(QStringLiteral("minCheckInterval"), 0, 86400)
        || !requireRange(QStringLiteral("maxWorkHours"), 1, 24)) {
        return result;
    }

    const QTime workStart = QTime::fromString(attendance.value(QStringLiteral("workStartTime")).toString(QStringLiteral("09:00")), QStringLiteral("HH:mm"));
    const QTime workEnd = QTime::fromString(attendance.value(QStringLiteral("workEndTime")).toString(QStringLiteral("18:00")), QStringLiteral("HH:mm"));
    const bool allowCrossDay = attendance.value(QStringLiteral("allowCrossDay")).toBool(false);
    if (!allowCrossDay && workStart >= workEnd) {
        result.errorMessage = QStringLiteral("workStartTime 必须早于 workEndTime");
        return result;
    }

    const QTime coreStart = QTime::fromString(attendance.value(QStringLiteral("coreWorkStart")).toString(QStringLiteral("10:00")), QStringLiteral("HH:mm"));
    const QTime coreEnd = QTime::fromString(attendance.value(QStringLiteral("coreWorkEnd")).toString(QStringLiteral("16:00")), QStringLiteral("HH:mm"));
    if (coreStart >= coreEnd) {
        result.errorMessage = QStringLiteral("coreWorkStart 必须早于 coreWorkEnd");
        return result;
    }

    const QTime lunchStart = QTime::fromString(attendance.value(QStringLiteral("lunchBreakStart")).toString(QStringLiteral("12:00")), QStringLiteral("HH:mm"));
    const QTime lunchEnd = QTime::fromString(attendance.value(QStringLiteral("lunchBreakEnd")).toString(QStringLiteral("13:00")), QStringLiteral("HH:mm"));
    if (attendance.value(QStringLiteral("lunchBreakEnabled")).toBool(true) && lunchStart >= lunchEnd) {
        result.errorMessage = QStringLiteral("lunchBreakStart 必须早于 lunchBreakEnd");
        return result;
    }

    result = validateSignature(config);
    if (!result.isValid) {
        return result;
    }

    result.isValid = true;
    return result;
}

AttendanceValidationResult AttendanceConfigValidator::validateSignature(const QJsonObject &config) const
{
    AttendanceValidationResult result;
    result.isValid = true;
    if (!config.value(QStringLiteral("signature")).toString().isEmpty()) {
        result.warnings.append(QStringLiteral("当前设备端未配置 RSA 公钥，已跳过签名验签"));
    }
    return result;
}

bool AttendanceConfigValidator::validateTimeFormat(const QString &time) const
{
    return QTime::fromString(time, QStringLiteral("HH:mm")).isValid();
}

bool AttendanceConfigValidator::validateRange(int value, int min, int max) const
{
    return value >= min && value <= max;
}

QJsonObject AttendanceConfigValidator::attendanceObject(const QJsonObject &config) const
{
    if (config.contains(QStringLiteral("attendance")) && config.value(QStringLiteral("attendance")).isObject()) {
        return config.value(QStringLiteral("attendance")).toObject();
    }
    const QJsonObject rootConfig = config.value(QStringLiteral("config")).toObject();
    if (rootConfig.contains(QStringLiteral("attendance")) && rootConfig.value(QStringLiteral("attendance")).isObject()) {
        return rootConfig.value(QStringLiteral("attendance")).toObject();
    }
    if (config.contains(QStringLiteral("config"))) {
        return {};
    }
    return config;
}
