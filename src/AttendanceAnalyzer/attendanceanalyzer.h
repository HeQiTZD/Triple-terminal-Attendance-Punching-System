#ifndef ATTENDANCEANALYZER_H
#define ATTENDANCEANALYZER_H

#include <QObject>
#include <QDate>
#include <QJsonArray>
#include <QJsonObject>
#include <QTime>
#include <QDateTime>

class DataManager;
class Person;
class DataService;

class AttendanceAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit  AttendanceAnalyzer(DataService* dataService,QObject* parent = nullptr);

    // 按天统计：输出每天的汇总（可按部门过滤，可按员工过滤）
    Q_INVOKABLE QJsonArray dailySummary(const QDate& start,const QDate& end,
                                        const QString& department = QString(),
                                        const QString& employeeId = QString()) const;

    // 按人员统计：输出人员维度汇总（可按部门过滤）
    Q_INVOKABLE QJsonArray personSummary(const QDate& start, const QDate& end,
                                         const QString& department = QString()) const;

    // 按部门统计：输出部门维度汇总
    Q_INVOKABLE QJsonArray departmentSummary(const QDate& start, const QDate& end) const;

    // 规则配置（先给默认值，后续可改成读取配置）
    void setWorkTime(const QTime& startWork, const QTime& endWork);
    void setLateGraceMinutes(int minutes);
    void setOvertimeThresholdMinutes(int minutes);

private:
    DataService* m_dataService;
    QTime m_startWork{9, 0};
    QTime m_endWork{18, 0};
    int m_lateGraceMin{5};
    int m_overtimeMin{60};
    bool isWorkday(const QDate& d) const;
};

#endif // ATTENDANCEANALYZER_H
