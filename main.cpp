#include "mainwindow.h"
#include <QApplication>
#include "CameraCapture/cameracapture.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    //w.show();

    CameraCapture b;
    b.initCamera();

    return a.exec();
}
