#include "attendanceanalyzer.h"

#include "../Services/dataservice.h"
#include "../Models/Person.h"
#include "../Models/attendancerecord.h"

AttendanceAnalyzer::AttendanceAnalyzer(DataService *dataService, QObject *parent)
    :QObject(parent),m_dataService(dataService)
{
    Q_ASSERT(m_dataService);
}

QJsonArray AttendanceAnalyzer::dailySummary(const QDate &start, const QDate &end, const QString &department, const QString &employeeId) const
{
    if(!m_dataService->isConnected()) return{};

    // 1) 准备人员映射：employeeId -> Person*
    //    DataManager 不再为返回对象设置 parent，需在函数尾部统一释放。
    const auto personObjs = m_dataService->getAllPerson();
    QHash<QString, Person*> personByEid;
    for(QObject* obj : personObjs){
        auto* p = qobject_cast<Person*>(obj);
        if(!p) continue;
        personByEid.insert(p->employeeId(), p);
    }

    // 2) 拉取明细记录（按日期区间）
    const QDateTime startDt(start,QTime(0,0,0));
    const QDateTime endDt(end,QTime(23,59,59));
    const auto records = m_dataService->selectAttendanceRecord(
        QString(), startDt, endDt, QString(), QString(), QString());

    // 3) 聚合：按 (date, employeeId) 记录最早/最晚打卡
    struct DayAgg
    {
        QDate date;
        QString employeeId;
        QDateTime first;
        QDateTime last;
        bool has = false;
    };
    QHash<QString,DayAgg> agg;

    for(QObject* obj : records){
        auto* r = qobject_cast<AttendanceRecord*>(obj);
        if (!r) continue;

        const QString eid = r->employeeId();
        const QDate d = r->checkTime().date();
        const QString key = d.toString(Qt::ISODate)+"#"+eid;

        auto it = agg.find(key);
        if(it == agg.end()){
            DayAgg a;
            a.date = d;
            a.employeeId = eid;
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
    QList<Person*> targetPersons;
    targetPersons.reserve(personByEid.size());
    for (auto it = personByEid.constBegin(); it != personByEid.constEnd(); ++it) {
        Person* p = it.value();
        if (!p) continue;
        if(!employeeId.isEmpty() && p->employeeId() != employeeId) continue;
        if(!department.isEmpty() && p->department() != department) continue;
        targetPersons.append(p);
    }

    QJsonArray out;
    const QTime lateLine = m_startWork.addSecs(m_lateGraceMin * 60);
    const QTime overtimeLine = m_endWork.addSecs(m_overtimeMin * 60);

    for(QDate d = start; d<=end; d = d.addDays(1)){
        if(!isWorkday(d)) continue;

        int total = targetPersons.size();
        int present = 0, late = 0, absent = 0, overtime = 0;

        for (Person* p : targetPersons) {
            if (!p) continue;
            const QString key = d.toString(Qt::ISODate) + "#" + p->employeeId();
            const auto it = agg.find(key);
            if (it == agg.end() || !it->has) { absent++; continue; }

            present++;
            if (it->first.time() > lateLine) late++;
            if (it->last.time() >= overtimeLine) overtime++;
        }

        QJsonObject row;
        row["date"] = d.toString(Qt::ISODate);
        row["total"] = total;
        row["present"] = present;
        row["late"] = late;
        row["absent"] = absent;
        row["overtime"] = overtime;
        out.append(row);
    }

    qDeleteAll(records);
    qDeleteAll(personObjs);
    return out;
}

QJsonArray AttendanceAnalyzer::personSummary(const QDate &start, const QDate &end, const QString &department) const
{
    if(!m_dataService->isConnected()) return{};

    //人员列表（DataManager 不再为返回对象设置 parent，需在函数尾部统一释放）
    const auto personObjs = m_dataService->getAllPerson();
    QList<Person*> persons;
    for(QObject* obj : personObjs){
        auto* p = qobject_cast<Person*>(obj);
        if(!p) continue;
        if(!department.isEmpty() && p->department() != department) continue;//筛选指定部门的员工
        persons.append(p);//添加到人员列表（部门的所有员工）
    }

    //获取指定范围内的打卡记录
    const auto records = m_dataService->selectAttendanceRecord(
        QString(), QDateTime(start, QTime(0, 0, 0)), QDateTime(end, QTime(23, 59, 59)),
        QString(), QString(), QString());

    //(date, employeeId) 最早/最晚
    QHash<QString,QPair<QDateTime,QDateTime>> dayFirstLast;// key date#eid -> (first,last) QPair<QDateTime, QDateTime>，存储这一天的第一次打卡时间和最后一次打卡时间。
    for (QObject* obj : records) {
        auto* r = qobject_cast<AttendanceRecord*>(obj);
        if (!r) continue;
        const QString key = r->checkTime().date().toString(Qt::ISODate) + "#" + r->employeeId();//保证了每个人每一天的唯一性。
        auto it = dayFirstLast.find(key);//查找该员工该天是否已有记录
        if (it == dayFirstLast.end()) {
            //第一次遇到 (it == ...end())：插入新记录，第一次和最后一次时间都设为当前记录时间
            dayFirstLast.insert(key, { r->checkTime(),r->checkTime() });
        }else {
            //已存在：比较并更新。qMin 更新最早时间（签到），qMax 更新最晚时间（签退）。
            it->first = qMin(it->first, r->checkTime());
            it->second = qMax(it->second, r->checkTime());
        }
    }

    //输出按人汇总
    QJsonArray out;
    const QTime lateLine = m_startWork.addSecs(m_lateGraceMin * 60);
    const QTime overtimeLine = m_endWork.addSecs(m_overtimeMin * 60);

    for (Person* p : persons) {
        int present = 0, late = 0, absent = 0, overtime = 0;

        for (QDate d = start; d <= end; d = d.addDays(1)) {
            if (!isWorkday(d)) continue;

            const QString key = d.toString(Qt::ISODate) + "#" + p->employeeId();
            const auto it = dayFirstLast.find(key);
            if (it == dayFirstLast.end()) { absent++; continue; }

            present++;
            if (it->first.time() > lateLine) late++;
            if (it->second.time() >= overtimeLine) overtime++;
        }

        QJsonObject row;
        row["employeeId"] = p->employeeId();
        row["name"] = p->name();
        row["department"] = p->department();
        row["present"] = present;
        row["late"] = late;
        row["absent"] = absent;
        row["overtime"] = overtime;
        out.append(row);
    }

    qDeleteAll(records);
    qDeleteAll(personObjs);
    return out;
}

QJsonArray AttendanceAnalyzer::departmentSummary(const QDate &start, const QDate &end) const
{
    if (!m_dataService->isConnected()) return{};

    //先按部门分组人员
    const auto personObjs = m_dataService->getAllPerson();
    QHash<QString, QList<Person*>> personByDept;
    for (auto obj : personObjs) {
        auto* p = qobject_cast<Person*>(obj);
        if (!p) continue;
        personByDept[p->department()].append(p);
    }

    // 对每个部门调用 personSummary 的聚合逻辑（这里直接复用 daily/person 的做法更快实现）
    QJsonArray out;
    for (auto it = personByDept.begin(); it != personByDept.end(); it++) {
        const QString dept = it.key();
        const QJsonArray persons = personSummary(start, end, dept);

        int present = 0, late = 0, absent = 0, overtime = 0;
        for (const auto& v : persons) {
            const QJsonObject o = v.toObject();
            present += o["present"].toInt();
            late += o["late"].toInt();
            absent += o["absent"].toInt();
            overtime += o["overtime"].toInt();
        }

        QJsonObject row;
        row["department"] = dept;
        row["present"] = present;
        row["late"] = late;
        row["absent"] = absent;
        row["overtime"] = overtime;
        out.append(row);
    }

    qDeleteAll(personObjs);
    return out;
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
