#include "src/DataManager/datamanager.h"
#include "src/TcpServer/tcpserver.h"
#include "src/Controllers/networkcontroller.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //创建核心模块实例
    TcpServer tcpServer;
    DataManager dataManager;

    //初始化数据库
    const bool dbOk = dataManager.initialize("localhost","attenddance_db","root","password");
     Q_UNUSED(dbOk);

    //组装控制器（把网络事件落到数据层）
    networkcontroller networkcontroller(&tcpServer,&dataManager);

    //启动 TCP 服务
    tcpServer.startServer(8080);
    

    //UI（如果你还要 QML 主界面）
    QQmlApplicationEngine engine;
    engine.loadFromModule("AttendanceServer", "Main");


    return app.exec();
}
