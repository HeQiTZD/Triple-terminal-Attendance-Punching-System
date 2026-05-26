#include "AttendanceRuleEngine.h"

#include "../Config/configmanager.h"
#include "../LocalStorage/localstorage.h"

AttendanceRuleEngine* AttendanceRuleEngine::instance()
{
    static AttendanceRuleEngine engine;
    return &engine;
}

AttendanceRuleEngine::AttendanceRuleEngine(QObject *parent)
    : QObject(parent)
{
}

AttendanceCheckResult AttendanceRuleEngine::evaluate(const QDateTime &checkTime) const
{
    AttendanceCheckResult result;
    if (!checkTime.isValid()) {
        result.message = QStringLiteral("打卡时间无效");
        return result;
    }

    ConfigManager *config = ConfigManager::instance();
    if (config->isFlexibleWorkEnabled()) {
        return evaluateFlexible(checkTime);
    }
    return evaluateStandard(checkTime);
}

AttendanceCheckResult AttendanceRuleEngine::evaluateWithEmployee(const QString &employeeId,
                                                                  const QDateTime &checkTime) const
{
    const bool isDuplicate = isDuplicateCheck(employeeId, checkTime);

    if (isDuplicate) {
        AttendanceCheckResult result;
        result.status = QStringLiteral("duplicate");
        result.isDuplicate = true;
        result.message = QStringLiteral("重复打卡");
        result.workDate = checkTime.date();
        return result;
    }

    return evaluate(checkTime);
}

bool AttendanceRuleEngine::isInCheckInRange(const QTime &time) const
{
    ConfigManager *config = ConfigManager::instance();
    const QTime checkInStart = config->getWorkStartTime().addSecs(-qMax(0, config->getCheckInStartOffset()) * 60);
    const QTime checkInEnd = config->getWorkStartTime().addSecs(qMax(0, config->getLateAllowance()) * 60);
    return isTimeInRange(time, checkInStart, checkInEnd);
}

bool AttendanceRuleEngine::isInCheckOutRange(const QTime &time) const
{
    ConfigManager *config = ConfigManager::instance();
    const QTime checkOutStart = config->getWorkEndTime().addSecs(-qMax(0, config->getEarlyLeaveAllowance()) * 60);
    const QTime checkOutEnd = config->getWorkEndTime().addSecs(qMax(0, config->getCheckOutEndOffset()) * 60);
    return isTimeInRange(time, checkOutStart, checkOutEnd);
}

bool AttendanceRuleEngine::isDuplicateCheck(const QString &employeeId, const QDateTime &checkTime) const
{
    if (employeeId.isEmpty() || !checkTime.isValid()) {
        return false;
    }

    const OutboxRecord lastRecord = LocalStorage::instance()->outbox().findLatestByEmployeeId(employeeId);
    if (lastRecord.id <= 0 || lastRecord.checkTime.isEmpty()) {
        return false;
    }

    QDateTime lastCheck = QDateTime::fromString(lastRecord.checkTime, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    if (!lastCheck.isValid()) {
        lastCheck = QDateTime::fromString(lastRecord.checkTime, QStringLiteral("yyyy-MM-dd hh:mm:ss"));
    }
    if (!lastCheck.isValid()) {
        return false;
    }

    const int minInterval = qMax(0, ConfigManager::instance()->getMinCheckInterval());
    return lastCheck.secsTo(checkTime) >= 0 && lastCheck.secsTo(checkTime) < minInterval;
}

AttendanceCheckResult AttendanceRuleEngine::evaluateStandard(const QDateTime &checkTime) const
{
    AttendanceCheckResult result;
    result.workDate = checkTime.date();

    ConfigManager *config = ConfigManager::instance();
    const QTime workStart = config->getWorkStartTime();
    const QTime workEnd = config->getWorkEndTime();
    if (!workStart.isValid() || !workEnd.isValid()
        || (!config->isCrossDayAllowed() && workStart >= workEnd)) {
        result.message = QStringLiteral("考勤规则配置无效");
        return result;
    }

    const int lateAllowance = qMax(0, config->getLateAllowance());
    const int earlyLeaveAllowance = qMax(0, config->getEarlyLeaveAllowance());
    const QTime timeOfDay = checkTime.time();
    const QTime checkInStart = workStart.addSecs(-qMax(0, config->getCheckInStartOffset()) * 60);
    const QTime checkInDeadline = workStart.addSecs(lateAllowance * 60);
    const QTime checkOutStart = workEnd.addSecs(-earlyLeaveAllowance * 60);
    const QTime checkOutEnd = workEnd.addSecs(qMax(0, config->getCheckOutEndOffset()) * 60);
    const int workSecs = workStart.secsTo(workEnd) > 0
                             ? workStart.secsTo(workEnd)
                             : workStart.secsTo(workEnd) + 24 * 60 * 60;
    const QTime workMiddle = workStart.addSecs(workSecs / 2);

    const bool inFullRange = isTimeInRange(timeOfDay, checkInStart, checkOutEnd);
    if (!inFullRange) {
        result.message = QStringLiteral("不在有效打卡时间范围内");
        return result;
    }

    result.isValid = true;
    if (isTimeInRange(timeOfDay, checkInStart, checkInDeadline)) {
        result.status = QStringLiteral("normal");
        result.message = QStringLiteral("上班打卡成功");
        return result;
    }

    if (isTimeInRange(timeOfDay, checkInDeadline.addSecs(1), workMiddle)) {
        result.status = QStringLiteral("late");
        result.lateMinutes = minutesLate(timeOfDay, workStart);
        result.message = QStringLiteral("迟到 %1 分钟").arg(result.lateMinutes);
        return result;
    }

    if (isTimeInRange(timeOfDay, workMiddle.addSecs(1), checkOutStart.addSecs(-1))) {
        result.status = QStringLiteral("early");
        result.earlyMinutes = minutesEarly(timeOfDay, workEnd);
        result.message = QStringLiteral("早退 %1 分钟").arg(result.earlyMinutes);
        return result;
    }

    result.status = QStringLiteral("normal");
    result.message = QStringLiteral("下班打卡成功");
    return result;
}

AttendanceCheckResult AttendanceRuleEngine::evaluateFlexible(const QDateTime &checkTime) const
{
    AttendanceCheckResult result;
    result.workDate = checkTime.date();

    ConfigManager *config = ConfigManager::instance();
    const QTime timeOfDay = checkTime.time();
    const QTime flexCheckInStart = config->getWorkStartTime().addSecs(-qMax(0, config->getFlexibleRange()) * 60);
    const QTime flexCheckInEnd = config->getWorkStartTime().addSecs(qMax(0, config->getFlexibleRange()) * 60);
    const QTime flexCheckOutStart = config->getWorkEndTime().addSecs(-qMax(0, config->getFlexibleRange()) * 60);
    const QTime flexCheckOutEnd = config->getWorkEndTime().addSecs(qMax(0, config->getFlexibleRange()) * 60);

    if (isTimeInRange(timeOfDay, flexCheckInStart, flexCheckInEnd)) {
        result.status = QStringLiteral("normal");
        result.isValid = true;
        result.message = QStringLiteral("弹性上班打卡成功");
        return result;
    }

    if (isTimeInRange(timeOfDay, flexCheckOutStart, flexCheckOutEnd)) {
        result.status = QStringLiteral("normal");
        result.isValid = true;
        result.message = QStringLiteral("弹性下班打卡成功");
        return result;
    }

    const QTime coreStart = config->getCoreWorkStart();
    if (timeOfDay > flexCheckInEnd && timeOfDay < coreStart) {
        result.status = QStringLiteral("late");
        result.isValid = true;
        result.lateMinutes = qMax(0, flexCheckInEnd.secsTo(timeOfDay) / 60);
        result.message = QStringLiteral("弹性迟到 %1 分钟").arg(result.lateMinutes);
        return result;
    }

    const QTime coreEnd = config->getCoreWorkEnd();
    if (timeOfDay > coreEnd && timeOfDay < flexCheckOutStart) {
        result.status = QStringLiteral("early");
        result.isValid = true;
        result.earlyMinutes = qMax(0, timeOfDay.secsTo(flexCheckOutStart) / 60);
        result.message = QStringLiteral("弹性早退 %1 分钟").arg(result.earlyMinutes);
        return result;
    }

    result.message = QStringLiteral("不在弹性工时打卡范围内");
    return result;
}

bool AttendanceRuleEngine::isTimeInRange(const QTime &time, const QTime &start, const QTime &end) const
{
    if (!time.isValid() || !start.isValid() || !end.isValid()) {
        return false;
    }
    if (start <= end) {
        return time >= start && time <= end;
    }
    return time >= start || time <= end;
}

int AttendanceRuleEngine::minutesLate(const QTime &checkTime, const QTime &workStart) const
{
    int seconds = workStart.secsTo(checkTime);
    if (seconds < 0) {
        seconds += 24 * 60 * 60;
    }
    return qMax(0, seconds / 60);
}

int AttendanceRuleEngine::minutesEarly(const QTime &checkTime, const QTime &workEnd) const
{
    int seconds = checkTime.secsTo(workEnd);
    if (seconds < 0) {
        seconds += 24 * 60 * 60;
    }
    return qMax(0, seconds / 60);
}
