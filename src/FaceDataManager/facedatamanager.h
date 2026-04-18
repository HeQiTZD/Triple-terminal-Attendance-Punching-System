#ifndef FACEDATAMANAGER_H
#define FACEDATAMANAGER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QJsonObject>
#include <QImage>

// 人脸数据管理器
class FaceDataManager : public QObject
{
    Q_OBJECT
        Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY initializationChanged)

public:
    explicit FaceDataManager(DataManager* dataManager, QObject* parent = nullptr);
    ~FaceDataManager();

    bool isInitialized() const { return m_initialized; }

    // ========== 人脸数据 CRUD 操作 ==========

    // 添加人脸数据（输入图片路径，内部调用人脸识别库生成特征向量）
    Q_INVOKABLE bool addFaceData(int personId, const QString& imagePath);

    // 更新人脸数据（替换现有人脸）
    Q_INVOKABLE bool updateFaceData(int faceDataId, const QString& imagePath);

    // 删除人脸数据
    Q_INVOKABLE bool deleteFaceData(int faceDataId);

    // 删除指定人员的所有人脸数据
    Q_INVOKABLE bool deleteFaceDataByPerson(int personId);

    // 获取所有人脸数据
    Q_INVOKABLE QList<QObject*> getAllFaceData();

    // 获取指定人员的所有人脸数据
    Q_INVOKABLE QList<QObject*> getFaceDataByPersonId(int personId);

    // 获取单个人脸数据
    Q_INVOKABLE QObject* getFaceDataById(int id);

    // 人脸识别比对（返回相似度，范围 0-1）
    Q_INVOKABLE double compareFaceFeatures(const QByteArray& feature1, const QByteArray& feature2);

    // 批量导入人脸数据
    Q_INVOKABLE bool importFaceDataBatch(const QString& directoryPath);

    // 导出人脸数据（包括图片和特征向量）
    Q_INVOKABLE bool exportFaceData(int personId, const QString& exportPath);

    // ========== 人脸特征提取 ==========

    // 从图片提取人脸特征向量（调用第三方库）
    QByteArray extractFaceFeature(const QString& imagePath, double& outQuality);

    // 验证图片是否包含有效人脸
    bool validateFaceImage(const QString& imagePath);

signals:
    void initializationChanged();
    void faceDataAdded(int id, int personId);
    void faceDataUpdated(int id, int personId);
    void faceDataDeleted(int id);
    void faceDataQualityChanged(int id, double quality);
    void batchImportProgress(int current, int total);
    void batchImportCompleted(int successCount, int failureCount);
    void errorOccurred(const QString& errorString);

private slots:
    // 异步处理人脸提取
    void onFaceExtractionCompleted(int personId, const QByteArray& feature, double quality);
    void onFaceExtractionFailed(const QString& imagePath, const QString& error);

private:
    DataManager* m_dataManager;
    bool m_initialized;

    // 数据库表创建
    bool createFaceDataTable();

    // 特征向量管理
    bool saveFaceFeatureToDb(int personId, const QByteArray& feature,
        double quality, const QString& imagePath);
    bool updateFaceFeatureInDb(int faceDataId, const QByteArray& feature,
        double quality, const QString& imagePath);

    // 文件管理
    QString generateFaceImagePath(int personId, int sequenceNumber);
    bool copyFaceImageToStorage(const QString& sourcePath, const QString& destPath);
    bool deleteFaceImageFromStorage(const QString& imagePath);
};
#endif // FACEDATAMANAGER_H