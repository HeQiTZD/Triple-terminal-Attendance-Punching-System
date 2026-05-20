#ifndef VIDEOFRAMECAPTURE_H
#define VIDEOFRAMECAPTURE_H

#include <QObject>
#include <QMediaCaptureSession>
#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QDebug>
#include <QVideoSink>

#include "cameracapture.h"
#include "../UI/facevideowidget.h"
class VideoFrameCapture : public QObject
{
    Q_OBJECT

public:
    explicit VideoFrameCapture(QObject *parent = nullptr);
    void captureFrame(QCamera *camera);

private:
    void setCamera();

public:
    QImage getCurrentFrame() const;//对外提供捕获的视频帧接口
    QWidget* getVideoWidget() const;//对外提供摄像头预览接口

private:
    QMediaCaptureSession *captureSession;//媒体捕获会话
    QVideoSink *videoSink;//视频接收器
    QCamera* camera;//摄像头实例
    QImage currentFrame;//当前捕获的视频帧
    FaceVideoWidget* videoWidget;//预览控件，返回捕获帧实例
    CameraCapture cameraCaptrue;//摄像头类的实例化

private slots:
    void processFrame(const QVideoFrame &frame);//处理捕获到的帧

public: signals:
    void frameCaptured(QImage image);//捕获成功信号

};

#endif // VIDEOFRAMECAPTURE_H
