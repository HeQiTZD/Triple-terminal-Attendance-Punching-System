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

    // 设置固定调色板，不随系统主题变化
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(240, 242, 245));
    palette.setColor(QPalette::WindowText, QColor(44, 62, 80));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(250, 251, 252));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(44, 62, 80));
    palette.setColor(QPalette::Text, QColor(44, 62, 80));
    palette.setColor(QPalette::Button, QColor(52, 152, 219));
    palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    palette.setColor(QPalette::BrightText, QColor(231, 76, 60));
    palette.setColor(QPalette::Highlight, QColor(52, 152, 219));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
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
