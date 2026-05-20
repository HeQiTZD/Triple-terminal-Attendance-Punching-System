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
        qWarning() << "AttendanceReporter: employeeId 为空";
        return {};
    }

    const QString msgId = Networkclient::instance()->uploadAttendance(employeeId, status, checkTime);

    qDebug() << "AttendanceReporter: 已提交上报, employeeId=" << employeeId
             << "status=" << status << "msgId=" << msgId;

    emit pendingCountChanged(pendingCount());
    return msgId;
}

QString AttendanceReporter::reportWithPhoto(const QString &employeeId,
                                             const QString &status,
                                             const QByteArray &photoJpeg,
                                             const QDateTime &checkTime)
{
    if (employeeId.isEmpty()) {
        qWarning() << "AttendanceReporter: employeeId 为空";
        return {};
    }
    if (photoJpeg.isEmpty()) {
        qWarning() << "AttendanceReporter: photoJpeg 为空, 回退到无照片上报";
        return report(employeeId, status, checkTime);
    }

    const QString msgId = Networkclient::instance()->uploadAttendanceWithPhoto(
        employeeId, status, photoJpeg, checkTime);

    qDebug() << "AttendanceReporter: 已提交上报（带照片）, employeeId=" << employeeId
             << "photo size=" << photoJpeg.size() << "msgId=" << msgId;

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
    qDebug() << "AttendanceReporter: 上报结果 employeeId=" << employeeId
             << "success=" << success << "message=" << message;

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
