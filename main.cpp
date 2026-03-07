#include "mainwindow.h"
#include <QApplication>
#include "CameraCapture/videoframecapture.h"

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
    video1.captureFrame();

    return a.exec();
}
