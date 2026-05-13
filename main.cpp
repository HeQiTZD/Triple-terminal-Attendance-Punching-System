#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStyleHints>

#include "src/Auth/SessionManager.h"
#include "src/Network/TcpConnectionManager.h"

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("Fusion"));

    QApplication app(argc, argv);
    if (QStyleHints *hints = QGuiApplication::styleHints())
        hints->setColorScheme(Qt::ColorScheme::Light);

    QCoreApplication::setOrganizationName(QStringLiteral("AttendanceAdmin"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("local"));
    QCoreApplication::setApplicationName(QStringLiteral("AttendanceAdmin"));

    auto *tcpManager = new TcpConnectionManager(&app);
    auto *sessionManager = new SessionManager(&app);
    sessionManager->setTcpManager(tcpManager);

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        { "sessionManager", QVariant::fromValue(sessionManager) },
        { "tcpManager", QVariant::fromValue(tcpManager) },
    });
    engine.loadFromModule("AttendanceAdmin", "Main");

    return app.exec();
}
