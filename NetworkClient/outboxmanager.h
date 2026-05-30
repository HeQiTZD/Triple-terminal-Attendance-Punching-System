#ifndef OUTBOXMANAGER_H
#define OUTBOXMANAGER_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QDateTime>

class MessageWriter;

class OutboxManager : public QObject
{
    Q_OBJECT

public:
    explicit OutboxManager(QObject *parent = nullptr);

    // ── 入队（委托 AttendanceOutboxRepository，只做业务校验）──
    QString enqueue(const QString &employeeId, const QString &status,
                    const QDateTime &checkTime);
    QString enqueueWithPhoto(const QString &employeeId, const QString &status,
                             const QByteArray &photoJpeg, const QDateTime &checkTime);

    // ── 调度发送（Writer 和 deviceId 作为参数传入，不持有）──
    void processOutbox(MessageWriter *writer, const QString &deviceId);

    // ── 响应处理 ──
    void handleUploadResponse(const QJsonObject &message);
    void handleServerError(const QJsonObject &message);

    // ── 重试控制 ──
    void retryAll(MessageWriter *writer, const QString &deviceId);
    void rollbackSendingToPending();  // 断连时调用
    int pendingCount() const;

    void setMaxRetryCount(int count)       { m_maxRetryCount = count; }
    void setRetryBackoffBaseMs(int ms)     { m_retryBackoffBaseMs = ms; }

signals:
    void attendanceReportResult(const QString &employeeId, bool success,
                                const QString &message);
    void uploadFinished(bool success, const QString &message);
    void pendingCountChanged(int count);

private slots:
    void onRetryTick();  // QTimer::timeout → processOutbox

private:
    QTimer *m_retryTimer;
    int m_retryRound = 0;
    int m_maxRetryCount = 5;
    int m_retryBackoffBaseMs = 2000;

    // 缓存的 writer 和 deviceId，用于定时器触发的重试
    MessageWriter *m_cachedWriter = nullptr;
    QString m_cachedDeviceId;
};

#endif // OUTBOXMANAGER_H
