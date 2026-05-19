#include "configmanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

ConfigManager* ConfigManager::s_instance = nullptr;

ConfigManager* ConfigManager::instance()
{
    if (!s_instance) {
        s_instance = new ConfigManager();
    }
    return s_instance;
}

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_settings(nullptr)
{
    // 初始化默认值
    restoreDefaults();

    // 创建QSettings对象
    QString configPath = getConfigFilePath();
    QDir dir;
    QString configDir = QFileInfo(configPath).path();
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }

    m_settings = new QSettings(configPath, QSettings::IniFormat, this);
    qDebug() << "配置文件路径:" << configPath;

    // 加载配置
    loadConfig();
}

ConfigManager::~ConfigManager()
{
    if (m_settings) {
        delete m_settings;
    }
}

QString ConfigManager::getConfigFilePath() const
{
    // 使用应用程序目录下的config文件夹
    QString appDir = QCoreApplication::applicationDirPath();
    QString configDir = appDir + "/config";
    return configDir + "/config.ini";
}

void ConfigManager::loadConfig()
{
    if (!m_settings) return;

    m_settings->beginGroup("Network");
    m_serverIP = m_settings->value("ServerIP", DEFAULT_SERVER_IP).toString();
    m_serverPort = m_settings->value("ServerPort", DEFAULT_SERVER_PORT).toInt();
    m_connectionTimeout = m_settings->value("ConnectionTimeout", DEFAULT_CONNECTION_TIMEOUT).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("FaceRecognition");
    m_faceThreshold = m_settings->value("FaceThreshold", DEFAULT_FACE_THRESHOLD).toInt();
    m_maxFaceCount = m_settings->value("MaxFaceCount", DEFAULT_MAX_FACE_COUNT).toInt();
    m_recognizeTimeout = m_settings->value("RecognizeTimeout", DEFAULT_RECOGNIZE_TIMEOUT).toInt();
    m_appId = m_settings->value("AppId", "").toString();
    m_sdkKey = m_settings->value("SdkKey", "").toString();
    m_settings->endGroup();

    m_settings->beginGroup("Attendance");
    QString workStartStr = m_settings->value("WorkStartTime", "09:00").toString();
    QString workEndStr = m_settings->value("WorkEndTime", "18:00").toString();
    m_workStartTime = QTime::fromString(workStartStr, "HH:mm");
    m_workEndTime = QTime::fromString(workEndStr, "HH:mm");
    m_lateAllowance = m_settings->value("LateAllowance", DEFAULT_LATE_ALLOWANCE).toInt();
    m_earlyLeaveAllowance = m_settings->value("EarlyLeaveAllowance", DEFAULT_EARLY_LEAVE_ALLOWANCE).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("Device");
    m_deviceId = m_settings->value("DeviceId", "").toString();
    m_deviceKey = m_settings->value("DeviceKey", "").toString();
    m_fwVersion = m_settings->value("FwVersion", "1.0.0").toString();
    m_settings->endGroup();

    m_settings->beginGroup("Storage");
    m_databasePath = m_settings->value("DatabasePath", "").toString();
    m_logPath = m_settings->value("LogPath", "").toString();
    m_settings->endGroup();

    m_settings->beginGroup("MainWindow");
    m_mainWindowWidth = m_settings->value("Width", DEFAULT_MAIN_WINDOW_WIDTH).toInt();
    m_mainWindowHeight = m_settings->value("Height", DEFAULT_MAIN_WINDOW_HEIGHT).toInt();
    m_settings->endGroup();

    qDebug() << "配置加载完成";
}

void ConfigManager::saveConfig()
{
    if (!m_settings) return;

    m_settings->beginGroup("Network");
    m_settings->setValue("ServerIP", m_serverIP);
    m_settings->setValue("ServerPort", m_serverPort);
    m_settings->setValue("ConnectionTimeout", m_connectionTimeout);
    m_settings->endGroup();

    m_settings->beginGroup("FaceRecognition");
    m_settings->setValue("FaceThreshold", m_faceThreshold);
    m_settings->setValue("MaxFaceCount", m_maxFaceCount);
    m_settings->setValue("RecognizeTimeout", m_recognizeTimeout);
    m_settings->setValue("AppId", m_appId);
    m_settings->setValue("SdkKey", m_sdkKey);
    m_settings->endGroup();

    m_settings->beginGroup("Attendance");
    m_settings->setValue("WorkStartTime", m_workStartTime.toString("HH:mm"));
    m_settings->setValue("WorkEndTime", m_workEndTime.toString("HH:mm"));
    m_settings->setValue("LateAllowance", m_lateAllowance);
    m_settings->setValue("EarlyLeaveAllowance", m_earlyLeaveAllowance);
    m_settings->endGroup();

    m_settings->beginGroup("Device");
    m_settings->setValue("DeviceId", m_deviceId);
    m_settings->setValue("DeviceKey", m_deviceKey);
    m_settings->setValue("FwVersion", m_fwVersion);
    m_settings->endGroup();

    m_settings->beginGroup("Storage");
    m_settings->setValue("DatabasePath", m_databasePath);
    m_settings->setValue("LogPath", m_logPath);
    m_settings->endGroup();

    m_settings->beginGroup("MainWindow");
    m_settings->setValue("Width", m_mainWindowWidth);
    m_settings->setValue("Height", m_mainWindowHeight);
    m_settings->endGroup();

    m_settings->sync();
    qDebug() << "配置保存完成";
}

QString ConfigManager::getDefaultDatabasePath()
{
    QString appDir = QCoreApplication::applicationDirPath();
    return appDir + "/data/attendance.db";
}

QString ConfigManager::getDefaultLogPath()
{
    QString appDir = QCoreApplication::applicationDirPath();
    return appDir + "/logs";
}

void ConfigManager::restoreDefaults()
{
    // 恢复网络连接设置
    m_serverIP = DEFAULT_SERVER_IP;
    m_serverPort = DEFAULT_SERVER_PORT;
    m_connectionTimeout = DEFAULT_CONNECTION_TIMEOUT;

    // 恢复人脸识别设置
    m_faceThreshold = DEFAULT_FACE_THRESHOLD;
    m_maxFaceCount = DEFAULT_MAX_FACE_COUNT;
    m_recognizeTimeout = DEFAULT_RECOGNIZE_TIMEOUT;

    // 恢复考勤规则设置
    m_workStartTime = QTime(9, 0);
    m_workEndTime = QTime(18, 0);
    m_lateAllowance = DEFAULT_LATE_ALLOWANCE;
    m_earlyLeaveAllowance = DEFAULT_EARLY_LEAVE_ALLOWANCE;

    // 设备信息默认值
    if (m_deviceId.isEmpty()) m_deviceId = QStringLiteral("device_001");
    m_fwVersion = QStringLiteral("1.0.0");

    // 存储设置路径不清空
    // m_databasePath, m_logPath 保持当前值
}

void ConfigManager::ensureDirectoriesExist()
{
    // 检查并创建数据库目录
    if(!m_databasePath.isEmpty()){
        QFileInfo dbFileInfo(m_databasePath);
        QString dbDir = dbFileInfo.path();
        QDir dir;
        if(!dir.exists(dbDir)){
            if(dir.mkpath(dbDir)){
                qDebug() << "数据库目录创建成功:" << dbDir;
            }else{
                qDebug() << "数据库目录创建失败:" << dbDir;
            }
        }else{
            qDebug() << "数据库目录已存在:" << dbDir;
        }
    }
    
    // 检查并创建日志目录
    if(!m_logPath.isEmpty()){
        QDir dir;
        if(!dir.exists(m_logPath)){
            if(dir.mkpath(m_logPath)){
                qDebug() << "日志目录创建成功:" << m_logPath;
            }else{
                qDebug() << "日志目录创建失败:" << m_logPath;
            }
        }else{
            qDebug() << "日志目录已存在:" << m_logPath;
        }
    }
}
