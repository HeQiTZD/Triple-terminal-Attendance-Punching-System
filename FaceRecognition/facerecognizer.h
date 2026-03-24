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
#include <QDateTime>
#include <QTimer>

#include "../LocalStorage/localstorage.h"
#include "../NetworkClient/networkclient.h"

enum class RecognitionState{
    IDLE,      // 空闲，等待检测
    DETECTING, // 正在识别中
    RECOGNIZED,// 已识别，冷却中
    LOST       // 人脸丢失，等待重置
};

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

signals:
    // 识别成功信号 - 只通知，不保存
    void recognitionSuccess(const QString &employeeId,
                            const QString &name,
                            const QString &status,
                            const QString &checkTime,
                            const QImage &faceImage);
    // 识别失败信号
    void recognitionFailed(const QString &reason);

    //请求保存打卡记录（由主线程处理）
    void requestSaveAttendance(const QString &employeeId,const QString &status);

    // 人脸检测信号（用于绘制人脸框）
    void faceDetected(const QVector<arcfaceengine::FaceInfo> &faceInfos);

private:
    //状态处理函数
    void handleIdleState(QImage &image);
    void handleDetectingState();
    void handleRecognizedState(QImage &image);
    void handleLostState();

    //执行识别
    void perfromRecognition(QImage &image);
    //切换状态
    void setState(RecognitionState newState);
    //检查是否是同一人脸（简单实现）
    bool isSamePerson(const QString &employeeId);

private:
    arcfaceengine* arcEngine = nullptr;//人脸功能实例
    VideoFrameCapture* videoCapture = nullptr;//捕获实例,通过其他路径传入
    FaceDatabaseManager* dataBase = nullptr;//内存加载特征，特征对比实例

private:
    //状态机相关
    RecognitionState m_currentState = RecognitionState::LOST;
    QString m_lastRecognizedId;// 上次识别的人员ID
    QDateTime m_recognitionTime;// 上次识别时间
    QTimer* m_cooldownTimer = nullptr;// 冷却定时器
    const int COOLDOWN_MS = 3000;// 冷却时间3秒
    const int LOST_TIMEOUT_MS = 3000;// 人脸丢失检测间隔

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
