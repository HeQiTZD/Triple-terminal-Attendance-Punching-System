#include "testapi.h"

#include "src/Services/dataservice.h"
#include "src/ExportManager/exportmanager.h"
#include "src/FaceDataManager/facedatamanager.h"
#include "src/SyncManager/syncmanager.h"
#include "src/TcpServer/tcpserver.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

TestApi::TestApi(DataService* dataService,
                 TcpServer* tcpServer,
                 SyncManager* syncManager,
                 ExportManager* exportManager,
                 FaceDataManager* faceDataManager,
                 QObject* parent)
    : QObject(parent),
      m_dataService(dataService),
      m_tcpServer(tcpServer),
      m_syncManager(syncManager),
      m_exportManager(exportManager),
      m_faceDataManager(faceDataManager)
{
}

void TestApi::setError(const QString& err)
{
    if (m_lastError == err) return;
    m_lastError = err;
    emit lastErrorChanged();
}

bool TestApi::initFaceEngine(const QString& appId, const QString& sdkKey)
{
    if (!m_faceDataManager) {
        setError("FaceDataManager 未初始化");
        return false;
    }
    if (appId.isEmpty() || sdkKey.isEmpty()) {
        setError("appId/sdkKey 不能为空");
        return false;
    }
    const bool ok = m_faceDataManager->initializeEngine(appId, sdkKey);
    if (!ok) {
        setError("初始化人脸引擎失败（请查看运行日志）");
        return false;
    }
    setError(QString());
    return true;
}

QString TestApi::extractFeatureBase64(const QString& imagePath)
{
    if (!m_faceDataManager) {
        setError("FaceDataManager 未初始化");
        return QString();
    }
    if (imagePath.isEmpty()) {
        setError("imagePath 不能为空");
        return QString();
    }
    QByteArray feature;
    const bool ok = m_faceDataManager->extractFaceFeature(imagePath, feature);
    if (!ok || feature.isEmpty()) {
        setError("提取特征失败（请查看运行日志）");
        return QString();
    }
    setError(QString());
    return QString::fromLatin1(feature.toBase64());
}

QString TestApi::selectImageFile()
{
    if (!m_faceDataManager) {
        setError("FaceDataManager 未初始化");
        return QString();
    }
    const QString path = m_faceDataManager->selectImageFile(nullptr);
    if (path.isEmpty()) {
        setError("未选择图片");
        return QString();
    }
    setError(QString());
    return path;
}

void TestApi::requestPersonSync(const QString& deviceId)
{
    if (!m_syncManager) {
        setError("SyncManager 未初始化");
        return;
    }
    if (deviceId.isEmpty()) {
        setError("deviceId 不能为空");
        return;
    }
    m_syncManager->sendPersonSyncNow(deviceId);
    setError(QString());
}

bool TestApi::sendToClientJson(const QString& deviceId, const QString& jsonText)
{
    if (!m_tcpServer) {
        setError("TcpServer 未初始化");
        return false;
    }
    if (deviceId.trimmed().isEmpty()) {
        setError("deviceId 不能为空");
        return false;
    }
    if (jsonText.trimmed().isEmpty()) {
        setError("jsonText 不能为空");
        return false;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &err);
    if (doc.isNull() || !doc.isObject()) {
        setError(QString("JSON 解析失败：%1").arg(err.errorString()));
        return false;
    }

    const bool ok = m_tcpServer->sendToClient(deviceId, doc.object());
    if (!ok) {
        setError("发送失败：设备不在线或 TCP 发送失败");
        return false;
    }

    setError(QString());
    return true;
}

bool TestApi::broadcastJson(const QString& jsonText)
{
    if (!m_tcpServer) {
        setError("TcpServer 未初始化");
        return false;
    }
    if (jsonText.trimmed().isEmpty()) {
        setError("jsonText 不能为空");
        return false;
    }
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &err);
    if (doc.isNull() || !doc.isObject()) {
        setError(QString("JSON 解析失败：%1").arg(err.errorString()));
        return false;
    }
    m_tcpServer->brodcastsToAll(doc.object());
    setError(QString());
    return true;
}

bool TestApi::addFaceDataBase64(const QString& employeeId, const QString& featureBase64)
{
    if (!m_dataService) {
        setError("DataService 未初始化");
        return false;
    }
    if (employeeId.trimmed().isEmpty()) {
        setError("employeeId 不能为空");
        return false;
    }
    if (featureBase64.trimmed().isEmpty()) {
        setError("featureBase64 不能为空");
        return false;
    }

    const QByteArray feature = QByteArray::fromBase64(featureBase64.toLatin1());
    if (feature.isEmpty()) {
        setError("Base64 解码失败");
        return false;
    }

    const bool ok = m_dataService->addFaceDataByEmployeeId(employeeId, feature);
    if (!ok) {
        setError("新增人脸特征失败（请查看数据库错误日志）");
        return false;
    }

    setError(QString());
    return true;
}

bool TestApi::updateFaceDataBase64(const QString& employeeId, const QString& featureBase64)
{
    if (!m_dataService) {
        setError("DataService 未初始化");
        return false;
    }
    if (employeeId.trimmed().isEmpty()) {
        setError("employeeId 不能为空");
        return false;
    }
    if (featureBase64.trimmed().isEmpty()) {
        setError("featureBase64 不能为空");
        return false;
    }

    const QByteArray feature = QByteArray::fromBase64(featureBase64.toLatin1());
    if (feature.isEmpty()) {
        setError("Base64 解码失败");
        return false;
    }

    const bool ok = m_dataService->updateFaceDataByEmployeeId(employeeId, feature);
    if (!ok) {
        setError("更新人脸特征失败（请查看数据库错误日志）");
        return false;
    }

    setError(QString());
    return true;
}

bool TestApi::deleteFaceDataByEmployeeId(const QString& employeeId)
{
    if (!m_dataService) {
        setError("DataService 未初始化");
        return false;
    }
    if (employeeId.trimmed().isEmpty()) {
        setError("employeeId 不能为空");
        return false;
    }

    const bool ok = m_dataService->deleteFaceDataByEmployeeId(employeeId);
    if (!ok) {
        setError("删除人脸特征失败（请查看数据库错误日志）");
        return false;
    }

    setError(QString());
    return true;
}

