#include "facerecognizer.h"
#include "../Config/configmanager.h"

FaceRecognizer::FaceRecognizer()
{
    //初始化冷却定时器
    m_cooldownTimer = new QTimer(this);
    m_cooldownTimer->setSingleShot(true);//设置定时器为单次触发模式
    connect(m_cooldownTimer,&QTimer::timeout,this,[this](){
        //冷却结束，根据当前状态执行下一步
        if(m_currentState == RecognitionState::RECOGNIZED){
            setState(RecognitionState::LOST);
        }
    });
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
    //引擎初始化
    arcEngine = arcfaceengine::instance();

    //从配置文件读取AppID和SDK Key
    ConfigManager* config = ConfigManager::instance();
    QString appid = config->getAppId();
    QString sdkKey = config->getSdkKey();

    if (!arcEngine->initialize(appid, sdkKey)) {
        qDebug() << "ArcFace 引擎初始化失败";
        return;
    }

    //加载特征到内存
    dataBase = FaceDatabaseManager::instance();
    dataBase->loadFromDatabase();
}

//主入口：状态机调度
void FaceRecognizer::WanZhengYeWuLiuCheng(QImage image)
{
    QMutexLocker locker(&m_mutex); // 保护成员变量

    // 确保图像有效
    if (image.isNull()) {
        return;
    }

    switch (m_currentState) {
    case RecognitionState::IDLE:
        handleIdleState(image);
        break;

    case RecognitionState::DETECTING:
        // 正在处理中，忽略新帧
        handleDetectingState();
        break;

    case RecognitionState::RECOGNIZED:
        // 检查人脸是否还在画面中
        handleRecognizedState(image);
        break;

    case RecognitionState::LOST:
        // 人脸已离开，准备重置
        handleLostState();
        break;
    }
}

//状态1：空闲 - 等待检测到人脸
void FaceRecognizer::handleIdleState(QImage &image)
{
    m_FaceInfo = arcEngine->detectFace(image);

    // 发射人脸检测信号
    emit faceDetected(m_FaceInfo);

    if(m_FaceInfo.isEmpty()){
        // 没有检测到人脸，保持IDLE状态
        return;
    }

    // 检测到人脸，切换到检测中状态
    qDebug() << "状态: IDLE -> DETECTING (检测到人脸)";
    setState(RecognitionState::DETECTING);

    // 执行完整识别流程
    perfromRecognition(image);
}

// 状态2：检测中 - 正在处理识别  有必要吗？
void FaceRecognizer::handleDetectingState()
{
    // 正在识别中，跳过新帧
    // 识别完成后会自动切换到RECOGNIZED状态
    qDebug() << "状态: DETECTING (识别中，跳过帧)";
}

// 状态3：已识别 - 检查人脸是否还在
void FaceRecognizer::handleRecognizedState(QImage &image)
{
    auto faceInfo = arcEngine->detectFace(image);

    // 发射人脸检测信号
    emit faceDetected(faceInfo);

    if(faceInfo.isEmpty()){
        // 人脸已离开
        qDebug() << "状态: RECOGNIZED -> LOST (人脸离开)";
        setState(RecognitionState::LOST);
    }else{
        // 人脸还在，继续冷却
        qDebug() << "状态: RECOGNIZED (人脸仍在，冷却中)";
    }
}

// 状态4：丢失等待 - 准备重置
void FaceRecognizer::handleLostState()
{
    // 检查是否可以重置（冷却已结束）
    if(!m_cooldownTimer->isActive()){
        qDebug() << "状态: LOST -> IDLE (重置完成)";
        setState(RecognitionState::IDLE);
        m_lastRecognizedId.clear();  // 清除上次识别记录
    }
}

// 执行完整识别流程
void FaceRecognizer::perfromRecognition(QImage &image)
{
    //特征提取
    m_FaceFeature = arcEngine->extractFeature(image,m_FaceInfo[0]);
    if(m_FaceFeature.data.isEmpty()){
        qWarning() << "特征提取失败";
        setState(RecognitionState::IDLE);  // 失败，回到空闲
        return;
    }

    //特征比对
    m_bestMatch = dataBase->findBestMatch(m_FaceFeature);

    //从配置文件读取相似度阈值（转换为0-1范围）
    ConfigManager* config = ConfigManager::instance();
    float threshold = config->getFaceThreshold() / 100.0f;

    //相似度检查
    if(m_bestMatch.second < threshold){
        qDebug() << "识别失败：相似度不足" << m_bestMatch.second << "阈值:" << threshold;
        emit recognitionFailed("未匹配到人员");
        setState(RecognitionState::IDLE);  // 失败，回到空闲
        return;
    }

    QString employeeId = m_bestMatch.first;
    QString status = "正常";
    QString checkTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 裁剪人脸区域图像用于显示
    QImage faceImage;
    if(!m_FaceInfo.isEmpty()){
        QRect faceRect = m_FaceInfo[0].rect;
        // 确保矩形在图像范围内
        faceRect = faceRect.intersected(image.rect());
        if(!faceRect.isEmpty()){
            faceImage = image.copy(faceRect);
        }
    }

    // 检查是否重复识别
    if(isSamePerson(employeeId)){
        qDebug() << "同一人脸，跳过重复识别：" << employeeId;
        emit recognitionSuccess(employeeId, employeeId, status, checkTime, faceImage);
        setState(RecognitionState::RECOGNIZED);
        return;
    }

    emit requestSaveAttendance(employeeId,status);

    // 更新本地状态
    m_lastRecognizedId = employeeId;
    m_recognitionTime = QDateTime::currentDateTime();

    // 发射识别成功信号（用于UI更新）
    emit recognitionSuccess(employeeId, employeeId, status, checkTime, faceImage);

    // 切换到已识别状态，启动冷却
    setState(RecognitionState::RECOGNIZED);
    m_cooldownTimer->start(COOLDOWN_MS);
}

// 切换状态
void FaceRecognizer::setState(RecognitionState newState)
{
    if(m_currentState != newState){
        m_currentState = newState;

        //状态日志
        QString stateStr;
        switch (newState) {
        case RecognitionState::IDLE: stateStr = "IDLE"; break;
        case RecognitionState::DETECTING: stateStr = "DETECTING"; break;
        case RecognitionState::RECOGNIZED: stateStr = "RECOGNIZED"; break;
        case RecognitionState::LOST: stateStr = "LOST"; break;
        }
        qDebug() << "状态切换:" << stateStr;
    }
}

// 检查是否是同一人脸（防止重复识别）
bool FaceRecognizer::isSamePerson(const QString &employeeId)
{
    // 如果没有上次记录，不是同一人
    if(m_lastRecognizedId.isEmpty()){
        return false;
    }

    //如果ID相同且冷却时间未过，认为是同一人
    if(m_lastRecognizedId == employeeId){
        //msecsTo 计算时间差
        int elapsed = m_recognitionTime.msecsTo(QDateTime::currentDateTime());
        if(elapsed < COOLDOWN_MS){
            return true;
        }
    }

    return false;
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
