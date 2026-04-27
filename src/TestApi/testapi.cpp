#include "testapi.h"

#include "src/DataManager/datamanager.h"
#include "src/ExportManager/exportmanager.h"
#include "src/FaceDataManager/facedatamanager.h"
#include "src/SyncManager/syncmanager.h"
#include "src/TcpServer/tcpserver.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

TestApi::TestApi(DataManager* dataManager,
                 TcpServer* tcpServer,
                 SyncManager* syncManager,
                 ExportManager* exportManager,
                 FaceDataManager* faceDataManager,
                 QObject* parent)
    : QObject(parent),
      m_dataManager(dataManager),
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

double TestApi::compareFeatureBase64(const QString& f1Base64, const QString& f2Base64)
{
    if (!m_faceDataManager) {
        setError("FaceDataManager 未初始化");
        return -1.0;
    }
    if (f1Base64.isEmpty() || f2Base64.isEmpty()) {
        setError("特征不能为空");
        return -1.0;
    }
    const QByteArray f1 = QByteArray::fromBase64(f1Base64.toLatin1());
    const QByteArray f2 = QByteArray::fromBase64(f2Base64.toLatin1());
    if (f1.isEmpty() || f2.isEmpty()) {
        setError("Base64 解码失败");
        return -1.0;
    }
    float similarity = 0.0f;
    const bool ok = m_faceDataManager->compareFaceFeature(f1, f2, similarity);
    if (!ok) {
        setError("特征对比失败（请查看运行日志）");
        return -1.0;
    }
    setError(QString());
    return static_cast<double>(similarity);
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

