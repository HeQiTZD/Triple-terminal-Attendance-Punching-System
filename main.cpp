#include "src/DataManager/datamanager.h"
#include "src/DeviceMonitor/devicemonitor.h"
#include "src/ExportManager/exportmanager.h"
#include "src/FaceDataManager/facedatamanager.h"
#include "src/TestApi/testapi.h"
#include "src/SyncManager/syncmanager.h"
#include "src/TcpServer/tcpserver.h"
#include "src/Controllers/networkcontroller.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //创建核心模块实例
    TcpServer tcpServer;
    DataManager dataManager;
    DeviceMonitor deviceMonitor(&tcpServer,&dataManager);
    ExportManager exportManager(&dataManager);
    FaceDataManager faceDataManager;

    //初始化数据库
    const bool dbOk = dataManager.initialize("localhost","textAttendance","root","root");
     Q_UNUSED(dbOk);

    //组装控制器（把网络事件落到数据层）
    networkcontroller networkcontroller(&tcpServer,&dataManager);
    SyncManager syncManager(&tcpServer,&dataManager);
    TestApi testApi(&dataManager, &tcpServer, &syncManager, &exportManager, &faceDataManager);

    //启动 TCP 服务
    tcpServer.startServer(8080);
    

    //UI（如果你还要 QML 主界面）
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("tcpServer", &tcpServer);
    engine.rootContext()->setContextProperty("dataManager", &dataManager);
    engine.rootContext()->setContextProperty("exportManager", &exportManager);
    engine.rootContext()->setContextProperty("syncManager", &syncManager);
    engine.rootContext()->setContextProperty("faceDataManager", &faceDataManager);
    engine.rootContext()->setContextProperty("testApi", &testApi);
    engine.loadFromModule("AttendanceServer", "Main");


    // 设备上线/离线接线
    QObject::connect(&tcpServer, &TcpServer::clientConnected,
                     &deviceMonitor, &DeviceMonitor::onClientConnected);
    QObject::connect(&tcpServer, &TcpServer::clientDisconnected,
                     &deviceMonitor, &DeviceMonitor::onClientDisconnected);
    QObject::connect(&tcpServer, &TcpServer::deviceStatusReceived,
                     &deviceMonitor, &DeviceMonitor::onDeviceStatusReceived);

    return app.exec();
}
