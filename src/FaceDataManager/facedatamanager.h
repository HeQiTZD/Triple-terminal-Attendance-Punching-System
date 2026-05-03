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

    // 人脸特征提取
    QVector<FaceInfo> detectFace(const QImage &image);
    FaceFeature extractFeature(const QImage &image, const FaceInfo &faceInfo);

    bool extractFaceFeature(const QString &imagePath, QByteArray &featureVector);

    QString getErrorMessage(int errCode) const;

    QString selectImageFile(QWidget *parent = nullptr);

signals:
    void engineInitialized();
    void engineReleased();
    void featureExtracted(bool success);
    void errorOccurred(const QString &error);

private slots:
    void onEngineReleased();
    void onFeatureExtracted(bool success);
    void onErrorOccurred(const QString &error);

private:
    MHandle m_engine;
    bool m_initialized;
    QImage m_convertedImage;
    QMutex m_mutex;
};

#endif // FACEDATAMANAGER_H
