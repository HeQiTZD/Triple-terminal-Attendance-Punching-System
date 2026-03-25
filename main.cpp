#include "mainwindow.h"
#include "Config/configmanager.h"
#include <QApplication>
#include <QImage>
#include <QStyleFactory>
#include <QPalette>

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
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(233, 233, 233));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(240, 240, 240));
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    a.setPalette(palette);

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
