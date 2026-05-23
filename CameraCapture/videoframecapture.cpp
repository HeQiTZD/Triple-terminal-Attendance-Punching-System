#include "videoframecapture.h"
#include "videoframeconverter.h"

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
    QImage image;
    if (m_converter) {
        image = m_converter->convertToQImage(frame);
    } else {
        image = VideoFrameConverter().convertToQImage(frame);
    }

    if (!image.isNull() && image.width() > 0 && image.height() > 0) {
        currentFrame = image;
        emit frameCaptured(currentFrame);
    }
}

void VideoFrameCapture::setFrameConverter(VideoFrameConverter *converter)
{
    m_converter = converter;
}
