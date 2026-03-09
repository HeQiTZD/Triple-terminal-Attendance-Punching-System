#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

#include <QObject>
#include <QImage>
#include <QRect>
#include <QVector>
#include <QPair>
#include <QString>
#include <QMutex>

// 视频采集相关
#include "../CameraCapture/videoframecapture.h"
#include "../CameraCapture/videoframeconverter.h"
#include "../CameraCapture/cameracapture.h"

// 人脸识别相关
#include "arcfaceengine.h"
#include "facedatabasemanager.h"

// Qt 相关
#include <QDateTime>
#include <QMap>
#include <QMutexLocker>
#include <QCamera>
#include <QDebug>

class FaceRecognizer : public QObject
{
    Q_OBJECT
    
    FaceRecognizer();

    //初始化人脸识别配置
    bool init();

    void WanZhengYeWuLiuCheng(QImage image);//人脸识别整体流程

private:
    QCamera* camera;//摄像头实例
    arcfaceengine* arcEngine;//人脸功能实例
    VideoFrameCapture* videoCapture;//捕获实例
    FaceDatabaseManager* Database;//内存加载特征，特征对比实例

private:
    QVector<arcfaceengine::FaceInfo> m_FaceInfo;//人脸检测信息
    arcfaceengine::FaceFeature m_FaceFeature;//提取的特征信息
    QPair<QString, float> m_bestMatch;//特征对比结果

public:
    QVector<arcfaceengine::FaceInfo> getFaceInfo();
    arcfaceengine::FaceFeature getFaceFeature();
    QPair<QString, float> getbestMatch();
};

#endif // FACERECOGNIZER_H
