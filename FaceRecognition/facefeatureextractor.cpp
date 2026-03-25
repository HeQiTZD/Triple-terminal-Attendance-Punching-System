#include "facefeatureextractor.h"

FaceFeatureExtractor::FaceFeatureExtractor() {}

arcfaceengine::FaceFeature FaceFeatureExtractor::FaceExtraction(const QImage &image)
{
    arcfaceengine::FaceFeature faceFeature;
    arcfaceengine* arcEngine = arcfaceengine::instance();
    if(!arcEngine->isInitialized()){
        return faceFeature;
    }

    QVector<arcfaceengine::FaceInfo> faceInfo = arcEngine->detectFace(image);
    if(faceInfo.isEmpty()){
        return faceFeature;
    }

    faceFeature = arcEngine->extractFeature(image,faceInfo[0]);

    return faceFeature;
}
