#include "facerecognizer.h"
#include "../Attendance/AttendanceRuleEngine.h"
#include "../Config/configmanager.h"

FaceRecognizer::FaceRecognizer()
    : m_currentState(RecognitionState::IDLE)
    , m_lastRecognizedId()
    , m_recognitionTime()
    , m_detectingStartTime()
    , m_cooldownTimer(new QTimer(this))
    , m_FaceInfo()
    , m_FaceFeature()
    , m_bestMatch()
{
    m_cooldownTimer->setSingleShot(true);
    connect(m_cooldownTimer,&QTimer::timeout,this,[this](){
        if(m_currentState == RecognitionState::RECOGNIZED){
            setState(RecognitionState::LOST);
        }
    });
}

FaceRecognizer::~FaceRecognizer()
{
}

void FaceRecognizer::init()
{
    arcEngine = arcfaceengine::instance();

    ConfigManager* config = ConfigManager::instance();
    QString appid = config->getAppId();
    QString sdkKey = config->getSdkKey();

    if (appid.isEmpty() || sdkKey.isEmpty()) {
        return;
    }

    if (!arcEngine->initialize(appid, sdkKey)) {
        return;
    }

    dataBase = FaceDatabaseManager::instance();
    dataBase->loadFromDatabase();
}

void FaceRecognizer::processFrame(QImage image)
{
    // 注意：此方法持有 m_mutex 锁期间会 emit 信号（recognitionSuccess、recognitionFailed 等）。
    // 所有连接到此对象信号的槽必须使用 Qt::QueuedConnection，否则会导致死锁。
    // 当前 mainwindow.cpp 中所有相关 connect 均已使用 Qt::QueuedConnection。
    QMutexLocker locker(&m_mutex);

    if (!arcEngine || !arcEngine->isInitialized()) {
        emit faceProcessingCompleted();
        return;
    }

    if (image.isNull()) {
        emit faceProcessingCompleted();
        return;
    }

    switch (m_currentState) {
    case RecognitionState::IDLE:
        handleIdleState(image);
        break;

    case RecognitionState::DETECTING:
        handleDetectingState();
        break;

    case RecognitionState::RECOGNIZED:
        handleRecognizedState(image);
        break;

    case RecognitionState::LOST:
        handleLostState();
        break;
    }

    emit faceProcessingCompleted();
}

void FaceRecognizer::handleIdleState(QImage &image)
{
    m_FaceInfo = arcEngine->detectFace(image);

    emit faceDetected(m_FaceInfo);

    if(m_FaceInfo.isEmpty()){
        return;
    }

    setState(RecognitionState::DETECTING);
    performRecognition(image);
}

void FaceRecognizer::handleDetectingState()
{
    // 检测超时：如果在检测状态停留太久，回到 IDLE
    // 使用成员变量 m_detectingStartTime 替代 static 局部变量，受 m_mutex 保护，线程安全
    if (!m_detectingStartTime.isValid()) {
        m_detectingStartTime = QDateTime::currentDateTime();
    }

    int elapsed = m_detectingStartTime.msecsTo(QDateTime::currentDateTime());
    if (elapsed > LOST_TIMEOUT_MS) {
        setState(RecognitionState::IDLE);
        m_detectingStartTime = QDateTime();  // 重置
    }
}

void FaceRecognizer::handleRecognizedState(QImage &image)
{
    auto faceInfo = arcEngine->detectFace(image);

    emit faceDetected(faceInfo);

    if(faceInfo.isEmpty()){
        setState(RecognitionState::LOST);
    }
}

void FaceRecognizer::handleLostState()
{
    if(!m_cooldownTimer->isActive()){
        setState(RecognitionState::IDLE);
        m_lastRecognizedId.clear();
    }
}

void FaceRecognizer::performRecognition(QImage &image)
{
    m_FaceFeature = arcEngine->extractFeature(image,m_FaceInfo[0]);

    if(m_FaceFeature.data.isEmpty()){
        setState(RecognitionState::IDLE);
        return;
    }

    m_bestMatch = dataBase->findBestMatch(m_FaceFeature);

    ConfigManager* config = ConfigManager::instance();
    float threshold = config->getFaceThreshold() / 100.0f;

    if(m_bestMatch.second < threshold){
        emit recognitionFailed("未匹配到人员");
        setState(RecognitionState::IDLE);
        return;
    }

    QString employeeId = m_bestMatch.first;
    QDateTime currentCheckTime = QDateTime::currentDateTime();
    AttendanceCheckResult checkResult = AttendanceRuleEngine::instance()->evaluateWithEmployee(employeeId, currentCheckTime);
    QString status = checkResult.status;
    QString checkTime = currentCheckTime.toString("yyyy-MM-dd hh:mm:ss");

    QImage faceImage;
    if(!m_FaceInfo.isEmpty()){
        QRect faceRect = m_FaceInfo[0].rect;
        faceRect = faceRect.intersected(image.rect());
        if(!faceRect.isEmpty()){
            faceImage = image.copy(faceRect);
        }
    }

    if (!checkResult.isValid) {
        emit recognitionFailed(checkResult.message);
        setState(RecognitionState::IDLE);
        return;
    }

    if(isSamePerson(employeeId)){
        emit recognitionSuccess(employeeId, employeeId, status, checkTime, faceImage);
        setState(RecognitionState::RECOGNIZED);
        return;
    }

    emit requestSaveAttendance(employeeId,status);

    m_lastRecognizedId = employeeId;
    m_recognitionTime = QDateTime::currentDateTime();

    emit recognitionSuccess(employeeId, employeeId, status, checkTime, faceImage);

    setState(RecognitionState::RECOGNIZED);
    m_cooldownTimer->start(COOLDOWN_MS);
}

void FaceRecognizer::setState(RecognitionState newState)
{
    if(m_currentState != newState){
        m_currentState = newState;
    }
}

bool FaceRecognizer::isSamePerson(const QString &employeeId)
{
    if(m_lastRecognizedId.isEmpty()){
        return false;
    }

    if(m_lastRecognizedId == employeeId){
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
