#include "configmanager.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QTextStream>

namespace {
QString calculateConfigHash(const QString &configContent)
{
    const QByteArray digest = QCryptographicHash::hash(configContent.toUtf8(),
                                                       QCryptographicHash::Sha256);
    return QStringLiteral("sha256:") + QString::fromLatin1(digest.toHex());
}

bool hasRequiredSections(const QSettings &settings)
{
    const QStringList groups = settings.childGroups();
    return groups.contains(QStringLiteral("FaceRecognition"))
           && groups.contains(QStringLiteral("Attendance"))
           && groups.contains(QStringLiteral("Device"));
}
}

ConfigManager* ConfigManager::instance()
{
    // C++11 保证 local static 对象的线程安全初始化
    static ConfigManager s_instance;
    return &s_instance;
}

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    // 初始化默认值
    restoreDefaults();

    // 创建QSettings对象
    QString configPath = getConfigFilePath();
    QString localConfigPath = getLocalConfigFilePath();
    QDir dir;
    QString configDir = QFileInfo(configPath).path();
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }

    // 不传 this 作为 parent，由 unique_ptr 管理生命周期
    m_settings = std::make_unique<QSettings>(configPath, QSettings::IniFormat);
    m_localSettings = std::make_unique<QSettings>(localConfigPath, QSettings::IniFormat);

    // 加载配置
    loadConfig();
}

ConfigManager::~ConfigManager() = default;

QString ConfigManager::getConfigFilePath() const
{
    // 使用应用程序目录下的config文件夹
    QString appDir = QCoreApplication::applicationDirPath();
    QString configDir = appDir + "/config";
    return configDir + "/config.ini";
}

QString ConfigManager::getLocalConfigFilePath() const
{
    // 本地配置文件路径
    QString appDir = QCoreApplication::applicationDirPath();
    QString configDir = appDir + "/config";
    return configDir + "/local.ini";
}

void ConfigManager::loadConfig()
{
    // 从 config.ini 加载可远程覆盖的配置（人脸识别、考勤规则、设备信息）
    if (m_settings) {
        m_settings->beginGroup("FaceRecognition");
        m_faceThreshold = m_settings->value("FaceThreshold", DEFAULT_FACE_THRESHOLD).toInt();
        m_maxFaceCount = m_settings->value("MaxFaceCount", DEFAULT_MAX_FACE_COUNT).toInt();
        m_recognizeTimeout = m_settings->value("RecognizeTimeout", DEFAULT_RECOGNIZE_TIMEOUT).toInt();
        m_cameraRotation = m_settings->value("CameraRotation", DEFAULT_CAMERA_ROTATION).toInt();
        m_appId = m_settings->value("AppId", "").toString();
        m_sdkKey = m_settings->value("SdkKey", "").toString();
        m_settings->endGroup();

        m_settings->beginGroup("Attendance");
        QString workStartStr = m_settings->value("WorkStartTime", "09:00").toString();
        QString workEndStr = m_settings->value("WorkEndTime", "18:00").toString();
        m_workStartTime = QTime::fromString(workStartStr, "HH:mm");
        m_workEndTime = QTime::fromString(workEndStr, "HH:mm");
        m_checkInStartOffset = m_settings->value("CheckInStartOffset", DEFAULT_CHECK_IN_START_OFFSET).toInt();
        m_lateAllowance = m_settings->value("LateAllowance", DEFAULT_LATE_ALLOWANCE).toInt();
        m_earlyLeaveAllowance = m_settings->value("EarlyLeaveAllowance", DEFAULT_EARLY_LEAVE_ALLOWANCE).toInt();
        m_checkOutEndOffset = m_settings->value("CheckOutEndOffset", DEFAULT_CHECK_OUT_END_OFFSET).toInt();
        m_flexibleWorkEnabled = m_settings->value("FlexibleWorkEnabled", DEFAULT_FLEXIBLE_WORK_ENABLED).toBool();
        m_flexibleRange = m_settings->value("FlexibleRange", DEFAULT_FLEXIBLE_RANGE).toInt();
        m_coreWorkStart = QTime::fromString(m_settings->value("CoreWorkStart", "10:00").toString(), "HH:mm");
        m_coreWorkEnd = QTime::fromString(m_settings->value("CoreWorkEnd", "16:00").toString(), "HH:mm");
        m_lunchBreakEnabled = m_settings->value("LunchBreakEnabled", DEFAULT_LUNCH_BREAK_ENABLED).toBool();
        m_lunchBreakStart = QTime::fromString(m_settings->value("LunchBreakStart", "12:00").toString(), "HH:mm");
        m_lunchBreakEnd = QTime::fromString(m_settings->value("LunchBreakEnd", "13:00").toString(), "HH:mm");
        m_minCheckInterval = m_settings->value("MinCheckInterval", DEFAULT_MIN_CHECK_INTERVAL).toInt();
        m_allowCrossDay = m_settings->value("AllowCrossDay", DEFAULT_ALLOW_CROSS_DAY).toBool();
        m_maxWorkHours = m_settings->value("MaxWorkHours", DEFAULT_MAX_WORK_HOURS).toInt();
        m_maxRetryCount = m_settings->value("MaxRetryCount", DEFAULT_MAX_RETRY_COUNT).toInt();
        m_retryBackoffBaseMs = m_settings->value("RetryBackoffBaseMs", DEFAULT_RETRY_BACKOFF_BASE_MS).toInt();
        m_settings->endGroup();

        m_settings->beginGroup("Device");
        m_deviceId = m_settings->value("DeviceId", "").toString();
        m_deviceKey = m_settings->value("DeviceKey", "").toString();
        m_fwVersion = m_settings->value("FwVersion", "1.0.0").toString();
        m_deviceName = m_settings->value("DeviceName", "").toString();
        m_configVersion = m_settings->value("ConfigVersion", "").toString();
        m_configHash = m_settings->value("ConfigHash", "").toString();
        m_settings->endGroup();
    }

    // 从 local.ini 加载本机配置（网络、同步、存储、窗口）
    if (m_localSettings) {
        m_localSettings->beginGroup("Network");
        m_serverIP = m_localSettings->value("ServerIP", DEFAULT_SERVER_IP).toString();
        m_serverPort = m_localSettings->value("ServerPort", DEFAULT_SERVER_PORT).toInt();
        m_connectionTimeout = m_localSettings->value("ConnectionTimeout", DEFAULT_CONNECTION_TIMEOUT).toInt();
        m_localSettings->endGroup();

        m_localSettings->beginGroup("Sync");
        m_autoSyncOnConnect = m_localSettings->value("AutoSyncOnConnect", DEFAULT_AUTO_SYNC_ON_CONNECT).toBool();
        m_syncTimeout = m_localSettings->value("SyncTimeout", DEFAULT_SYNC_TIMEOUT).toInt();
        m_localSettings->endGroup();

        m_localSettings->beginGroup("Storage");
        m_databasePath = m_localSettings->value("DatabasePath", "").toString();
        m_logPath = m_localSettings->value("LogPath", "").toString();
        m_localSettings->endGroup();

        m_localSettings->beginGroup("MainWindow");
        m_mainWindowWidth = m_localSettings->value("Width", DEFAULT_MAIN_WINDOW_WIDTH).toInt();
        m_mainWindowHeight = m_localSettings->value("Height", DEFAULT_MAIN_WINDOW_HEIGHT).toInt();
        m_localSettings->endGroup();
    }
}

void ConfigManager::saveConfig()
{
    // 保存可远程覆盖的配置到 config.ini（人脸识别、考勤规则、设备信息）
    if (m_settings) {
        m_settings->beginGroup("FaceRecognition");
        m_settings->setValue("FaceThreshold", m_faceThreshold);
        m_settings->setValue("MaxFaceCount", m_maxFaceCount);
        m_settings->setValue("RecognizeTimeout", m_recognizeTimeout);
        m_settings->setValue("CameraRotation", m_cameraRotation);
        m_settings->setValue("AppId", m_appId);
        m_settings->setValue("SdkKey", m_sdkKey);
        m_settings->endGroup();

        m_settings->beginGroup("Attendance");
        m_settings->setValue("WorkStartTime", m_workStartTime.toString("HH:mm"));
        m_settings->setValue("WorkEndTime", m_workEndTime.toString("HH:mm"));
        m_settings->setValue("CheckInStartOffset", m_checkInStartOffset);
        m_settings->setValue("LateAllowance", m_lateAllowance);
        m_settings->setValue("EarlyLeaveAllowance", m_earlyLeaveAllowance);
        m_settings->setValue("CheckOutEndOffset", m_checkOutEndOffset);
        m_settings->setValue("FlexibleWorkEnabled", m_flexibleWorkEnabled);
        m_settings->setValue("FlexibleRange", m_flexibleRange);
        m_settings->setValue("CoreWorkStart", m_coreWorkStart.toString("HH:mm"));
        m_settings->setValue("CoreWorkEnd", m_coreWorkEnd.toString("HH:mm"));
        m_settings->setValue("LunchBreakEnabled", m_lunchBreakEnabled);
        m_settings->setValue("LunchBreakStart", m_lunchBreakStart.toString("HH:mm"));
        m_settings->setValue("LunchBreakEnd", m_lunchBreakEnd.toString("HH:mm"));
        m_settings->setValue("MinCheckInterval", m_minCheckInterval);
        m_settings->setValue("AllowCrossDay", m_allowCrossDay);
        m_settings->setValue("MaxWorkHours", m_maxWorkHours);
        m_settings->setValue("MaxRetryCount", m_maxRetryCount);
        m_settings->setValue("RetryBackoffBaseMs", m_retryBackoffBaseMs);
        m_settings->endGroup();

        m_settings->beginGroup("Device");
        m_settings->setValue("DeviceId", m_deviceId);
        m_settings->setValue("DeviceKey", m_deviceKey);
        m_settings->setValue("FwVersion", m_fwVersion);
        m_settings->setValue("DeviceName", m_deviceName);
        m_settings->setValue("ConfigVersion", m_configVersion);
        m_settings->setValue("ConfigHash", m_configHash);
        m_settings->endGroup();

        m_settings->sync();
    }

    // 保存本机配置到 local.ini（网络、同步、存储、窗口）
    if (m_localSettings) {
        m_localSettings->beginGroup("Network");
        m_localSettings->setValue("ServerIP", m_serverIP);
        m_localSettings->setValue("ServerPort", m_serverPort);
        m_localSettings->setValue("ConnectionTimeout", m_connectionTimeout);
        m_localSettings->endGroup();

        m_localSettings->beginGroup("Sync");
        m_localSettings->setValue("AutoSyncOnConnect", m_autoSyncOnConnect);
        m_localSettings->setValue("SyncTimeout", m_syncTimeout);
        m_localSettings->endGroup();

        m_localSettings->beginGroup("Storage");
        m_localSettings->setValue("DatabasePath", m_databasePath);
        m_localSettings->setValue("LogPath", m_logPath);
        m_localSettings->endGroup();

        m_localSettings->beginGroup("MainWindow");
        m_localSettings->setValue("Width", m_mainWindowWidth);
        m_localSettings->setValue("Height", m_mainWindowHeight);
        m_localSettings->endGroup();

        m_localSettings->sync();
    }
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
    restoreNetworkDefaults();
    restoreFaceRecognitionDefaults();
    restoreAttendanceDefaults();
    restoreDeviceDefaults();
    restoreSyncDefaults();
    restoreRetryDefaults();
}

void ConfigManager::restoreNetworkDefaults()
{
    m_serverIP = DEFAULT_SERVER_IP;
    m_serverPort = DEFAULT_SERVER_PORT;
    m_connectionTimeout = DEFAULT_CONNECTION_TIMEOUT;
}

void ConfigManager::restoreFaceRecognitionDefaults()
{
    m_faceThreshold = DEFAULT_FACE_THRESHOLD;
    m_maxFaceCount = DEFAULT_MAX_FACE_COUNT;
    m_recognizeTimeout = DEFAULT_RECOGNIZE_TIMEOUT;
    m_cameraRotation = DEFAULT_CAMERA_ROTATION;
}

void ConfigManager::restoreAttendanceDefaults()
{
    m_workStartTime = QTime(9, 0);
    m_workEndTime = QTime(18, 0);
    m_checkInStartOffset = DEFAULT_CHECK_IN_START_OFFSET;
    m_lateAllowance = DEFAULT_LATE_ALLOWANCE;
    m_earlyLeaveAllowance = DEFAULT_EARLY_LEAVE_ALLOWANCE;
    m_checkOutEndOffset = DEFAULT_CHECK_OUT_END_OFFSET;
    m_flexibleWorkEnabled = DEFAULT_FLEXIBLE_WORK_ENABLED;
    m_flexibleRange = DEFAULT_FLEXIBLE_RANGE;
    m_coreWorkStart = QTime(10, 0);
    m_coreWorkEnd = QTime(16, 0);
    m_lunchBreakEnabled = DEFAULT_LUNCH_BREAK_ENABLED;
    m_lunchBreakStart = QTime(12, 0);
    m_lunchBreakEnd = QTime(13, 0);
    m_minCheckInterval = DEFAULT_MIN_CHECK_INTERVAL;
    m_allowCrossDay = DEFAULT_ALLOW_CROSS_DAY;
    m_maxWorkHours = DEFAULT_MAX_WORK_HOURS;
}

void ConfigManager::restoreDeviceDefaults()
{
    if (m_deviceId.isEmpty()) m_deviceId = QStringLiteral("device_001");
    m_deviceName = QString();
    m_fwVersion = QStringLiteral("1.0.0");
    m_configVersion = QString();
    m_configHash = QString();
}

void ConfigManager::restoreSyncDefaults()
{
    m_autoSyncOnConnect = DEFAULT_AUTO_SYNC_ON_CONNECT;
    m_syncTimeout = DEFAULT_SYNC_TIMEOUT;
}

void ConfigManager::restoreRetryDefaults()
{
    m_maxRetryCount = DEFAULT_MAX_RETRY_COUNT;
    m_retryBackoffBaseMs = DEFAULT_RETRY_BACKOFF_BASE_MS;
}

void ConfigManager::ensureDirectoriesExist()
{
    // 检查并创建数据库目录
    if(!m_databasePath.isEmpty()){
        QFileInfo dbFileInfo(m_databasePath);
        QString dbDir = dbFileInfo.path();
        QDir dir;
        if(!dir.exists(dbDir)){
            dir.mkpath(dbDir);
        }
    }

    // 检查并创建日志目录
    if(!m_logPath.isEmpty()){
        QDir dir;
        if(!dir.exists(m_logPath)){
            dir.mkpath(m_logPath);
        }
    }
}

bool ConfigManager::applyRemoteConfig(const QString &configContent,
                                      const QString &configVersion,
                                      const QString &configHash,
                                      QString *errorMessage)
{
    auto fail = [errorMessage](const QString &message) {
        if (errorMessage) {
            *errorMessage = message;
        }
        return false;
    };

    if (!m_settings) {
        return fail(QStringLiteral("config manager not initialized"));
    }

    if (configVersion.trimmed().isEmpty()) {
        return fail(QStringLiteral("missing config version"));
    }

    if (configHash.trimmed().isEmpty()) {
        return fail(QStringLiteral("missing config hash"));
    }

    const QString calculatedHash = calculateConfigHash(configContent);
    if (calculatedHash.compare(configHash.trimmed(), Qt::CaseInsensitive) != 0) {
        return fail(QStringLiteral("config hash mismatch"));
    }

    // 获取配置文件路径
    QString configPath = getConfigFilePath();

    // 将接收到的配置内容写入临时文件
    QString tempPath = configPath + ".tmp";
    QFile tempFile(tempPath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return fail(QStringLiteral("unable to create temporary config file"));
    }

    QTextStream out(&tempFile);
    out << configContent;
    tempFile.close();

    // 验证临时配置文件是否有效
    QSettings tempSettings(tempPath, QSettings::IniFormat, this);
    if (tempSettings.status() != QSettings::NoError) {
        QFile::remove(tempPath);
        return fail(QStringLiteral("invalid config file format"));
    }

    if (!hasRequiredSections(tempSettings)) {
        QFile::remove(tempPath);
        return fail(QStringLiteral("missing required config sections"));
    }

    // 备份原配置文件
    QString backupPath = configPath + ".bak";
    if (QFile::exists(configPath)) {
        QFile::remove(backupPath);
        if (!QFile::copy(configPath, backupPath)) {
            QFile::remove(tempPath);
            return fail(QStringLiteral("failed to backup original config file"));
        }
    }

    // 用新配置覆盖原文件
    m_settings.reset();

    auto restoreSettings = [this, &configPath]() {
        if (!m_settings) {
            m_settings = std::make_unique<QSettings>(configPath, QSettings::IniFormat);
            loadConfig();
        }
    };

    if (QFile::exists(configPath) && !QFile::remove(configPath)) {
        // 恢复备份
        if (QFile::exists(backupPath)) {
            QFile::copy(backupPath, configPath);
        }
        QFile::remove(tempPath);
        restoreSettings();
        return fail(QStringLiteral("failed to remove original config file"));
    }

    if (!QFile::rename(tempPath, configPath)) {
        // 恢复备份
        if (QFile::exists(backupPath)) {
            QFile::copy(backupPath, configPath);
        }
        QFile::remove(tempPath);
        restoreSettings();
        return fail(QStringLiteral("failed to replace config file"));
    }

    // 重新加载配置
    m_settings = std::make_unique<QSettings>(configPath, QSettings::IniFormat);
    loadConfig();

    m_configVersion = configVersion.trimmed();
    m_configHash = configHash.trimmed();
    m_settings->beginGroup(QStringLiteral("Device"));
    m_settings->setValue(QStringLiteral("ConfigVersion"), m_configVersion);
    m_settings->setValue(QStringLiteral("ConfigHash"), m_configHash);
    m_settings->endGroup();
    m_settings->sync();

    return true;
}
