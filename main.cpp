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

    networkcontroller networkcontroller(&tcpServer,&dataManager);
    tcpServer.startServer(8080);
    


    QQmlApplicationEngine engine;


    return app.exec();
}
