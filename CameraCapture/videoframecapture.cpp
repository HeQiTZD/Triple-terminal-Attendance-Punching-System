#include "videoframecapture.h"
#include "videoframeconverter.h"

VideoFrameCapture::VideoFrameCapture() {

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
        captureSession = new QMediaCaptureSession();
        captureSession->setCamera(camera);

        videoWidget=new QVideoWidget();
        captureSession->setVideoOutput(videoWidget);

        //创建视频接收器
        videoSink = videoWidget->videoSink();
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
QVideoWidget *VideoFrameCapture::getVideoWidget() const
{
    return videoWidget;
}

//处理视频帧
void VideoFrameCapture::processFrame(const QVideoFrame &frame)
{
    QImage image = VideoFrameConverter::convertToQImage(frame);
    
    if (!image.isNull() && image.width() > 0 && image.height() > 0) {
        currentFrame = image;
        emit frameCaptured(currentFrame);
    }
}
