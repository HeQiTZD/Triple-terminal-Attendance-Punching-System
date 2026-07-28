#pragma once

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QTime>

struct AttendanceCheckResult {
    QString status;
    bool isValid = false;
    bool isDuplicate = false;
    QString message;
    int lateMinutes = 0;
    int earlyMinutes = 0;
    QDate workDate;
};

class AttendanceRuleEngine : public QObject {
    Q_OBJECT

public:
    static AttendanceRuleEngine* instance();

    AttendanceCheckResult evaluate(const QDateTime &checkTime) const;
    AttendanceCheckResult evaluateWithEmployee(const QString &employeeId, const QDateTime &checkTime) const;

    bool isInCheckInRange(const QTime &time) const;
    bool isInCheckOutRange(const QTime &time) const;
    bool isDuplicateCheck(const QString &employeeId, const QDateTime &checkTime) const;

private:
    explicit AttendanceRuleEngine(QObject *parent = nullptr);

    AttendanceCheckResult evaluateStandard(const QDateTime &checkTime) const;
    AttendanceCheckResult evaluateFlexible(const QDateTime &checkTime) const;
    bool isTimeInRange(const QTime &time, const QTime &start, const QTime &end) const;
    int minutesLate(const QTime &checkTime, const QTime &workStart) const;
    int minutesEarly(const QTime &checkTime, const QTime &workEnd) const;
};
