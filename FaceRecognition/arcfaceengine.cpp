#include "arcfaceengine.h"

arcfaceengine* arcfaceengine::s_instance = nullptr;
QMutex arcfaceengine::s_mutex;

arcfaceengine::arcfaceengine() : m_engine(nullptr), m_initialized(false)
{
}

arcfaceengine::~arcfaceengine()
{
    uninitialize();
}

arcfaceengine* arcfaceengine::instance()
{
    if (!s_instance) {
        QMutexLocker locker(&s_mutex);
        if (!s_instance) {
            s_instance = new arcfaceengine();
        }
    }
    return s_instance;
}

bool arcfaceengine::initialize(const QString &appId, const QString &sdkKey)
{
    if (m_initialized) {
        return true;
    }

    MRESULT result = ASFOnlineActivation(appId.toLocal8Bit().data(), sdkKey.toLocal8Bit().data());
    if (MOK != result && MERR_ASF_ALREADY_ACTIVATED != result) {
        return false;
    }

    MInt32 combinedMask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER;

    result = ASFInitEngine(ASF_DETECT_MODE_VIDEO, ASF_OP_0_ONLY, 16, 10, combinedMask, &m_engine);
    if (result != MOK) {
        return false;
    }

    m_initialized = true;
    return true;
}

void arcfaceengine::uninitialize()
{
    if (m_initialized && m_engine) {
        ASFUninitEngine(m_engine);
        m_engine = nullptr;
        m_initialized = false;
    }
}

bool arcfaceengine::isInitialized() const
{
    return m_initialized;
}

MInt32 arcfaceengine::getPixelFormat(const QImage &image)
{
    switch (image.format()) {
    case QImage::Format_RGB888:
        return ASVL_PAF_RGB24_R8G8B8;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
        return ASVL_PAF_RGB32_R8G8B8;
    case QImage::Format_Grayscale8:
        return ASVL_PAF_GRAY;
    default:
        return ASVL_PAF_RGB24_R8G8B8;
    }
}

QVector<arcfaceengine::FaceInfo> arcfaceengine::detectFace(const QImage &image)
{
    QVector<FaceInfo> faceInfos;

    if (!m_initialized || image.isNull()) {
        return faceInfos;
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

    ASF_MultiFaceInfo detectedFaces = {0};
    MRESULT result = ASFDetectFacesEx(m_engine, &asvl, &detectedFaces, ASF_DETECT_MODEL_RGB);

    if (result != MOK) {
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

arcfaceengine::FaceFeature arcfaceengine::extractFeature(const QImage &image, const FaceInfo &faceInfo)
{
    FaceFeature feature;

    if (!m_initialized || image.isNull()) {
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
        return feature;
    }

    feature.size = sdkFeature.featureSize;
    feature.data = QByteArray(reinterpret_cast<char*>(sdkFeature.feature), sdkFeature.featureSize);

    return feature;
}

float arcfaceengine::compareFeatures(const FaceFeature &feature1, const FaceFeature &feature2)
{
    if (!m_initialized || feature1.size == 0 || feature2.size == 0) {
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
        return 0.0f;
    }

    return confidence;
}
