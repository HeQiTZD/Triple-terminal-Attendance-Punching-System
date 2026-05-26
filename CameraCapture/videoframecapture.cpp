#include "videoframecapture.h"
#include "videoframeconverter.h"
#include <QTransform>

VideoFrameCapture::VideoFrameCapture(QObject *parent)
    : QObject(parent)
{
}

void VideoFrameCapture::captureFrame(QCamera *camera)
{
    this->camera=camera;
    setCamera();
    if(camera && videoSink){
        if(!camera->isActive()){
            camera->start();
        }
    }
}

//设置捕获，接收视频帧
void VideoFrameCapture::setCamera()
{
    if(camera){
        //创建媒体捕获会话
        captureSession = new QMediaCaptureSession(this);
        captureSession->setCamera(camera);

        // 创建自定义视频控件（用于显示视频帧和人脸框）
        videoWidget=new FaceVideoWidget();

        // 创建独立的视频接收器（不再使用 setVideoOutput，避免硬件渲染层覆盖 QPainter 绘制）
        videoSink = new QVideoSink(this);
        captureSession->setVideoSink(videoSink);

        connect(videoSink,&QVideoSink::videoFrameChanged,this,&VideoFrameCapture::processFrame);
    }
}

//对外接口：已处理的视频帧
QImage VideoFrameCapture::getCurrentFrame() const
{
    return currentFrame;
}

//对外接口：图像预览窗口
QWidget *VideoFrameCapture::getVideoWidget() const
{
    return videoWidget;
}

//处理视频帧
void VideoFrameCapture::processFrame(const QVideoFrame &frame)
{
    // 帧丢弃：人脸识别线程来不及处理时跳过当前帧，防止事件队列堆积导致内存持续增长
    if (m_pendingFrames.loadRelaxed() >= kMaxPendingFrames) {
        return;
    }

    // 转换为 QImage（始终无旋转，给 FaceRecognizer 做人脸检测）
    QVideoFrame cloneFrame = frame;
    QImage rawImage = cloneFrame.toImage();

    if (rawImage.isNull() || rawImage.width() <= 0 || rawImage.height() <= 0) {
        return;
    }

    currentFrame = rawImage;
    m_pendingFrames.ref();
    emit frameCaptured(currentFrame);

    // 显示用：在 QImage 上直接旋转（避免第二次昂贵的 QVideoFrame→QImage 转换）
    QImage displayImage;
    if (m_converter && m_converter->rotation() != 0) {
        displayImage = rawImage.transformed(QTransform().rotate(m_converter->rotation()));
    } else {
        displayImage = rawImage;
    }
    emit frameForDisplay(displayImage);
}

void VideoFrameCapture::onFaceProcessingDone()
{
    m_pendingFrames.deref();
}

void VideoFrameCapture::setFrameConverter(VideoFrameConverter *converter)
{
    m_converter = converter;
}
