#include "AttendanceConfigParser.h"

#include "AttendanceConfigValidator.h"
#include "../Config/configmanager.h"

#include <QJsonObject>

AttendanceConfigParser* AttendanceConfigParser::instance()
{
    static AttendanceConfigParser parser;
    return &parser;
}

AttendanceConfigParser::AttendanceConfigParser(QObject *parent)
    : QObject(parent)
{
}

bool AttendanceConfigParser::parseAttendanceConfig(const QJsonObject &json)
{
    const AttendanceValidationResult validation = AttendanceConfigValidator::instance()->validate(json);
    if (!validation.isValid) {
        emit configApplyFailed(validation.errorMessage);
        return false;
    }

    const QJsonObject attendance = attendanceObject(json);
    ConfigManager *config = ConfigManager::instance();

    config->setWorkStartTime(parseTime(attendance.value(QStringLiteral("workStartTime")).toString(), config->getWorkStartTime()));
    config->setWorkEndTime(parseTime(attendance.value(QStringLiteral("workEndTime")).toString(), config->getWorkEndTime()));
    config->setCheckInStartOffset(attendance.value(QStringLiteral("checkInStartOffset")).toInt(config->getCheckInStartOffset()));
    config->setLateAllowance(attendance.value(QStringLiteral("lateAllowance")).toInt(config->getLateAllowance()));
    config->setEarlyLeaveAllowance(attendance.value(QStringLiteral("earlyLeaveAllowance")).toInt(config->getEarlyLeaveAllowance()));
    config->setCheckOutEndOffset(attendance.value(QStringLiteral("checkOutEndOffset")).toInt(config->getCheckOutEndOffset()));
    config->setFlexibleWorkEnabled(attendance.value(QStringLiteral("flexibleWorkEnabled")).toBool(config->isFlexibleWorkEnabled()));
    config->setFlexibleRange(attendance.value(QStringLiteral("flexibleRange")).toInt(config->getFlexibleRange()));
    config->setCoreWorkStart(parseTime(attendance.value(QStringLiteral("coreWorkStart")).toString(), config->getCoreWorkStart()));
    config->setCoreWorkEnd(parseTime(attendance.value(QStringLiteral("coreWorkEnd")).toString(), config->getCoreWorkEnd()));
    config->setLunchBreakEnabled(attendance.value(QStringLiteral("lunchBreakEnabled")).toBool(config->isLunchBreakEnabled()));
    config->setLunchBreakStart(parseTime(attendance.value(QStringLiteral("lunchBreakStart")).toString(), config->getLunchBreakStart()));
    config->setLunchBreakEnd(parseTime(attendance.value(QStringLiteral("lunchBreakEnd")).toString(), config->getLunchBreakEnd()));
    config->setMinCheckInterval(attendance.value(QStringLiteral("minCheckInterval")).toInt(config->getMinCheckInterval()));
    config->setAllowCrossDay(attendance.value(QStringLiteral("allowCrossDay")).toBool(config->isCrossDayAllowed()));
    config->setMaxWorkHours(attendance.value(QStringLiteral("maxWorkHours")).toInt(config->getMaxWorkHours()));
    config->saveConfig();

    emit configParsed();
    return true;
}

QJsonObject AttendanceConfigParser::exportCurrentConfig() const
{
    ConfigManager *config = ConfigManager::instance();
    QJsonObject attendance;
    attendance[QStringLiteral("workStartTime")] = formatTime(config->getWorkStartTime());
    attendance[QStringLiteral("workEndTime")] = formatTime(config->getWorkEndTime());
    attendance[QStringLiteral("checkInStartOffset")] = config->getCheckInStartOffset();
    attendance[QStringLiteral("lateAllowance")] = config->getLateAllowance();
    attendance[QStringLiteral("earlyLeaveAllowance")] = config->getEarlyLeaveAllowance();
    attendance[QStringLiteral("checkOutEndOffset")] = config->getCheckOutEndOffset();
    attendance[QStringLiteral("flexibleWorkEnabled")] = config->isFlexibleWorkEnabled();
    attendance[QStringLiteral("flexibleRange")] = config->getFlexibleRange();
    attendance[QStringLiteral("coreWorkStart")] = formatTime(config->getCoreWorkStart());
    attendance[QStringLiteral("coreWorkEnd")] = formatTime(config->getCoreWorkEnd());
    attendance[QStringLiteral("lunchBreakEnabled")] = config->isLunchBreakEnabled();
    attendance[QStringLiteral("lunchBreakStart")] = formatTime(config->getLunchBreakStart());
    attendance[QStringLiteral("lunchBreakEnd")] = formatTime(config->getLunchBreakEnd());
    attendance[QStringLiteral("minCheckInterval")] = config->getMinCheckInterval();
    attendance[QStringLiteral("allowCrossDay")] = config->isCrossDayAllowed();
    attendance[QStringLiteral("maxWorkHours")] = config->getMaxWorkHours();

    QJsonObject rootConfig;
    rootConfig[QStringLiteral("attendance")] = attendance;

    QJsonObject root;
    root[QStringLiteral("deviceId")] = config->getDeviceId();
    root[QStringLiteral("generatedAt")] = QString();
    root[QStringLiteral("config")] = rootConfig;
    root[QStringLiteral("signature")] = QString();
    return root;
}

QJsonObject AttendanceConfigParser::attendanceObject(const QJsonObject &json) const
{
    if (json.contains(QStringLiteral("attendance")) && json.value(QStringLiteral("attendance")).isObject()) {
        return json.value(QStringLiteral("attendance")).toObject();
    }
    const QJsonObject rootConfig = json.value(QStringLiteral("config")).toObject();
    if (rootConfig.contains(QStringLiteral("attendance")) && rootConfig.value(QStringLiteral("attendance")).isObject()) {
        return rootConfig.value(QStringLiteral("attendance")).toObject();
    }
    if (json.contains(QStringLiteral("config"))) {
        return {};
    }
    return json;
}

QTime AttendanceConfigParser::parseTime(const QString &timeStr, const QTime &fallback) const
{
    const QTime parsed = QTime::fromString(timeStr, QStringLiteral("HH:mm"));
    return parsed.isValid() ? parsed : fallback;
}

QString AttendanceConfigParser::formatTime(const QTime &time) const
{
    return time.isValid() ? time.toString(QStringLiteral("HH:mm")) : QString();
}
