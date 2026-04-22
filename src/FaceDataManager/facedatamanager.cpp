#include "facedatamanager.h"
#include <QImage>
#include <QDebug>
#include <QFileDialog>
#include <QWidget>

// 构造函数：初始化成员变量
FaceDataManager::FaceDataManager(QObject *parent)
    : QObject(parent), m_engine(nullptr), m_initialized(false)
{
    connect(this, &FaceDataManager::engineReleased, this, &FaceDataManager::onEngineReleased);
    connect(this, &FaceDataManager::featureExtracted, this, &FaceDataManager::onFeatureExtracted);
    connect(this, &FaceDataManager::errorOccurred, this, &FaceDataManager::onErrorOccurred);
}

// 析构函数：释放引擎资源
FaceDataManager::~FaceDataManager()
{
    releaseEngine();
}

/**
 * @brief 初始化人脸识别引擎
 * @param appId 应用ID
 * @param sdkKey SDK密钥
 * @return 初始化成功返回true
 */
bool FaceDataManager::initializeEngine(const QString &appId, const QString &sdkKey)
{
    QMutexLocker locker(&m_mutex);
    if (m_initialized) return true;
    MRESULT result = ASFOnlineActivation(appId.toLocal8Bit().data(), sdkKey.toLocal8Bit().data());
    if (MOK != result && MERR_ASF_ALREADY_ACTIVATED != result) {
        emit errorOccurred(QString("激活失败，错误码：%1").arg(result));
        return false;
    }
    MInt32 combinedMask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER;
    result = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, 16, 10, combinedMask, &m_engine);
    if (result != MOK) {
        emit errorOccurred(QString("初始化失败，错误码：%1").arg(result));
        return false;
    }
    m_initialized = true;
    emit engineInitialized();
    return true;
}

/**
 * @brief 释放引擎资源
 */
void FaceDataManager::releaseEngine()
{
    QMutexLocker locker(&m_mutex);
    if (m_initialized && m_engine) {
        ASFUninitEngine(m_engine);
        m_engine = nullptr;
        m_initialized = false;
        emit engineReleased();
    }
}

/**
 * @brief 检查引擎是否已初始化
 * @return 已初始化返回true
 */
bool FaceDataManager::isEngineInitialized() const
{
    return m_initialized;
}

/**
 * @brief 检测图像中的所有人脸
 * @param image 输入图像
 * @return 检测到的人脸信息列表
 */
QVector<FaceDataManager::FaceInfo> FaceDataManager::detectFace(const QImage &image)
{
    QVector<FaceInfo> faceInfos;
    if (!m_initialized) {
        qWarning() << "引擎未初始化";
        return faceInfos;
    }
    if (image.isNull()) {
        qWarning() << "输入图像为空";
        return faceInfos;
    }
    // 图像格式转换（RGB -> BGR）
    QImage tempImage = image.convertToFormat(QImage::Format_RGB888);
    m_convertedImage = QImage(tempImage.size(), QImage::Format_RGB888);
    for (int y = 0; y < tempImage.height(); ++y) {
        const uchar* srcLine = tempImage.scanLine(y);
        uchar* dstLine = m_convertedImage.scanLine(y);
        for (int x = 0; x < tempImage.width(); ++x) {
            dstLine[x * 3 + 0] = srcLine[x * 3 + 2];
            dstLine[x * 3 + 1] = srcLine[x * 3 + 1];
            dstLine[x * 3 + 2] = srcLine[x * 3 + 0];
        }
    }
    int originalWidth = m_convertedImage.width();
    int alignedWidth = (originalWidth / 4) * 4;
    if (alignedWidth != originalWidth) {
        m_convertedImage = m_convertedImage.copy(0, 0, alignedWidth, m_convertedImage.height());
    }
    ASVLOFFSCREEN asvl = {0};
    asvl.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
    asvl.i32Width = m_convertedImage.width();
    asvl.i32Height = m_convertedImage.height();
    asvl.ppu8Plane[0] = m_convertedImage.bits();
    asvl.pi32Pitch[0] = m_convertedImage.bytesPerLine();
    ASF_MultiFaceInfo detectedFaces = {0};
    MRESULT result = ASFDetectFacesEx(m_engine, &asvl, &detectedFaces, ASF_DETECT_MODEL_RGB);
    if (result != MOK) {
        qWarning() << "人脸检测失败，错误码：" << result;
        return faceInfos;
    }
    for (MInt32 i = 0; i < detectedFaces.faceNum; ++i) {
        FaceInfo info;
        info.rect = QRect(
            detectedFaces.faceRect[i].left,
            detectedFaces.faceRect[i].top,
            detectedFaces.faceRect[i].right - detectedFaces.faceRect[i].left,
            detectedFaces.faceRect[i].bottom - detectedFaces.faceRect[i].top
        );
        info.orient = detectedFaces.faceOrient[i];
        info.faceId = detectedFaces.faceID ? detectedFaces.faceID[i] : -1;
        faceInfos.append(info);
    }
    return faceInfos;
}

/**
 * @brief 从指定人脸区域提取特征
 * @param image 输入图像
 * @param faceInfo 人脸信息
 * @return 人脸特征数据
 */
FaceDataManager::FaceFeature FaceDataManager::extractFeature(const QImage &image, const FaceInfo &faceInfo)
{
    FaceFeature feature;
    if (!m_initialized) {
        qWarning() << "引擎未初始化";
        return feature;
    }
    if (image.isNull()) {
        qWarning() << "输入图像为空";
        return feature;
    }
    QImage tempImage = image.convertToFormat(QImage::Format_RGB888);
    m_convertedImage = QImage(tempImage.size(), QImage::Format_RGB888);
    for (int y = 0; y < tempImage.height(); ++y) {
        const uchar* srcLine = tempImage.scanLine(y);
        uchar* dstLine = m_convertedImage.scanLine(y);
        for (int x = 0; x < tempImage.width(); ++x) {
            dstLine[x * 3 + 0] = srcLine[x * 3 + 2];
            dstLine[x * 3 + 1] = srcLine[x * 3 + 1];
            dstLine[x * 3 + 2] = srcLine[x * 3 + 0];
        }
    }
    int originalWidth = m_convertedImage.width();
    int alignedWidth = (originalWidth / 4) * 4;
    if (alignedWidth != originalWidth) {
        m_convertedImage = m_convertedImage.copy(0, 0, alignedWidth, m_convertedImage.height());
    }
    ASVLOFFSCREEN asvl = {0};
    asvl.u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
    asvl.i32Width = m_convertedImage.width();
    asvl.i32Height = m_convertedImage.height();
    asvl.ppu8Plane[0] = m_convertedImage.bits();
    asvl.pi32Pitch[0] = m_convertedImage.bytesPerLine();
    ASF_SingleFaceInfo singleFaceInfo = {0};
    singleFaceInfo.faceRect.left = faceInfo.rect.left();
    singleFaceInfo.faceRect.top = faceInfo.rect.top();
    singleFaceInfo.faceRect.right = faceInfo.rect.right();
    singleFaceInfo.faceRect.bottom = faceInfo.rect.bottom();
    singleFaceInfo.faceOrient = faceInfo.orient;
    ASF_FaceFeature sdkFeature = {0};
    MRESULT result = ASFFaceFeatureExtractEx(m_engine, &asvl, &singleFaceInfo, &sdkFeature);
    if (result != MOK) {
        qWarning() << "特征提取失败，错误码：" << result;
        return feature;
    }
    feature.size = sdkFeature.featureSize;
    feature.data = QByteArray(reinterpret_cast<char*>(sdkFeature.feature), sdkFeature.featureSize);
    return feature;
}

/**
 * @brief 对比两个人脸特征的相似度
 * @param feature1 第一个人脸特征
 * @param feature2 第二个人脸特征
 * @return 相似度分数（0.0-1.0）
 */
float FaceDataManager::compareFeatures(const FaceFeature &feature1, const FaceFeature &feature2) const
{
    if (!m_initialized) {
        qWarning() << "引擎未初始化";
        return 0.0f;
    }
    if (feature1.size == 0 || feature2.size == 0) {
        qWarning() << "特征数据为空";
        return 0.0f;
    }
    ASF_FaceFeature sdkFeature1 = {0};
    sdkFeature1.feature = reinterpret_cast<MByte*>(const_cast<char*>(feature1.data.data()));
    sdkFeature1.featureSize = feature1.size;
    ASF_FaceFeature sdkFeature2 = {0};
    sdkFeature2.feature = reinterpret_cast<MByte*>(const_cast<char*>(feature2.data.data()));
    sdkFeature2.featureSize = feature2.size;
    MFloat confidence = 0.0f;
    MRESULT result = ASFFaceFeatureCompare(
        m_engine,
        &sdkFeature1,
        &sdkFeature2,
        &confidence,
        ASF_LIFE_PHOTO
    );
    if (result != MOK) {
        qWarning() << "特征对比失败，错误码：" << result;
        return 0.0f;
    }
    return confidence;
}

/**
 * @brief 从图片路径提取最大人脸特征
 * @param imagePath 图片路径
 * @param featureVector 输出特征数据
 * @return 提取成功返回true
 */
bool FaceDataManager::extractFaceFeature(const QString &imagePath, QByteArray &featureVector)
{
    featureVector.clear();
    QImage image(imagePath);
    if (image.isNull()) {
        emit errorOccurred("Image load failed: " + imagePath);
        emit featureExtracted(false);
        return false;
    }
    QVector<FaceInfo> faces = detectFace(image);
    if (faces.isEmpty()) {
        emit errorOccurred("No face detected");
        emit featureExtracted(false);
        return false;
    }
    // 默认取面积最大的人脸
    const FaceInfo *maxFace = &faces[0];
    int maxArea = maxFace->rect.width() * maxFace->rect.height();
    for (const auto &f : faces) {
        int area = f.rect.width() * f.rect.height();
        if (area > maxArea) {
            maxArea = area;
            maxFace = &f;
        }
    }
    FaceFeature feat = extractFeature(image, *maxFace);
    if (feat.size == 0) {
        emit errorOccurred("Feature extraction failed");
        emit featureExtracted(false);
        return false;
    }
    featureVector = feat.data;
    emit featureExtracted(true);
    return true;
}

/**
 * @brief 对比两个人脸特征的相似度
 * @param feature1 特征1
 * @param feature2 特征2
 * @param similarity 输出相似度
 * @return 对比成功返回true
 */
bool FaceDataManager::compareFaceFeature(const QByteArray &feature1, const QByteArray &feature2, float &similarity) const
{
    FaceFeature f1, f2;
    f1.data = feature1;
    f1.size = feature1.size();
    f2.data = feature2;
    f2.size = feature2.size();
    similarity = compareFeatures(f1, f2);
    return similarity > 0.0f;
}

/**
 * @brief 判断相似度是否超过阈值
 * @param similarity 相似度
 * @param threshold 阈值（默认0.8）
 * @return 超过阈值返回true
 */
bool FaceDataManager::isSimilarityAboveThreshold(float similarity, float threshold) const
{
    return similarity >= threshold;
}

/**
 * @brief 判断是否应该更新人脸特征（相似度>=0.8）
 * @param newFeature 新特征
 * @param oldFeature 旧特征
 * @return 应该更新返回true
 */
bool FaceDataManager::shouldUpdateFaceFeature(const QByteArray &newFeature, const QByteArray &oldFeature) const
{
    float sim = 0.0f;
    compareFaceFeature(newFeature, oldFeature, sim);
    return isSimilarityAboveThreshold(sim, 0.8f);
}

/**
 * @brief 获取错误信息字符串
 * @param errCode 错误码
 * @return 错误描述
 */
QString FaceDataManager::getErrorMessage(int errCode) const
{
    return QString("Error code: %1").arg(errCode);
}

/**
 * @brief 打开文件夹选择图片，返回选中的图片路径（实现放在 cpp 中，避免在 header 中包含 QFileDialog）
 */
QString FaceDataManager::selectImageFile(QWidget *parent)
{
    const QString filter = QObject::tr("Images (*.png *.jpg *.jpeg *.bmp *.gif);;All Files (*)");
    QString file = QFileDialog::getOpenFileName(parent, tr("Select Image"), QString(), filter);
    return file;
}

void FaceDataManager::onEngineReleased()
{
    QMutexLocker locker(&m_mutex);
    m_convertedImage = QImage();
    qInfo() << "FaceDataManager engine released";
}

void FaceDataManager::onFeatureExtracted(bool success)
{
    if (success) {
        qInfo() << "FaceDataManager feature extracted successfully";
    } else {
        qWarning() << "FaceDataManager feature extraction failed";
    }
}

void FaceDataManager::onErrorOccurred(const QString &error)
{
    qWarning() << "FaceDataManager error:" << error;
}
