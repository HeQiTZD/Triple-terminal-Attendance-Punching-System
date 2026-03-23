#include "mainwindow.h"
#include "Config/configmanager.h"
#include <QApplication>
#include <QImage>

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
