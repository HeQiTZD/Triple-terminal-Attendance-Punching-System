#ifndef FACEFEATUREEXTRACTOR_H
#define FACEFEATUREEXTRACTOR_H

#include <QImage>
#include <QDebug>
#include "arcfaceengine.h"
class FaceFeatureExtractor
{
public:
    FaceFeatureExtractor();

    arcfaceengine::FaceFeature FaceExtraction(const QImage &image);

};

#endif // FACEFEATUREEXTRACTOR_H
