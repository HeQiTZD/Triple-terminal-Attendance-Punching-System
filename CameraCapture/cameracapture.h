#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include <QObject>
#include <QCamera>
#include <QVideoWidget>
#include <QMediaDevices>
#include <QWidget>
#include <QCameraDevice>
#include <QMediaCaptureSession>
#include <QVBoxLayout>
class CameraCapture : public QObject
{
    Q_OBJECT

public:
    CameraCapture();
    ~CameraCapture();

    void initCamera();//初始化摄像头

private:
    QWidget* cameraWidget;
    QCamera* camera;
    QVideoWidget* videoWidget;
    QList<QCameraDevice> cameras;
};

#endif // CAMERACAPTURE_H
