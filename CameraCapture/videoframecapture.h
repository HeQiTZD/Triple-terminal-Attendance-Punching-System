#ifndef VIDEOFRAMECAPTURE_H
#define VIDEOFRAMECAPTURE_H

#include <QObject>
#include <QMediaCaptureSession>
#include <QVideoFrame>
#include <QVideoWidget>
#include <QVideoFrameFormat>
#include <QDebug>
#include <QVideoSink>

#include "cameracapture.h"

class VideoFrameCapture : public QObject
{
    Q_OBJECT

public:
    VideoFrameCapture();
    void captureFrame(QCamera *camera);

private:
    void setCamera();

public:
    QImage getCurrentFrame() const;//对外提供捕获的视频帧接口

private:
    QMediaCaptureSession *captureSession;//媒体捕获会话
    QVideoSink *videoSink;//视频接收器
    QCamera* camera;//摄像头实例
    QImage currentFrame;//当前捕获的视频帧
    QVideoWidget* videoWidget;//预览控件，返回捕获帧实例
    CameraCapture cameraCaptrue;//摄像头类的实例化

private slots:
    void processFrame(const QVideoFrame &frame);//处理捕获到的帧

public: signals:
    void frameCaptured(QImage image);//捕获成功信号

};

#endif // VIDEOFRAMECAPTURE_H
