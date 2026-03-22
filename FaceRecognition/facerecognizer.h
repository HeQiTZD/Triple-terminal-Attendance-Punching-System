#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

// 视频采集相关
#include "../CameraCapture/videoframecapture.h"
#include "../CameraCapture/videoframeconverter.h"
#include "../CameraCapture/cameracapture.h"

// 人脸识别相关
#include "arcfaceengine.h"
#include "facedatabasemanager.h"

// Qt 相关
#include <QCamera>
#include <QDebug>
#include <QObject>
#include <QImage>
#include <QVector>
#include <QPair>
#include <QString>
#include <QMutex>

#include "../LocalStorage/localstorage.h"
#include "../NetworkClient/networkclient.h"

class FaceRecognizer : public QObject
{
    Q_OBJECT
    
public:
    FaceRecognizer();
    ~FaceRecognizer();
    //初始化人脸识别配置
    void init();

public slots:
    void WanZhengYeWuLiuCheng(QImage image);//人脸识别整体流程

private:
    arcfaceengine* arcEngine = nullptr;//人脸功能实例
    VideoFrameCapture* videoCapture = nullptr;//捕获实例,通过其他路径传入
    FaceDatabaseManager* dataBase = nullptr;//内存加载特征，特征对比实例

private:
    QVector<arcfaceengine::FaceInfo> m_FaceInfo;//人脸检测信息
    arcfaceengine::FaceFeature m_FaceFeature;//提取的特征信息
    QPair<QString, float> m_bestMatch;//特征对比结果

public:
    QVector<arcfaceengine::FaceInfo> getFaceInfo();
    arcfaceengine::FaceFeature getFaceFeature();
    QPair<QString, float> getbestMatch();

private:
    QMutex m_mutex;
};

#endif // FACERECOGNIZER_H
