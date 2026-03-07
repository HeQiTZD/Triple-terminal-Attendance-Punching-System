#include "videoframecapture.h"
#include "videoframeconverter.h"

#include <QWidget>
#include <QVBoxLayout>
VideoFrameCapture::VideoFrameCapture() {

}

void VideoFrameCapture::captureFrame()
{
    setCamera();
    if(camera && videoSink){
        if(!camera->isActive()){
            camera->start();
        }
    }
}

void VideoFrameCapture::setCamera()
{
    //清理之前的设置
    // if(camera){
    //     if(camera->isActive()){
    //         camera->stop();
    //     }
    //     delete camera;
    //     camera=nullptr;
    // }
    // if(captureSession){
    //     delete captureSession;
    //     captureSession=nullptr;
    // }
    // if(videoSink){
    //     delete videoSink;
    //     videoSink=nullptr;
    // }

    cameraCaptrue.initCamera();
    camera=cameraCaptrue.getCamera();
    if(camera){
        //创建媒体捕获会话
        captureSession = new QMediaCaptureSession();
        captureSession->setCamera(camera);


        //测试
        QVBoxLayout* layout=new QVBoxLayout();
        QWidget* widget=new QWidget();
        videoWidget=new QVideoWidget(widget);
        widget->setLayout(layout);
        layout->addWidget(videoWidget);
        captureSession->setVideoOutput(videoWidget);
        widget->show();

        //创建视频接收器
        videoSink = videoWidget->videoSink();
        captureSession->setVideoSink(videoSink);

        connect(videoSink,&QVideoSink::videoFrameChanged,this,&VideoFrameCapture::processFrame);
    }
}

QImage VideoFrameCapture::getCurrentFrame() const
{
    return currentFrame;
}

void VideoFrameCapture::processFrame(const QVideoFrame &frame)
{
    QImage image = VideoFrameConverter::convertToQImage(frame);
    
    if (!image.isNull() && image.width() > 0 && image.height() > 0) {
        currentFrame = image;
        emit frameCaptured(currentFrame);
    }
}
