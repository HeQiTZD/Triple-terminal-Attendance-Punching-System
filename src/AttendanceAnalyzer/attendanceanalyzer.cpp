#include "attendanceanalyzer.h"

AttendanceAnalyzer::AttendanceAnalyzer(DataManager *dataManager, QObject *parent)
    :QObject(parent),m_dataManager(dataManager)
{
    Q_ASSERT(m_dataManager);
}

QJsonArray AttendanceAnalyzer::dailySummary(const QDate &start, const QDate &end, const QString &department, const QString &employeeId) const
{
    if(!m_dataManager->isConnected()) return{};

    // 1) 准备人员映射：personId -> Person*
    QHash<int,Person*> personById;
    {
        const auto persons = m_dataManager->getAllPerson();
        for(const QObject* obj : persons){
            auto* p = qobject_cast<Person*>(obj);
            if(!p) continue;
            personById.insert(p->id(),p);
        }
    }

    // 2) 拉取明细记录（按日期区间）
    const QDateTime startDt(start,QTime(0,0,0));
    const QDateTime endDt(end,QTime(23,59,59));
    const auto records = m_dataManager->getAttendanceRecords(startDt,endDt);

    // 3) 聚合：按 (date, personId) 记录最早/最晚打卡
    struct DayAgg
    {
        QDate date;
        int personId;
        QDateTime first;
        QDateTime last;
        bool has = false;
    };
    QHash<QString,DayAgg> agg;

    for(QObject* obj : records){
        auto* r = qobject_cast<AttendanceRecord*>(obj);
        if (!r) continue;

        const int pid = r->personId();
        const QDate d = r->checkTime().date();
        const QString key = d.toString(Qt::ISODate)+"#"+QString::number(pid);

        auto it = agg.find(key);
        if(it == agg.end()){
            DayAgg a;
            a.date = d;
            a.personId = pid;
            a.first = r->checkTime();
            a.last = r->checkTime();
            a.has = true;
            agg.insert(key,a);
        }else{
            if(!it->has || r->checkTime() < it->first) it->first = r->checkTime();
            if(!it->has || r->checkTime() > it->last) it->last = r->checkTime();
            it->has = true;
        }
    }

    // 4) 输出：按天汇总（人数/迟到/缺勤/加班）
    // 先确定“参与统计人员集合”（按 department/employeeId 过滤）
    QList<Person*>  targetPersons;
    targetPersons.reserve(personById.size());
    for(auto* p : targetPersons){
        if(!employeeId.isEmpty() && p->employeeId() != employeeId) continue;
        if(!department.isEmpty() && p->department() != department) continue;
        targetPersons.append(p);
    }

    QJsonArray outo;
    for(QDate d = start; d<=end; d= d.addDays(1)){
        if(!isWorkday(d)) continue;

        int persont = 0,late = 0,absent = 0,overtime = 0;
        const QTime lateLine = m_startWork.addSecs(m_lateGraceMin * 60);
    }
}

QJsonArray AttendanceAnalyzer::personSummary(const QDate &start, const QDate &end, const QString &department) const
{
    if(!m_dataManager->isConnected()) return{};

    //人员列表
    QList<Person*> persons;
    {
        const auto objs = m_dataManager->getAllPerson();
        for(QObject* obj : objs){
            auto* p = qobject_cast<Person*>(obj);
            if(!p) continue;
            if(!department.isEmpty() && p->department() != department) continue;
            persons.append(p);
        }
    }

    //明细
    const auto records = m_dataManager->getAttendanceRecords(QDateTime(start,QTime(0,0,0)),QDateTime(end,QTime(23,23,23)));

    //(date, personId) 最早/最晚
    QHash<QString,QPair<QDateTime,QDateTime>> dayFirstLast;//// key date#pid -> (first,last)
}

QJsonArray AttendanceAnalyzer::departmentSummary(const QDate &start, const QDate &end) const
{

}

void AttendanceAnalyzer::setWorkTime(const QTime &startWork, const QTime &endWork)
{
    m_startWork = startWork;
    m_endWork = endWork;
}

void AttendanceAnalyzer::setLateGraceMinutes(int minutes)
{
    m_lateGraceMin = minutes;
}

void AttendanceAnalyzer::setOvertimeThresholdMinutes(int minutes)
{
    m_overtimeMin = minutes;
}

bool AttendanceAnalyzer::isWorkday(const QDate &d) const
{
    // 先按周一到周五作为工作日；后续可接节假日表/排班表
    //获取星期数：
    //Qt QDate::dayOfWeek() 规则：1=周一，2=周二…7=周日，用 const int 存储避免修改
    const int wd = d.dayOfWeek(); // 1..7 (Mon..Sun)
    return wd >= 1 && wd <= 5;
}
