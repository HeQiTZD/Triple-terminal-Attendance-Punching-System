#include "src/DataManager/datamanager.h"
#include "src/TcpServer/tcpserver.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //创建核心模块实例
    TcpServer tcpServer;
    DataManager dataManager;

    // 初始化数据库（根据实际情况修改连接参数）
    // dataManager.initialize("localhost", "attendance_db", "root", "password");

    //


    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("AttendanceServer", "Main");

    return app.exec();
}
