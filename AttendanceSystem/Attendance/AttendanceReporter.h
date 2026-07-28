#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QObject>
#include <QString>

/// 考勤上报协调器 — 封装 outbox 持久化 → 发送 → 响应 → 清理 的完整流程
class AttendanceReporter : public QObject
{
    Q_OBJECT

public:
    explicit AttendanceReporter(QObject *parent = nullptr);

    /// 考勤上报（无照片）
    QString report(const QString &employeeId,
                   const QString &status,
                   const QDateTime &checkTime = QDateTime::currentDateTime());

    /// 考勤上报（带照片）
    QString reportWithPhoto(const QString &employeeId,
                            const QString &status,
                            const QByteArray &photoJpeg,
                            const QDateTime &checkTime = QDateTime::currentDateTime());

    /// 重试所有待发送记录
    void retryAll();

    /// 待发送记录数
    int pendingCount() const;

    /// 检查是否可以上报
    bool canReport() const;

signals:
    /// 单条上报结果
    void reportCompleted(const QString &employeeId, bool success, const QString &message);

    /// 待发送数量变化
    void pendingCountChanged(int count);

public slots:
    /// 接收来自 NetworkClient 的上报结果
    void onReportResult(const QString &employeeId, bool success, const QString &message);

    /// 连接状态变化时更新内部状态
    void onConnectionStateChanged(bool isOnline);

private:
    bool m_isOnline = false;
};
