#include <QApplication>
#include <QByteArray>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStyleHints>

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("Fusion"));

    QApplication app(argc, argv);
    if (QStyleHints *hints = QGuiApplication::styleHints())
        hints->setColorScheme(Qt::ColorScheme::Light);

    QCoreApplication::setOrganizationName(QStringLiteral("AttendanceAdmin"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("local"));
    QCoreApplication::setApplicationName(QStringLiteral("AttendanceAdmin"));

    QQmlApplicationEngine engine;
    engine.loadFromModule("AttendanceAdmin", "Main");

    return app.exec();
}
