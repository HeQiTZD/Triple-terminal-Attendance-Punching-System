#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include <QObject>
#include <QCamera>
#include <QMediaDevices>
#include <QCameraDevice>
#include <QDebug>
class CameraCapture : public QObject
{
    Q_OBJECT

public:
    CameraCapture();
    ~CameraCapture();

    bool initCamera();//初始化摄像头
    void clearCamera();//释放摄像头实例

private:
    QCamera* camera=nullptr;
    QList<QCameraDevice> cameras;

public:
    QCamera* getCamera() const;
};

#endif // CAMERACAPTURE_H
