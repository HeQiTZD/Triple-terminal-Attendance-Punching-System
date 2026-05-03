#include "src/DataManager/datamanager.h"
#include "src/DeviceMonitor/devicemonitor.h"
#include "src/ExportManager/exportmanager.h"
#include "src/FaceDataManager/facedatamanager.h"
#include "src/Services/dataservice.h"
#include "src/TestApi/testapi.h"
#include "src/SyncManager/syncmanager.h"
#include "src/TcpServer/tcpserver.h"
#include "src/AttendanceAnalyzer/attendanceanalyzer.h"
#include "src/Controllers/networkcontroller.h"
#include <QApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("AttendanceServer"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("local"));
    QCoreApplication::setApplicationName(QStringLiteral("AttendanceServer"));

    //创建核心模块实例
    TcpServer tcpServer;
    DataManager dataManager;
    DataService dataService(&dataManager);
    DeviceMonitor deviceMonitor(&tcpServer, &dataService);
    ExportManager exportManager(&dataService);
    FaceDataManager faceDataManager;
    AttendanceAnalyzer attendanceAnalyzer(&dataService);

    //初始化数据库
    const bool dbOk = dataManager.initialize("localhost","textAttendance","root","root");
     Q_UNUSED(dbOk);

    //组装控制器（把网络事件落到数据层）
    networkcontroller networkcontroller(&tcpServer, &dataService);
    SyncManager syncManager(&tcpServer, &dataService);
    TestApi testApi(&dataService, &tcpServer, &syncManager, &exportManager, &faceDataManager);

    //启动 TCP 服务
    tcpServer.startServer(8080);
    

    //UI（如果你还要 QML 主界面）
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("tcpServer", &tcpServer);
    engine.rootContext()->setContextProperty("dataManager", &dataManager);
    engine.rootContext()->setContextProperty("dataService", &dataService);
    engine.rootContext()->setContextProperty("exportManager", &exportManager);
    engine.rootContext()->setContextProperty("syncManager", &syncManager);
    engine.rootContext()->setContextProperty("faceDataManager", &faceDataManager);
    engine.rootContext()->setContextProperty("testApi", &testApi);
    engine.rootContext()->setContextProperty("attendanceAnalyzer", &attendanceAnalyzer);
    engine.loadFromModule("AttendanceServer", "Main");

    return app.exec();
}
