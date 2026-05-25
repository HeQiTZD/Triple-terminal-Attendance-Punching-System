#include "AttendanceReporter.h"

#include <QDebug>
#include <QTime>

#include "../LocalStorage/localstorage.h"
#include "../NetworkClient/networkclient.h"

AttendanceReporter::AttendanceReporter(QObject *parent)
    : QObject(parent)
{
}

QString AttendanceReporter::report(const QString &employeeId,
                                    const QString &status,
                                    const QDateTime &checkTime)
{
    if (employeeId.isEmpty()) {
        qWarning() << "[打卡流程] employeeId 为空";
        return {};
    }

    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[开始] 打卡记录上传"
             << "employeeId=" << employeeId;

    const QString msgId = Networkclient::instance()->uploadAttendance(employeeId, status, checkTime);

    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[完成] 打卡记录上传"
             << "employeeId=" << employeeId
             << "msgId=" << msgId;

    emit pendingCountChanged(pendingCount());
    return msgId;
}

QString AttendanceReporter::reportWithPhoto(const QString &employeeId,
                                             const QString &status,
                                             const QByteArray &photoJpeg,
                                             const QDateTime &checkTime)
{
    if (employeeId.isEmpty()) {
        qWarning() << "[打卡流程] employeeId 为空";
        return {};
    }
    if (photoJpeg.isEmpty()) {
        qWarning() << "[打卡流程] photoJpeg 为空, 回退到无照片上报";
        return report(employeeId, status, checkTime);
    }

    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[开始] 打卡记录上传（带照片）"
             << "employeeId=" << employeeId
             << "photo size=" << photoJpeg.size();

    const QString msgId = Networkclient::instance()->uploadAttendanceWithPhoto(
        employeeId, status, photoJpeg, checkTime);

    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[完成] 打卡记录上传（带照片）"
             << "employeeId=" << employeeId
             << "msgId=" << msgId;

    emit pendingCountChanged(pendingCount());
    return msgId;
}

void AttendanceReporter::retryAll()
{
    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[开始] Outbox重试检查";

    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[执行] 触发全部重试";
    Networkclient::instance()->retryOutbox();

    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[完成] Outbox重试检查"
             << "待重试数量=" << pendingCount();

    emit pendingCountChanged(pendingCount());
}

int AttendanceReporter::pendingCount() const
{
    return LocalStorage::instance()->outbox().pendingCount();
}

bool AttendanceReporter::canReport() const
{
    return m_isOnline && Networkclient::instance()->isAuthenticated();
}

void AttendanceReporter::onReportResult(const QString &employeeId, bool success,
                                         const QString &message)
{
    qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "[收到] 上报结果"
             << "employeeId=" << employeeId
             << "success=" << success
             << "message=" << message;

    emit reportCompleted(employeeId, success, message);
    emit pendingCountChanged(pendingCount());
}

void AttendanceReporter::onConnectionStateChanged(bool isOnline)
{
    bool wasOnline = m_isOnline;
    m_isOnline = isOnline;

    if (isOnline && !wasOnline) {
        qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
                 << "[状态] 网络恢复";
        emit pendingCountChanged(pendingCount());
    }
}
