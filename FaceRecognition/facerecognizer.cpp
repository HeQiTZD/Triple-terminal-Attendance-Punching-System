#include "facerecognizer.h"

FaceRecognizer::FaceRecognizer()
{
    
}

FaceRecognizer::~FaceRecognizer()
{
    if(videoCapture){
        delete videoCapture;
        videoCapture = nullptr;
    }
}

void FaceRecognizer::init()
{
    connect(videoCapture,&VideoFrameCapture::frameCaptured,this,&FaceRecognizer::WanZhengYeWuLiuCheng);
    
    //引擎初始化
    arcEngine = arcfaceengine::instance();
    QString appid="JBT9EUHsd8RVuvbgwNLNFP1ezsdtsuUenhD6gjSkoKhG";
    QString Key="4szkxxMUBVRLirbAsTMzT9u2b5R9w5umHiucbPvTy91Z";
    if (!arcEngine->initialize(appid, Key)) {
        qDebug() << "ArcFace 引擎初始化失败";
        return;
    }

    //加载特征到内存
    dataBase = FaceDatabaseManager::instance();
    dataBase->loadFromDatabase();
}

void FaceRecognizer::WanZhengYeWuLiuCheng(QImage image)
{
    QMutexLocker locker(&m_mutex); // 保护成员变量

    //人脸检测
    m_FaceInfo = arcEngine->detectFace(image);
    if(m_FaceInfo.isEmpty()){
        qDebug()<<"人脸数量为0";
        return;
    }

    //特征提取
    m_FaceFeature = arcEngine->extractFeature(image,m_FaceInfo[0]);

    //特征对比
    m_bestMatch = dataBase->findBestMatch(m_FaceFeature);

    //新增，识别成功且相似度达标，保存打卡记录
    if(m_bestMatch.second>0.8f){
        QString employeeId = m_bestMatch.first;
        QString status = "正常";

        //保存到本地数据库
        bool saved = LocalStorage::instance()->addAttendanceRecord(employeeId,status);

        if(saved){
            //尝试上传
            Protocol::AttendanceRecord record;
            record.employeeId = employeeId;
            record.checktTime = QDateTime::currentDateTime().toString(Qt::ISODate);
            record.status = status;

            Networkclient::instance()->uploadAttendance(record);
        }
    }
}

QVector<arcfaceengine::FaceInfo> FaceRecognizer::getFaceInfo()
{
    QMutexLocker locker(&m_mutex);
    return m_FaceInfo;
}

arcfaceengine::FaceFeature FaceRecognizer::getFaceFeature()
{
    QMutexLocker locker(&m_mutex);
    return m_FaceFeature;
}

QPair<QString, float> FaceRecognizer::getbestMatch()
{
    QMutexLocker locker(&m_mutex);
    return m_bestMatch;
}
