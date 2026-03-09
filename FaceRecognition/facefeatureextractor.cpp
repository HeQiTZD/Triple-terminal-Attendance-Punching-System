#include "facefeatureextractor.h"

FaceFeatureExtractor::FaceFeatureExtractor() {}

arcfaceengine::FaceFeature FaceFeatureExtractor::FaceExtraction(const QImage &image)
{
    arcfaceengine::FaceFeature faceFeature;
    arcfaceengine* arcEngine = arcfaceengine::instance();
    if(!arcEngine->isInitialized()){
        qDebug()<<"引擎未初始化";
        return faceFeature;
    }

    QVector<arcfaceengine::FaceInfo> faceInfo = arcEngine->detectFace(image);
    if(faceInfo.isEmpty()){
        qDebug()<<"当前图片未检测到人脸";
        return faceFeature;
    }

    faceFeature = arcEngine->extractFeature(image,faceInfo[0]);

    return faceFeature;
}
