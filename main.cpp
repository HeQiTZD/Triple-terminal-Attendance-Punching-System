#include "mainwindow.h"
#include <QApplication>
#include <QImage>
#include "CameraCapture/videoframecapture.h"
#include "FaceRecognition/arcfaceengine.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <QObject>
#include <QTimer>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // 仅需设置 Windows 控制台编码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    QApplication a(argc, argv);
    MainWindow w;

    //w.show();

    VideoFrameCapture video1;
    arcfaceengine* engine=arcfaceengine::instance();
    video1.captureFrame();

    QString appid="JBT9EUHsd8RVuvbgwNLNFP1ezsdtsuUenhD6gjSkoKhG";
    QString Key="4szkxxMUBVRLirbAsTMzT9u2b5R9w5umHiucbPvTy91Z";

    engine->initialize(appid,Key);

    QTimer* capTimer = new QTimer();
    capTimer->setInterval(2000);
    capTimer->start();

    QObject::connect(capTimer,&QTimer::timeout,&w,[&]() {
        QImage image=video1.getCurrentFrame();
        QVector<arcfaceengine::FaceInfo> faceInfo = engine->detectFace(image);
        if(!faceInfo.isEmpty()){
        static arcfaceengine::FaceFeature feature1 = engine->extractFeature(image,faceInfo[0]);
        arcfaceengine::FaceFeature feature2 = engine->extractFeature(image,faceInfo[0]);
        engine->compareFeatures(feature1,feature2);
        }
    });

    return a.exec();
}
