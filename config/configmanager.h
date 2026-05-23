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
    int getCameraRotation() const { return m_cameraRotation; }

    // 人脸识别设置 - Setter
    void setFaceThreshold(int threshold) { m_faceThreshold = threshold; }
    void setMaxFaceCount(int count) { m_maxFaceCount = count; }
    void setRecognizeTimeout(int timeout) { m_recognizeTimeout = timeout; }
    void setCameraRotation(int degrees) { m_cameraRotation = degrees; }

    // ArcFace SDK配置 - Getter
    QString getAppId() const { return m_appId; }
    QString getSdkKey() const { return m_sdkKey; }

    // ArcFace SDK配置 - Setter
    void setAppId(const QString &appId) { m_appId = appId; }
    void setSdkKey(const QString &sdkKey) { m_sdkKey = sdkKey; }

    // 考勤规则设置 - Getter
    QTime getWorkStartTime() const { return m_workStartTime; }
    QTime getWorkEndTime() const { return m_workEndTime; }
    int getCheckInStartOffset() const { return m_checkInStartOffset; }
    int getLateAllowance() const { return m_lateAllowance; }
    int getEarlyLeaveAllowance() const { return m_earlyLeaveAllowance; }
    int getCheckOutEndOffset() const { return m_checkOutEndOffset; }
    bool isFlexibleWorkEnabled() const { return m_flexibleWorkEnabled; }
    int getFlexibleRange() const { return m_flexibleRange; }
    QTime getCoreWorkStart() const { return m_coreWorkStart; }
    QTime getCoreWorkEnd() const { return m_coreWorkEnd; }
    bool isLunchBreakEnabled() const { return m_lunchBreakEnabled; }
    QTime getLunchBreakStart() const { return m_lunchBreakStart; }
    QTime getLunchBreakEnd() const { return m_lunchBreakEnd; }
    int getMinCheckInterval() const { return m_minCheckInterval; }
    bool isCrossDayAllowed() const { return m_allowCrossDay; }
    int getMaxWorkHours() const { return m_maxWorkHours; }

    // 考勤规则设置 - Setter
    void setWorkStartTime(const QTime &time) { m_workStartTime = time; }
    void setWorkEndTime(const QTime &time) { m_workEndTime = time; }
    void setCheckInStartOffset(int minutes) { m_checkInStartOffset = minutes; }
    void setLateAllowance(int minutes) { m_lateAllowance = minutes; }
    void setEarlyLeaveAllowance(int minutes) { m_earlyLeaveAllowance = minutes; }
    void setCheckOutEndOffset(int minutes) { m_checkOutEndOffset = minutes; }
    void setFlexibleWorkEnabled(bool enabled) { m_flexibleWorkEnabled = enabled; }
    void setFlexibleRange(int minutes) { m_flexibleRange = minutes; }
    void setCoreWorkStart(const QTime &time) { m_coreWorkStart = time; }
    void setCoreWorkEnd(const QTime &time) { m_coreWorkEnd = time; }
    void setLunchBreakEnabled(bool enabled) { m_lunchBreakEnabled = enabled; }
    void setLunchBreakStart(const QTime &time) { m_lunchBreakStart = time; }
    void setLunchBreakEnd(const QTime &time) { m_lunchBreakEnd = time; }
    void setMinCheckInterval(int seconds) { m_minCheckInterval = seconds; }
    void setAllowCrossDay(bool enabled) { m_allowCrossDay = enabled; }
    void setMaxWorkHours(int hours) { m_maxWorkHours = hours; }

    // 设备信息 - Getter
    QString getDeviceId() const { return m_deviceId; }
    QString getDeviceKey() const { return m_deviceKey; }
    QString getFwVersion() const { return m_fwVersion; }
    QString getDeviceName() const { return m_deviceName; }
    QString getConfigVersion() const { return m_configVersion; }
    QString getConfigHash() const { return m_configHash; }

    // 设备信息 - Setter
    void setDeviceId(const QString &id) { m_deviceId = id; }
    void setDeviceKey(const QString &key) { m_deviceKey = key; }
    void setFwVersion(const QString &ver) { m_fwVersion = ver; }
    void setDeviceName(const QString &name) { m_deviceName = name; }

    // 同步设置 - Getter
    bool getAutoSyncOnConnect() const { return m_autoSyncOnConnect; }
    int getSyncTimeout() const { return m_syncTimeout; }

    // 同步设置 - Setter
    void setAutoSyncOnConnect(bool enabled) { m_autoSyncOnConnect = enabled; }
    void setSyncTimeout(int seconds) { m_syncTimeout = seconds; }

    // 考勤重试设置 - Getter
    int getMaxRetryCount() const { return m_maxRetryCount; }
    int getRetryBackoffBaseMs() const { return m_retryBackoffBaseMs; }

    // 考勤重试设置 - Setter
    void setMaxRetryCount(int count) { m_maxRetryCount = count; }
    void setRetryBackoffBaseMs(int ms) { m_retryBackoffBaseMs = ms; }

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

    // 从管理端接收的新配置覆盖 config.ini
    bool applyRemoteConfig(const QString &configContent,
                           const QString &configVersion,
                           const QString &configHash,
                           QString *errorMessage = nullptr);

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    QString getConfigFilePath() const;
    QString getLocalConfigFilePath() const;

private:
    static ConfigManager* s_instance;
    QSettings* m_settings;      // config.ini - 可远程覆盖的配置
    QSettings* m_localSettings; // local.ini - 本机配置

    // 网络连接设置
    QString m_serverIP;
    int m_serverPort;
    int m_connectionTimeout;

    // 人脸识别设置
    int m_faceThreshold;
    int m_maxFaceCount;
    int m_recognizeTimeout;
    int m_cameraRotation;

    // ArcFace SDK配置
    QString m_appId;
    QString m_sdkKey;

    // 考勤规则设置
    QTime m_workStartTime;
    QTime m_workEndTime;
    int m_checkInStartOffset;
    int m_lateAllowance;
    int m_earlyLeaveAllowance;
    int m_checkOutEndOffset;
    bool m_flexibleWorkEnabled;
    int m_flexibleRange;
    QTime m_coreWorkStart;
    QTime m_coreWorkEnd;
    bool m_lunchBreakEnabled;
    QTime m_lunchBreakStart;
    QTime m_lunchBreakEnd;
    int m_minCheckInterval;
    bool m_allowCrossDay;
    int m_maxWorkHours;

    // 存储设置
    QString m_databasePath;
    QString m_logPath;

    // 设备信息
    QString m_deviceId;
    QString m_deviceKey;
    QString m_fwVersion;
    QString m_deviceName;
    QString m_configVersion;
    QString m_configHash;

    // 同步设置
    bool m_autoSyncOnConnect;
    int m_syncTimeout;

    // 考勤重试设置
    int m_maxRetryCount;
    int m_retryBackoffBaseMs;

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
    static constexpr int DEFAULT_CAMERA_ROTATION = 0;
    static constexpr int DEFAULT_CHECK_IN_START_OFFSET = 120;
    static constexpr int DEFAULT_LATE_ALLOWANCE = 15;
    static constexpr int DEFAULT_EARLY_LEAVE_ALLOWANCE = 15;
    static constexpr int DEFAULT_CHECK_OUT_END_OFFSET = 180;
    static constexpr bool DEFAULT_FLEXIBLE_WORK_ENABLED = false;
    static constexpr int DEFAULT_FLEXIBLE_RANGE = 30;
    static constexpr bool DEFAULT_LUNCH_BREAK_ENABLED = true;
    static constexpr int DEFAULT_MIN_CHECK_INTERVAL = 60;
    static constexpr bool DEFAULT_ALLOW_CROSS_DAY = false;
    static constexpr int DEFAULT_MAX_WORK_HOURS = 12;
    static constexpr int DEFAULT_MAIN_WINDOW_WIDTH = 1200;
    static constexpr int DEFAULT_MAIN_WINDOW_HEIGHT = 800;
    static constexpr bool DEFAULT_AUTO_SYNC_ON_CONNECT = true;
    static constexpr int DEFAULT_SYNC_TIMEOUT = 300;
    static constexpr int DEFAULT_MAX_RETRY_COUNT = 5;
    static constexpr int DEFAULT_RETRY_BACKOFF_BASE_MS = 2000;
};

#endif // CONFIGMANAGER_H
