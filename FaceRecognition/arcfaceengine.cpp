#include "arcfaceengine.h"

arcfaceengine::arcfaceengine() {}

//QImage到SDK格式的转换
ASVLOFFSCREEN arcfaceengine::qImageToASVL(const QImage &image)
{
    ASVLOFFSCREEN asvl = {0};

    //确保图像格式正确
    QImage convertedImage = image.convertToFormat(QImage::Format_RGB888);

    asvl.u32PixelArrayFormat = ASVL_PAF_RGB24_R8G8B8;
    asvl.i32Width = convertedImage.width();
    asvl.i32Height = convertedImage.height();
    asvl.ppu8Plane[0] = const_cast<MUInt8*>(convertedImage.bits());
    asvl.pi32Pitch[0] = convertedImage.bytesPerLine();

    return asvl;
}

//获取图片格式
MInt32 arcfaceengine::getPixeFormat(const QImage &image)
{
    switch(image.format()){
    case QImage::Format_RGB888:
        return ASVL_PAF_RGB24_R8G8B8;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
        return ASVL_PAF_RGB32_R8G8B8;
    case QImage::Format_Grayscale8:
        return ASVL_PAF_GRAY;
    default:
        return ASVL_PAF_RGB24_R8G8B8;  // 默认RGB888
    }
}

//初始化引擎流程
bool arcfaceengine::initialize(const QString &appId, const QString &sdkKey)
{
    //1.在线激活
    MRESULT result = ASFOnlineActivation(appId.toLocal8Bit().data(),sdkKey.toLocal8Bit().data());
    if (MOK != result && MERR_ASF_ALREADY_ACTIVATED != result)
    {
        qWarning() << "激活失败，错误码：" << result;
        return false;
    }

    //初始化引擎
    MInt32 combinedMask= ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER;
    result = ASFInitEngine(ASF_DETECT_MODE_VIDEO,ASF_OP_0_ONLY,16,10,combinedMask,&m_engine);
    if(result!=MOK){
        qWarning() << "初始化失败，错误码：" << result;
        return false;
    }

    m_initialized = true;
    qDebug()<<"ArcFace引擎初始化成功";
    return true;
}

//特征对比
float arcfaceengine::compareFeatures(const FaceFeature &feature1, const FaceFeature &feature2)
{
    if (!m_initialized){
        qWarning() << "引擎未初始化";
        return 0.0f;
    }

    //构造SDK特征结构
    LPASF_FaceFeature sdkFeatture1 = {0};
    sdkFeatture1->feature = reinterpret_cast<MByte*>(const_cast<char*>(feature1.data.data()));
    sdkFeatture1->featureSize = feature1.size;

    LPASF_FaceFeature sdkFeatture2 = {0};
    sdkFeatture2->feature = reinterpret_cast<MByte*>(const_cast<char*>(feature2.data.data()));
    sdkFeatture2->featureSize = feature2.size;

    MFloat confidence = 0.0f;
    MRESULT result = ASFFaceFeatureCompare(
        m_engine,
        sdkFeatture1,
        sdkFeatture2,
        &confidence,
        ASF_LIFE_PHOTO//生活照对比模式
        );

    if(result!=MOK){
        qWarning()<<"特征对比失败"<<result;
        return 0.0f;
    }

    qDebug()<<"特征对比相似度:"<<confidence;
    return confidence;
}

//提取人脸特征
arcfaceengine::FaceFeature arcfaceengine::extractFeature(const QImage &image, const FaceInfo &faceInfo)
{
    FaceFeature feature;

    if(!m_initialized){
        qWarning()<<"引擎未初始化";
        return feature;
    }

    //转换图像格式
    ASVLOFFSCREEN asvlImage = qImageToASVL(image);

    //构造单人脸信息
    ASF_SingleFaceInfo singleFaceInfo = {0};
    singleFaceInfo.faceRect.left = faceInfo.rect.left();
    singleFaceInfo.faceRect.top = faceInfo.rect.top();
    singleFaceInfo.faceRect.right = faceInfo.rect.right();
    singleFaceInfo.faceRect.bottom = faceInfo.rect.bottom();
    singleFaceInfo.faceOrient = faceInfo.orient;

    //提取特征
    LPASF_FaceFeature sdkFeature = {0};
    MRESULT result = ASFFaceFeatureExtractEx(m_engine,&asvlImage,&singleFaceInfo,sdkFeature);

    if(result!=MOK){
        qWarning()<<"特征提取失败"<<result;
        return feature;
    }

    //复制特征数据
    feature.size = sdkFeature->featureSize;
    feature.data = QByteArray(reinterpret_cast<char*>(sdkFeature->feature),sdkFeature->featureSize);

    //后续查找SDK特点的释放函数

    qDebug()<<"特征提取成功，大小："<< feature.size;
    return feature;
}

//检测人脸
QVector<arcfaceengine::FaceInfo> arcfaceengine::detectFace(const QImage &image)
{
    QVector<FaceInfo> faceInfos;
    if(!m_initialized){
        qWarning()<<"引擎未初始化";
        return faceInfos;
    }

    //转换图像格式
    ASVLOFFSCREEN asvlImage = qImageToASVL(image);

    //调用SDK检测人脸
    ASF_MultiFaceInfo detectedFaces={0};
    MRESULT result = ASFDetectFacesEx(m_engine,&asvlImage,&detectedFaces,ASF_DETECT_MODEL_RGB);

    if(result!=MOK){
        qWarning()<<"人脸检测失败"<<result;
        return faceInfos;
    }

    //转换结果
    for(MInt32 i=0;i<detectedFaces.faceNum;++i){
        FaceInfo info;
        info.rect=QRect(
            detectedFaces.faceRect[i].left,
            detectedFaces.faceRect[i].top,
            detectedFaces.faceRect[i].right-detectedFaces.faceRect[i].left,
            detectedFaces.faceRect[i].bottom-detectedFaces.faceRect[i].top
            );
        info.orient = detectedFaces.faceOrient[i];
        info.faceId = detectedFaces.faceID ? detectedFaces.faceID[i] : -1;

        faceInfos.append(info);
    }

    qDebug()<<"检测到"<<faceInfos.size()<<"张人脸";
    return faceInfos;
}
