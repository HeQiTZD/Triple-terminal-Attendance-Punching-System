#include "facerecognizer.h"

FaceRecognizer::FaceRecognizer()
{
    connect(videoCapture,&VideoFrameCapture::frameCaptured,this,&FaceRecognizer::WanZhengYeWuLiuCheng);
}

bool FaceRecognizer::init()
{
    //引擎初始化
    arcEngine = arcfaceengine::instance();
    QString appid="JBT9EUHsd8RVuvbgwNLNFP1ezsdtsuUenhD6gjSkoKhG";
    QString Key="4szkxxMUBVRLirbAsTMzT9u2b5R9w5umHiucbPvTy91Z";
    arcEngine->initialize(appid,Key);

    //加载特征到内存
    Database = FaceDatabaseManager::instance();
    Database->loadFromDatabase();

    //摄像头初始化
    CameraCapture cameraCapture;
    cameraCapture.initCamera();
    camera = cameraCapture.getCamera();
}

void FaceRecognizer::WanZhengYeWuLiuCheng(QImage image)
{
    //启动摄像头,开启视频帧捕获处理
    videoCapture->captureFrame(camera);

    //人脸检测
    m_FaceInfo = arcEngine->detectFace(image);

    //特征提取
    m_FaceFeature = arcEngine->extractFeature(image,m_FaceInfo[0]);

    //特征对比
    m_bestMatch = Database->findBestMatch(m_FaceFeature);
}

QVector<arcfaceengine::FaceInfo> FaceRecognizer::getFaceInfo()
{
    return m_FaceInfo;
}

arcfaceengine::FaceFeature FaceRecognizer::getFaceFeature()
{
    return m_FaceFeature;
}

QPair<QString, float> FaceRecognizer::getbestMatch()
{
    return m_bestMatch;
}
