#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QTime>

// 应用程序配置管理类
// 使用QSettings管理配置文件的读写
// 配置文件位置：
//   Windows: %APPDATA%/AttendanceSystem/config.ini
//   Linux: ~/.config/AttendanceSystem/config.ini
class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager* instance();

    // 加载配置
    void loadConfig();
    // 保存配置
    void saveConfig();

    // 网络连接设置 - Getter
    QString getServerIP() const { return m_serverIP; }
    int getServerPort() const { return m_serverPort; }
    int getConnectionTimeout() const { return m_connectionTimeout; }

    // 网络连接设置 - Setter
    void setServerIP(const QString &ip) { m_serverIP = ip; }
    void setServerPort(int port) { m_serverPort = port; }
    void setConnectionTimeout(int timeout) { m_connectionTimeout = timeout; }

    // 人脸识别设置 - Getter
    int getFaceThreshold() const { return m_faceThreshold; }
    int getMaxFaceCount() const { return m_maxFaceCount; }
    int getRecognizeTimeout() const { return m_recognizeTimeout; }

    // 人脸识别设置 - Setter
    void setFaceThreshold(int threshold) { m_faceThreshold = threshold; }
    void setMaxFaceCount(int count) { m_maxFaceCount = count; }
    void setRecognizeTimeout(int timeout) { m_recognizeTimeout = timeout; }

    // ArcFace SDK配置 - Getter
    QString getAppId() const { return m_appId; }
    QString getSdkKey() const { return m_sdkKey; }

    // ArcFace SDK配置 - Setter
    void setAppId(const QString &appId) { m_appId = appId; }
    void setSdkKey(const QString &sdkKey) { m_sdkKey = sdkKey; }

    // 考勤规则设置 - Getter
    QTime getWorkStartTime() const { return m_workStartTime; }
    QTime getWorkEndTime() const { return m_workEndTime; }
    int getLateAllowance() const { return m_lateAllowance; }
    int getEarlyLeaveAllowance() const { return m_earlyLeaveAllowance; }

    // 考勤规则设置 - Setter
    void setWorkStartTime(const QTime &time) { m_workStartTime = time; }
    void setWorkEndTime(const QTime &time) { m_workEndTime = time; }
    void setLateAllowance(int minutes) { m_lateAllowance = minutes; }
    void setEarlyLeaveAllowance(int minutes) { m_earlyLeaveAllowance = minutes; }

    // 存储设置 - Getter
    QString getDatabasePath() const { return m_databasePath; }
    QString getLogPath() const { return m_logPath; }

    // 存储设置 - Setter
    void setDatabasePath(const QString &path) { m_databasePath = path; }
    void setLogPath(const QString &path) { m_logPath = path; }

    // 主窗口尺寸 - Getter
    int getMainWindowWidth() const { return m_mainWindowWidth; }
    int getMainWindowHeight() const { return m_mainWindowHeight; }

    // 主窗口尺寸 - Setter
    void setMainWindowWidth(int width) { m_mainWindowWidth = width; }
    void setMainWindowHeight(int height) { m_mainWindowHeight = height; }

    // 恢复所有设置为默认值
    void restoreDefaults();

    // 获取默认数据库路径
    static QString getDefaultDatabasePath();
    // 获取默认日志路径
    static QString getDefaultLogPath();
    
    // 检查并创建必要的目录（数据库目录、日志目录）
    void ensureDirectoriesExist();

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    QString getConfigFilePath() const;

private:
    static ConfigManager* s_instance;
    QSettings* m_settings;

    // 网络连接设置
    QString m_serverIP;
    int m_serverPort;
    int m_connectionTimeout;

    // 人脸识别设置
    int m_faceThreshold;
    int m_maxFaceCount;
    int m_recognizeTimeout;

    // ArcFace SDK配置
    QString m_appId;
    QString m_sdkKey;

    // 考勤规则设置
    QTime m_workStartTime;
    QTime m_workEndTime;
    int m_lateAllowance;
    int m_earlyLeaveAllowance;

    // 存储设置
    QString m_databasePath;
    QString m_logPath;

    // 主窗口尺寸
    int m_mainWindowWidth;
    int m_mainWindowHeight;

    // 默认值常量
    static constexpr const char* DEFAULT_SERVER_IP = "192.168.1.100";
    static constexpr int DEFAULT_SERVER_PORT = 8080;
    static constexpr int DEFAULT_CONNECTION_TIMEOUT = 30;
    static constexpr int DEFAULT_FACE_THRESHOLD = 80;
    static constexpr int DEFAULT_MAX_FACE_COUNT = 5;
    static constexpr int DEFAULT_RECOGNIZE_TIMEOUT = 10;
    static constexpr int DEFAULT_LATE_ALLOWANCE = 15;
    static constexpr int DEFAULT_EARLY_LEAVE_ALLOWANCE = 15;
    static constexpr int DEFAULT_MAIN_WINDOW_WIDTH = 1200;
    static constexpr int DEFAULT_MAIN_WINDOW_HEIGHT = 800;
};

#endif // CONFIGMANAGER_H
