#include "arcfaceengine.h"

// 静态成员初始化
arcfaceengine* arcfaceengine::s_instance = nullptr;
QMutex arcfaceengine::s_mutex;

// 构造函数：初始化成员变量
arcfaceengine::arcfaceengine() : m_engine(nullptr), m_initialized(false)
{
}

// 析构函数：自动释放引擎资源，防止内存泄漏
arcfaceengine::~arcfaceengine()
{
    uninitialize();
}

// 单例模式：双重检查锁实现，保证线程安全
arcfaceengine* arcfaceengine::instance()
{
    if (!s_instance) {
        QMutexLocker locker(&s_mutex);  // 加锁
        if (!s_instance) {
            s_instance = new arcfaceengine();
        }
    }
    return s_instance;
}

// 初始化引擎：激活 + 创建引擎实例
bool arcfaceengine::initialize(const QString &appId, const QString &sdkKey)
{
    // 防止重复初始化
    if (m_initialized) {
        return true;
    }

    // 步骤1：在线激活SDK（需要联网，首次激活后可离线使用）
    MRESULT result = ASFOnlineActivation(appId.toLocal8Bit().data(), sdkKey.toLocal8Bit().data());
    if (MOK != result && MERR_ASF_ALREADY_ACTIVATED != result) {
        qWarning() << "激活失败，错误码：" << result;
        return false;
    }

    // 步骤2：初始化引擎
    // combinedMask：指定引擎支持的功能（人脸检测 + 人脸识别 + 年龄 + 性别）
    MInt32 combinedMask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER;
    
    // ASF_DETECT_MODE_VIDEO：视频模式，支持人脸追踪
    // ASF_OP_0_ONLY：人脸角度检测范围（0度，即正脸）
    // 16：最小人脸尺寸（像素），越小能检测到越小的人脸，但速度越慢
    // 10：最大人脸数量
    result = ASFInitEngine(ASF_DETECT_MODE_VIDEO, ASF_OP_0_ONLY, 16, 10, combinedMask, &m_engine);
    if (result != MOK) {
        qWarning() << "初始化失败，错误码：" << result;
        return false;
    }

    m_initialized = true;
    return true;
}

// 释放引擎资源
void arcfaceengine::uninitialize()
{
    if (m_initialized && m_engine) {
        ASFUninitEngine(m_engine);  // SDK引擎反初始化
        m_engine = nullptr;
        m_initialized = false;
    }
}

// 检查初始化状态
bool arcfaceengine::isInitialized() const
{
    return m_initialized;
}

// 获取QImage格式对应的SDK像素格式常量
MInt32 arcfaceengine::getPixelFormat(const QImage &image)
{
    switch (image.format()) {
    case QImage::Format_RGB888:
        return ASVL_PAF_RGB24_R8G8B8;   // RGB888格式
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
        return ASVL_PAF_RGB32_R8G8B8;   // RGB32格式
    case QImage::Format_Grayscale8:
        return ASVL_PAF_GRAY;           // 灰度图
    default:
        return ASVL_PAF_RGB24_R8G8B8;   // 默认使用RGB888
    }
}

// 人脸检测：输入图像，返回检测到的所有人脸信息
QVector<arcfaceengine::FaceInfo> arcfaceengine::detectFace(const QImage &image)
{
    QVector<FaceInfo> faceInfos;
    
    // 前置检查
    if (!m_initialized) {
        qWarning() << "引擎未初始化";
        return faceInfos;
    }
    if (image.isNull()) {
        qWarning() << "输入图像为空";
        return faceInfos;
    }



    // 步骤1：图像格式转换
    // Qt的Format_RGB888是R-G-B顺序，但SDK可能需要B-G-R顺序
    // 先转为RGB888，再交换R和B通道
    QImage tempImage = image.convertToFormat(QImage::Format_RGB888);
    
    // 交换R和B通道（RGB -> BGR）
    m_convertedImage = QImage(tempImage.size(), QImage::Format_RGB888);
    for (int y = 0; y < tempImage.height(); ++y) {
        const uchar* srcLine = tempImage.scanLine(y);
        uchar* dstLine = m_convertedImage.scanLine(y);
        for (int x = 0; x < tempImage.width(); ++x) {
            dstLine[x * 3 + 0] = srcLine[x * 3 + 2];  // B = R
            dstLine[x * 3 + 1] = srcLine[x * 3 + 1];  // G = G
            dstLine[x * 3 + 2] = srcLine[x * 3 + 0];  // R = B
        }
    }

    // 步骤2：宽度四字节对齐（SDK要求：宽度必须是4的倍数）
    int originalWidth = m_convertedImage.width();
    int alignedWidth = (originalWidth / 4) * 4;  // 向下取整到4的倍数

    if (alignedWidth != originalWidth) {
        // 裁剪右侧多余像素，确保宽度对齐
        m_convertedImage = m_convertedImage.copy(0, 0, alignedWidth, m_convertedImage.height());
    }

    // 步骤3：构造SDK图像结构
    ASVLOFFSCREEN asvl = {0};
    asvl.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;   // 使用BGR格式
    asvl.i32Width = m_convertedImage.width();           // 图像宽度
    asvl.i32Height = m_convertedImage.height();         // 图像高度
    asvl.ppu8Plane[0] = m_convertedImage.bits();        // 图像数据指针
    asvl.pi32Pitch[0] = m_convertedImage.bytesPerLine();// 每行字节数（可能有填充）



    // 步骤4：调用SDK人脸检测
    ASF_MultiFaceInfo detectedFaces = {0};
    MRESULT result = ASFDetectFacesEx(m_engine, &asvl, &detectedFaces, ASF_DETECT_MODEL_RGB);

    if (result != MOK) {
        qWarning() << "人脸检测失败，错误码：" << result;
        return faceInfos;
    }

    // 步骤5：转换SDK结果为自定义结构
    for (MInt32 i = 0; i < detectedFaces.faceNum; ++i) {
        FaceInfo info;
        // SDK返回的是left/top/right/bottom，转换为QRect的x/y/width/height
        info.rect = QRect(
            detectedFaces.faceRect[i].left,
            detectedFaces.faceRect[i].top,
            detectedFaces.faceRect[i].right - detectedFaces.faceRect[i].left,
            detectedFaces.faceRect[i].bottom - detectedFaces.faceRect[i].top
        );
        info.orient = detectedFaces.faceOrient[i];  // 人脸角度
        info.faceId = detectedFaces.faceID ? detectedFaces.faceID[i] : -1;  // 追踪ID
        faceInfos.append(info);
    }

    return faceInfos;
}

// 特征提取：从指定人脸区域提取特征向量
arcfaceengine::FaceFeature arcfaceengine::extractFeature(const QImage &image, const FaceInfo &faceInfo)
{
    FaceFeature feature;

    // 前置检查
    if (!m_initialized) {
        qWarning() << "引擎未初始化";
        return feature;
    }
    if (image.isNull()) {
        qWarning() << "输入图像为空";
        return feature;
    }

    // 图像格式转换（RGB -> BGR）
    QImage tempImage = image.convertToFormat(QImage::Format_RGB888);
    
    m_convertedImage = QImage(tempImage.size(), QImage::Format_RGB888);
    for (int y = 0; y < tempImage.height(); ++y) {
        const uchar* srcLine = tempImage.scanLine(y);
        uchar* dstLine = m_convertedImage.scanLine(y);
        for (int x = 0; x < tempImage.width(); ++x) {
            dstLine[x * 3 + 0] = srcLine[x * 3 + 2];  // B = R
            dstLine[x * 3 + 1] = srcLine[x * 3 + 1];  // G = G
            dstLine[x * 3 + 2] = srcLine[x * 3 + 0];  // R = B
        }
    }

    int originalWidth = m_convertedImage.width();
    int alignedWidth = (originalWidth / 4) * 4;

    if (alignedWidth != originalWidth) {
        m_convertedImage = m_convertedImage.copy(0, 0, alignedWidth, m_convertedImage.height());
    }

    // 构造SDK图像结构
    ASVLOFFSCREEN asvl = {0};
    asvl.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
    asvl.i32Width = m_convertedImage.width();
    asvl.i32Height = m_convertedImage.height();
    asvl.ppu8Plane[0] = m_convertedImage.bits();
    asvl.pi32Pitch[0] = m_convertedImage.bytesPerLine();

    // 构造单人脸信息（指定要提取特征的人脸区域）
    ASF_SingleFaceInfo singleFaceInfo = {0};
    singleFaceInfo.faceRect.left = faceInfo.rect.left();
    singleFaceInfo.faceRect.top = faceInfo.rect.top();
    singleFaceInfo.faceRect.right = faceInfo.rect.right();
    singleFaceInfo.faceRect.bottom = faceInfo.rect.bottom();
    singleFaceInfo.faceOrient = faceInfo.orient;  // 角度信息用于特征提取时的校正

    // 调用SDK提取特征
    ASF_FaceFeature sdkFeature = {0};
    MRESULT result = ASFFaceFeatureExtractEx(m_engine, &asvl, &singleFaceInfo, &sdkFeature);

    if (result != MOK) {
        qWarning() << "特征提取失败，错误码：" << result;
        return feature;
    }

    // 复制特征数据到自定义结构（SDK内部数据需要拷贝出来）
    feature.size = sdkFeature.featureSize;
    feature.data = QByteArray(reinterpret_cast<char*>(sdkFeature.feature), sdkFeature.featureSize);

    return feature;
}

// 特征对比：计算两个人脸特征的相似度
float arcfaceengine::compareFeatures(const FaceFeature &feature1, const FaceFeature &feature2)
{
    // 前置检查
    if (!m_initialized) {
        qWarning() << "引擎未初始化";
        return 0.0f;
    }
    if (feature1.size == 0 || feature2.size == 0) {
        qWarning() << "特征数据为空";
        return 0.0f;
    }

    // 构造SDK特征结构
    ASF_FaceFeature sdkFeature1 = {0};
    sdkFeature1.feature = reinterpret_cast<MByte*>(const_cast<char*>(feature1.data.data()));
    sdkFeature1.featureSize = feature1.size;

    ASF_FaceFeature sdkFeature2 = {0};
    sdkFeature2.feature = reinterpret_cast<MByte*>(const_cast<char*>(feature2.data.data()));
    sdkFeature2.featureSize = feature2.size;

    // 调用SDK对比特征
    MFloat confidence = 0.0f;
    MRESULT result = ASFFaceFeatureCompare(
        m_engine,
        &sdkFeature1,
        &sdkFeature2,
        &confidence,
        ASF_LIFE_PHOTO  // 生活照对比模式（还有证件照模式）
    );

    if (result != MOK) {
        qWarning() << "特征对比失败，错误码：" << result;
        return 0.0f;
    }

    // confidence范围0.0-1.0，一般阈值：
    // - 0.8以上：极可能是同一人
    // - 0.6-0.8：可能是同一人
    // - 0.6以下：不太可能是同一人
    return confidence;
}
