#include "mainwindow.h"
#include "Config/configmanager.h"
#include <QApplication>
#include <QImage>
#include <QStyleFactory>
#include <QPalette>
#include <QFile>

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

    // 设置应用样式为 Fusion，确保颜色不随系统变化
    a.setStyle(QStyleFactory::create("Fusion"));

    // 设置深色调色板，与 mainwindow.qss 深色科技风主题一致
    QPalette palette;
    palette.setColor(QPalette::Window,          QColor(0x0d, 0x11, 0x17));
    palette.setColor(QPalette::WindowText,      Qt::white);
    palette.setColor(QPalette::Base,            QColor(0x16, 0x1b, 0x22));
    palette.setColor(QPalette::AlternateBase,   QColor(0x21, 0x26, 0x2d));
    palette.setColor(QPalette::ToolTipBase,     QColor(0x16, 0x1b, 0x22));
    palette.setColor(QPalette::ToolTipText,     Qt::white);
    palette.setColor(QPalette::Text,            Qt::white);
    palette.setColor(QPalette::Button,          QColor(0x21, 0x26, 0x2d));
    palette.setColor(QPalette::ButtonText,      Qt::white);
    palette.setColor(QPalette::BrightText,      Qt::white);
    palette.setColor(QPalette::Highlight,       QColor(0x1f, 0x6f, 0xeb));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Link,            QColor(0x00, 0xd4, 0xff));
    palette.setColor(QPalette::Midlight,        QColor(0x30, 0x36, 0x3d));
    palette.setColor(QPalette::Dark,            QColor(0x0d, 0x11, 0x17));
    palette.setColor(QPalette::Mid,             QColor(0x21, 0x26, 0x2d));
    palette.setColor(QPalette::Shadow,          QColor(0x00, 0x00, 0x00));
    a.setPalette(palette);

    // 加载QSS样式文件
    QFile styleFile(":/qss/mainwindow.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString style = QLatin1String(styleFile.readAll());
        a.setStyleSheet(style);
        styleFile.close();
    }

    // 初始化配置管理器（单例，会自动加载配置）
    ConfigManager* config = ConfigManager::instance();
    
    // 如果配置中没有数据库路径，设置为默认路径
    if(config->getDatabasePath().isEmpty()){
        config->setDatabasePath(ConfigManager::getDefaultDatabasePath());
        config->saveConfig();
    }
    // 如果配置中没有日志路径，设置为默认路径
    if(config->getLogPath().isEmpty()){
        config->setLogPath(ConfigManager::getDefaultLogPath());
        config->saveConfig();
    }
    
    // 检查并创建必要的目录（数据库目录、日志目录）
    config->ensureDirectoriesExist();

    MainWindow w;
    w.show();

    return a.exec();
}
// 在 main 函数或测试代码中使用
