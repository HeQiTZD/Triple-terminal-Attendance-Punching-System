#include "cameracapture.h"

CameraCapture::CameraCapture() {

    cameras=QMediaDevices::videoInputs();//扫描可用摄像头
}

CameraCapture::~CameraCapture()
{
    clearCamera();
}


//初始化摄像头
bool CameraCapture::initCamera()
{
    //是否存在可用摄像头
    if(cameras.isEmpty()){
        qDebug()<<"无可用摄像头";
        return false;
    }

    //是否存在摄像头实例
    clearCamera();

    //实例化摄像头
    const QCameraDevice firstCamera=cameras[0];
    camera=new QCamera(firstCamera);
    if(!camera){
        return false;
    }
    return true;
}

//释放摄像头实例
void CameraCapture::clearCamera()
{
    if(camera){
        if(camera->isActive()){
            camera->stop();
        }
        delete camera;
        camera=nullptr;
    }
}

//开启摄像头
void CameraCapture::startCamera()
{
    if(camera){
        if(!camera->isActive()){
            camera->start();
        }
    }
}

//关闭摄像头
void CameraCapture::stopCamera()
{
    if(camera){
        if(camera->isActive()){
            camera->stop();
        }
    }
}

//获取摄像头实例
QCamera* CameraCapture::getCamera() const
{
    return camera;
}
