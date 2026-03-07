#ifndef ARCFACEENGINE_H
#define ARCFACEENGINE_H

#include <QObject>
#include <QRect>
#include <QImage>
#include <QVector>
#include <QByteArray>
#include <QDebug>

#include "arcsoft_face_sdk.h"
#include "merror.h"
class arcfaceengine
{
public:
    arcfaceengine();

private:
    //内部转换函数
    ASVLOFFSCREEN qImageToASVL(const QImage &image);
    MInt32 getPixeFormat(const QImage& image);

    MHandle m_engine;//SDK引擎句柄
    bool m_initialized;

    struct FaceInfo{
        QRect rect;//人脸位置
        int orient;//人脸角度
        int faceId;//人脸ID
    };

    struct FaceFeature{
        QByteArray data;//特征数据
        int size;//特征大小
    };

public:
    //单例模式
    static arcfaceengine* instance();

    //生命周期管理
    bool initialize(const QString &appId,const QString &sdkKey);
    void uninitialize();
    bool isInitialized() const;

    //核心功能
    //输入一张图片中识别并定位所有的人脸区域
    QVector<FaceInfo> detectFace(const QImage &image);

    //从检测到的某个人脸区域中，提取用于身份识别的人脸特征向量（人脸的 “数字指纹”）
    FaceFeature extractFeature(const QImage &image,const FaceInfo &faceInfo);

    //计算两个人脸特征向量的相似度，返回一个相似度分数
    float compareFeatures(const FaceFeature &feature1,const FaceFeature &feature2);

};

#endif // ARCFACEENGINE_H
