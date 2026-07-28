#ifndef ATTENDANCESERVICE_H
#define ATTENDANCESERVICE_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class TcpConnectionManager;

class AttendanceService : public QObject
{
    Q_OBJECT
        Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
        Q_PROPERTY(QVariantList records READ records NOTIFY recordsChanged)
        Q_PROPERTY(QVariantList archiveRecords READ archiveRecords NOTIFY archiveRecordsChanged)

public:
    explicit AttendanceService(QObject* parent = nullptr);
    void setTcpManager(TcpConnectionManager* tcp);

    bool busy() const { return m_busy; }
    QVariantList records() const { return m_records; }
    QVariantList archiveRecords() const { return m_archiveRecords; }

    // idFilter: <0 表示 JSON null（不按 id 过滤）；>=0 则按 id 精确匹配
    Q_INVOKABLE void query(int idFilter,
        const QString& employeeId,
        const QString& checkTime,
        const QString& deviceId,
        const QString& status,
        const QString& receivedTime);

    Q_INVOKABLE void createRecord(const QString& employeeId,
        const QString& checkTime,
        const QString& status,
        const QString& deviceId);

    // locate / updates：键与文档一致；空字符串的键可在组装 JSON 时省略（updates），或保留为 ""（按服务端约定，一般 updates 只发非空字段）
    Q_INVOKABLE void updateRecord(const QVariantMap& locate, const QVariantMap& updates);

    Q_INVOKABLE void deleteRecord(const QVariantMap& criteria);

    // P1 归档
    Q_INVOKABLE void queryArchive(int idFilter,
        const QString& employeeId,
        const QString& name,
        const QString& department,
        const QString& position,
        const QString& checkTime,
        const QString& deviceId,
        const QString& status,
        const QString& receivedTime,
        const QString& archivedAt,
        const QString& archiveReason);

    Q_INVOKABLE void deleteArchive(const QString& employeeId);

    // 导出文件到本地（客户端侧写文件），成功后 emit operationSucceeded("attendance.export", ...)
    Q_INVOKABLE void exportToFile(const QUrl& fileUrl, const QString& content);

signals:
    void busyChanged();
    void recordsChanged();
    void archiveRecordsChanged();

    void operationSucceeded(const QString& apiType, const QString& message);
    void operationFailed(const QString& apiType, int code, const QString& message);

private:
    void setBusy(bool v);
    static QJsonObject filterCriteriaToJson(const QVariantMap& m, bool forNestedLocateOrDelete);
    static bool hasAnyNonEmptyString(const QJsonObject& o);
    static QVariantList parseLiveRecords(const QJsonArray& arr);
    static QVariantList parseArchiveRecords(const QJsonArray& arr);

    TcpConnectionManager* m_tcp = nullptr;
    bool m_busy = false;
    QVariantList m_records;
    QVariantList m_archiveRecords;
};

#endif