#pragma once

#include <QObject>
#include <QString>

class DataManager;
class DataService;
class TcpServer;
class SyncManager;
class ExportManager;
class FaceDataManager;

class TestApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit TestApi(DataService* dataService,
                     TcpServer* tcpServer,
                     SyncManager* syncManager,
                     ExportManager* exportManager,
                     FaceDataManager* faceDataManager,
                     QObject* parent = nullptr);

    QString lastError() const { return m_lastError; }

    Q_INVOKABLE bool initFaceEngine(const QString& appId, const QString& sdkKey);
    Q_INVOKABLE QString extractFeatureBase64(const QString& imagePath);
    Q_INVOKABLE QString selectImageFile();

    Q_INVOKABLE void requestPersonSync(const QString& deviceId);
    Q_INVOKABLE bool sendToClientJson(const QString& deviceId, const QString& jsonText);
    Q_INVOKABLE bool broadcastJson(const QString& jsonText);
    Q_INVOKABLE bool addFaceDataBase64(const QString& employeeId, const QString& featureBase64);
    Q_INVOKABLE bool updateFaceDataBase64(const QString& employeeId, const QString& featureBase64);
    Q_INVOKABLE bool deleteFaceDataByEmployeeId(const QString& employeeId);

    // ===== 文件辅助（供 QML 使用：另存为对话框、读写文本） =====
    Q_INVOKABLE QString selectSaveFile(const QString& title = QString(),
                                       const QString& defaultName = QString(),
                                       const QString& nameFilter = QString());
    Q_INVOKABLE QString selectOpenFile(const QString& title = QString(),
                                       const QString& nameFilter = QString());
    Q_INVOKABLE QString readTextFile(const QString& path);
    Q_INVOKABLE bool writeTextFile(const QString& path, const QString& content);

signals:
    void lastErrorChanged();

private:
    DataService* m_dataService = nullptr;
    TcpServer* m_tcpServer = nullptr;
    SyncManager* m_syncManager = nullptr;
    ExportManager* m_exportManager = nullptr;
    FaceDataManager* m_faceDataManager = nullptr;
    QString m_lastError;

    void setError(const QString& err);
};

