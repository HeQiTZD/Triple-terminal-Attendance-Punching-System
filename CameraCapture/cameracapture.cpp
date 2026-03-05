#include "cameracapture.h"

CameraCapture::CameraCapture() {

    //扫描可用摄像头
    cameras=QMediaDevices::videoInputs();
}

CameraCapture::~CameraCapture()
{
    delete cameraWidget;
    delete camera;
    delete videoWidget;
}

void CameraCapture::initCamera()
{
    cameraWidget=new QWidget();
    QVBoxLayout* VLayout=new QVBoxLayout(cameraWidget);
    cameraWidget->setLayout(VLayout);
    //窗口属性设置
    cameraWidget->setWindowTitle("摄像头测试");
    cameraWidget->setMinimumSize(400,400);

    //扫描可用摄像头
    if(cameras.isEmpty()){
        qDebug()<<"无可用摄像头";
        return;
    }

    QCameraDevice firstCamera=cameras[0];
    camera=new QCamera(firstCamera);

     //创建捕获会话
     QMediaCaptureSession* captureSession=new QMediaCaptureSession(this);
     captureSession->setCamera(camera);

    videoWidget=new QVideoWidget(cameraWidget);
    VLayout->addWidget(videoWidget);
    videoWidget->setMinimumSize(400,400);

    captureSession->setVideoOutput(videoWidget);

    cameraWidget->show();
    camera->start();
}

// {

// }
