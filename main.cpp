#include "mainwindow.h"
#include <QApplication>
#include <QImage>
#include "CameraCapture/videoframecapture.h"
#include "FaceRecognition/arcfaceengine.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    QApplication a(argc, argv);
    MainWindow w;

    //w.show();

    VideoFrameCapture video1;
    arcfaceengine arcFaceEnging;
    video1.captureFrame();

    QString appid="JBT9EUHsd8RVuvbgwNLNFP1Qg57ZBq3vQUbhnxUPL1br";
    QString Key="SdZeXr84tegSkhumqMeP7T7z4QU5GYpYCMTX5QxDzaR";

    arcFaceEnging.initialize(appid,Key);
    arcFaceEnging.detectFace(video1.getCurrentFrame());

    return a.exec();
}
