#include "AttendanceReporter.h"

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
        return {};
    }

    const QString msgId = NetworkClient::instance()->uploadAttendance(employeeId, status, checkTime);

    emit pendingCountChanged(pendingCount());
    return msgId;
}

QString AttendanceReporter::reportWithPhoto(const QString &employeeId,
                                             const QString &status,
                                             const QByteArray &photoJpeg,
                                             const QDateTime &checkTime)
{
    if (employeeId.isEmpty()) {
        return {};
    }
    if (photoJpeg.isEmpty()) {
        return report(employeeId, status, checkTime);
    }

    const QString msgId = NetworkClient::instance()->uploadAttendanceWithPhoto(
        employeeId, status, photoJpeg, checkTime);

    emit pendingCountChanged(pendingCount());
    return msgId;
}

void AttendanceReporter::retryAll()
{
    NetworkClient::instance()->retryOutbox();
    emit pendingCountChanged(pendingCount());
}

int AttendanceReporter::pendingCount() const
{
    return LocalStorage::instance()->outbox().pendingCount();
}

bool AttendanceReporter::canReport() const
{
    return m_isOnline && NetworkClient::instance()->isAuthenticated();
}

void AttendanceReporter::onReportResult(const QString &employeeId, bool success,
                                         const QString &message)
{
    emit reportCompleted(employeeId, success, message);
    emit pendingCountChanged(pendingCount());
}

void AttendanceReporter::onConnectionStateChanged(bool isOnline)
{
    bool wasOnline = m_isOnline;
    m_isOnline = isOnline;

    if (isOnline && !wasOnline) {
        emit pendingCountChanged(pendingCount());
    }
}
