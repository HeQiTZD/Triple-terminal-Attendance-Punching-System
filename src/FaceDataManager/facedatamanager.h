#ifndef FACEDATAMANAGER_H
#define FACEDATAMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QRect>
#include <QVector>
#include <QImage>
#include <QMutex>

// forward declaration to avoid including QWidget header in this header file
class QWidget;

// 直接包含 ArcSoft SDK 头文件
#include "../third_party/arcface/include/arcsoft_face_sdk.h"
#include "../third_party/arcface/include/merror.h"

class FaceDataManager : public QObject
{
    Q_OBJECT
public:
    struct FaceInfo {
        QRect rect;
        int orient;
        int faceId;
    };
    struct FaceFeature {
        QByteArray data;
        int size;
        FaceFeature() : size(0) {}
    };

    explicit FaceDataManager(QObject *parent = nullptr);
    ~FaceDataManager();

    // 引擎管理接口
    bool initializeEngine(const QString &appId, const QString &sdkKey);
    void releaseEngine();
    bool isEngineInitialized() const;

    // 人脸特征提取与对比
    QVector<FaceInfo> detectFace(const QImage &image);
    FaceFeature extractFeature(const QImage &image, const FaceInfo &faceInfo);
    float compareFeatures(const FaceFeature &feature1, const FaceFeature &feature2) const;

    bool extractFaceFeature(const QString &imagePath, QByteArray &featureVector);
    bool compareFaceFeature(const QByteArray &feature1, const QByteArray &feature2, float &similarity) const;
    bool isSimilarityAboveThreshold(float similarity, float threshold = 0.8f) const;
    bool shouldUpdateFaceFeature(const QByteArray &newFeature, const QByteArray &oldFeature) const;

    QString getErrorMessage(int errCode) const;

    /**
     * @brief 打开文件夹选择图片，返回选中的图片路径（实现见 cpp，不包含 QFileDialog 头文件）
     * @param parent 父窗口指针
     * @return 选中的图片路径，未选择返回空字符串
     */
    QString selectImageFile(QWidget *parent = nullptr);

signals:
    void engineInitialized();
    void engineReleased();
    void featureExtracted(bool success);
    void errorOccurred(const QString &error);

private:
    MHandle m_engine;
    bool m_initialized;
    QImage m_convertedImage;
    QMutex m_mutex;
};

#endif // FACEDATAMANAGER_H
