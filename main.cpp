#include "mainwindow.h"
#include <QApplication>
#include <QImage>

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

     w.show();

    // SetWindow w;
    // w.show();

    return a.exec();
}
