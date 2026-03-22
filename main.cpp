#include "mainwindow.h"
#include <QApplication>
#include <QImage>

#ifdef Q_OS_WIN
#include <windows.h>
#include <QObject>
#include <QTimer>
#endif

void testFaceRegistration()
{
    // 创建测试对象
    FaceRegistrationTest tester;

    // 方式1：单张图片注册
    bool result = tester.registerFace(
        "C:/Users/LENOVO/Pictures/Screenshots/text.jpg",  // 照片路径
        "10001",                         // 员工ID
        "张三",                          // 姓名
        "技术部"                         // 部门（可选）
        );

    if (result) {
        qDebug() << "注册成功！";
    } else {
        qDebug() << "注册失败:" << tester.lastError();
    }
}


int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // 仅需设置 Windows 控制台编码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    QApplication a(argc, argv);
     // MainWindow w;
     // w.show();

    testFaceRegistration();

    return a.exec();
}
// 在 main 函数或测试代码中使用
