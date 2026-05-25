#include "AttendanceReporter.h"

#include <QDebug>

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
        qWarning() << "[打卡流程] 上报失败: employeeId 为空";
        return {};
    }

    qDebug() << "[打卡流程] 提交打卡上报"
             << "employeeId=" << employeeId
             << "status=" << status
             << "checkTime=" << checkTime.toString("yyyy-MM-dd HH:mm:ss");

    const QString msgId = Networkclient::instance()->uploadAttendance(employeeId, status, checkTime);

    qDebug() << "[打卡流程] 打卡已入队"
             << "msgId=" << msgId
             << "待发送数=" << pendingCount();

    emit pendingCountChanged(pendingCount());
    return msgId;
}

QString AttendanceReporter::reportWithPhoto(const QString &employeeId,
                                             const QString &status,
                                             const QByteArray &photoJpeg,
                                             const QDateTime &checkTime)
{
    if (employeeId.isEmpty()) {
        qWarning() << "[打卡流程] 上报失败: employeeId 为空";
        return {};
    }
    if (photoJpeg.isEmpty()) {
        qWarning() << "[打卡流程] photoJpeg 为空, 回退到无照片上报";
        return report(employeeId, status, checkTime);
    }

    qDebug() << "[打卡流程] 提交打卡上报（带照片）"
             << "employeeId=" << employeeId
             << "status=" << status
             << "photoSize=" << photoJpeg.size() << "bytes";

    const QString msgId = Networkclient::instance()->uploadAttendanceWithPhoto(
        employeeId, status, photoJpeg, checkTime);

    qDebug() << "[打卡流程] 打卡已入队（带照片）"
             << "msgId=" << msgId
             << "待发送数=" << pendingCount();

    emit pendingCountChanged(pendingCount());
    return msgId;
}

void AttendanceReporter::retryAll()
{
    qDebug() << "AttendanceReporter: 触发全部重试";
    Networkclient::instance()->retryOutbox();
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
    if (success) {
        qDebug() << "[打卡流程] ✓ 上报成功"
                 << "employeeId=" << employeeId;
    } else {
        qWarning() << "[打卡流程] ✗ 上报失败"
                   << "employeeId=" << employeeId
                   << "message=" << message;
    }

    emit reportCompleted(employeeId, success, message);
    emit pendingCountChanged(pendingCount());
}

void AttendanceReporter::onConnectionStateChanged(bool isOnline)
{
    bool wasOnline = m_isOnline;
    m_isOnline = isOnline;

    if (isOnline && !wasOnline) {
        qDebug() << "AttendanceReporter: 网络恢复";
        emit pendingCountChanged(pendingCount());
    }
}
