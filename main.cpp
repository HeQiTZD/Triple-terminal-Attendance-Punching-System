#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStyleHints>
#include <cstdio>

#include "src/Attendance/AttendanceService.h"
#include "src/Auth/SessionManager.h"
#include "src/Config/ConfigDeployServer.h"
#include "src/Device/DeviceServer.h"
#include "src/Event/EventSubscriptionService.h"
#include "src/Face/FaceServer.h"
#include "src/Network/TcpConnectionManager.h"
#include "src/Person/PersonServer.h"
#include "src/Rbac/RbacServer.h"
#include "src/User/UserServer.h"

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("Fusion"));
    fprintf(stderr, "DEBUG: 1 QApplication creating...\n");
    fflush(stderr);

    QApplication app(argc, argv);
    fprintf(stderr, "DEBUG: 2 QApplication created\n");
    fflush(stderr);

    if (QStyleHints *hints = QGuiApplication::styleHints())
        hints->setColorScheme(Qt::ColorScheme::Light);

    QCoreApplication::setOrganizationName(QStringLiteral("AttendanceAdmin"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("local"));
    QCoreApplication::setApplicationName(QStringLiteral("AttendanceAdmin"));

    fprintf(stderr, "DEBUG: 3 Creating TcpConnectionManager...\n");
    fflush(stderr);
    auto *tcpManager = new TcpConnectionManager(&app);
    fprintf(stderr, "DEBUG: 4 TcpConnectionManager created\n");
    fflush(stderr);

    // SessionManager 作为 tcpManager 子对象，保证退出时先于 TcpConnectionManager 析构
    auto *sessionManager = new SessionManager(tcpManager);
    sessionManager->setTcpManager(tcpManager);
    fprintf(stderr, "DEBUG: 5 SessionManager created\n");
    fflush(stderr);

    auto *personServer = new PersonServer(&app);
    personServer->setTcpManager(tcpManager);
    auto *deviceServer = new DeviceServer(&app);
    deviceServer->setTcpManager(tcpManager);
    auto *configDeployServer = new ConfigDeployServer(&app);
    configDeployServer->setTcpManager(tcpManager);
    auto *attendanceService = new AttendanceService(&app);
    attendanceService->setTcpManager(tcpManager);
    auto *faceServer = new FaceServer(&app);
    faceServer->setTcpManager(tcpManager);
    auto *rbacServer = new RbacServer(&app);
    rbacServer->setTcpManager(tcpManager);
    auto *eventService = new EventSubscriptionService(&app);
    eventService->setTcpManager(tcpManager);
    auto *userServer = new UserServer(&app);
    userServer->setTcpManager(tcpManager);
    fprintf(stderr, "DEBUG: 6 All services created\n");
    fflush(stderr);

    QQmlApplicationEngine engine;
    fprintf(stderr, "DEBUG: 7 QQmlApplicationEngine created, setting properties...\n");
    fflush(stderr);

    engine.setInitialProperties({
        { QStringLiteral("sessionManager"), QVariant::fromValue(sessionManager) },
        { QStringLiteral("tcpManager"), QVariant::fromValue(tcpManager) },
        { QStringLiteral("personServer"), QVariant::fromValue(personServer) },
        { QStringLiteral("deviceServer"), QVariant::fromValue(deviceServer) },
        { QStringLiteral("configDeployServer"), QVariant::fromValue(configDeployServer) },
        { QStringLiteral("attendanceService"), QVariant::fromValue(attendanceService) },
        { QStringLiteral("faceServer"), QVariant::fromValue(faceServer) },
        { QStringLiteral("rbacServer"), QVariant::fromValue(rbacServer) },
        { QStringLiteral("eventService"), QVariant::fromValue(eventService) },
        { QStringLiteral("userServer"), QVariant::fromValue(userServer) },
    });
    fprintf(stderr, "DEBUG: 8 Properties set, loading module...\n");
    fflush(stderr);

    engine.loadFromModule("AttendanceAdmin", "Main");
    fprintf(stderr, "DEBUG: 9 Module loaded\n");
    fflush(stderr);

    if (engine.rootObjects().isEmpty()) {
        fprintf(stderr, "DEBUG: 10 FAIL - rootObjects is empty\n");
        fflush(stderr);
        return -1;
    }

    fprintf(stderr, "DEBUG: 11 Entering event loop\n");
    fflush(stderr);
    return app.exec();
}
