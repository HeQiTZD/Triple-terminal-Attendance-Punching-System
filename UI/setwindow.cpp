#include "setwindow.h"
#include "ui_setwindow.h"
#include "../CameraCapture/videoframeconverter.h"
#include "../Config/configmanager.h"
#include "../NetworkClient/networkclient.h"
#include "UI/Theme/ThemeManager.h"
#include "UI/Theme/DesignTokens.h"
#include <QMessageBox>
#include <QTcpSocket>
#include <QTimer>
#include <QElapsedTimer>

SetWindow::SetWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)  // 使用 Qt::Window 标志，使其成为独立窗口
    , ui(new Ui::SetWindow)
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle("设置");

    setupConnections();
    loadFromConfig();

    // ---- 应用 ActionButton 主题样式 ----
    auto applyButtonTheme = [](QPushButton* btn, const QString& bgHex, const QString& fgHex,
                                const QString& borderHex, const QString& hoverBgHex,
                                const QString& activeBgHex, bool hasBorder) {
        auto* tm = ThemeManager::instance();
        int r = tm->radius("md");
        btn->setStyleSheet(QString(
            "QPushButton {"
            "  background-color: %1; color: %2;"
            "  border: %3; border-radius: %4px;"
            "  padding: 6px 16px; font-size: 13px; font-weight: 600;"
            "  min-height: 28px;"
            "}"
            "QPushButton:hover { background-color: %5; }"
            "QPushButton:pressed { background-color: %6; }"
            "QPushButton:disabled { opacity: 0.5; }"
        ).arg(bgHex).arg(fgHex)
         .arg(hasBorder ? QString("1px solid %1").arg(borderHex) : QString("none"))
         .arg(r).arg(hoverBgHex).arg(activeBgHex));
    };

    auto* tm = ThemeManager::instance();

    // 连接服务器按钮 → Primary
    applyButtonTheme(ui->btnTestConnection,
        tm->colorHex(DesignTokens::Semantic::brandPrimary),
        "#ffffff",
        "",
        tm->colorHex(DesignTokens::Semantic::brandPrimaryHover),
        tm->colorHex(DesignTokens::Semantic::brandPrimaryActive),
        false);

    // 断开连接按钮 → Danger
    applyButtonTheme(ui->btnDisconnect,
        "transparent",
        tm->colorHex(DesignTokens::Semantic::semDanger),
        tm->colorHex(DesignTokens::Semantic::semDanger),
        QString("rgba(220,38,38,0.15)"),
        QString("rgba(220,38,38,0.25)"),
        true);

    // 保存设置按钮 → Primary
    applyButtonTheme(ui->btnSave,
        tm->colorHex(DesignTokens::Semantic::brandPrimary),
        "#ffffff",
        "",
        tm->colorHex(DesignTokens::Semantic::brandPrimaryHover),
        tm->colorHex(DesignTokens::Semantic::brandPrimaryActive),
        false);

    // 取消按钮 → Secondary
    applyButtonTheme(ui->btnCancel,
        tm->colorHex(DesignTokens::Semantic::bgElevated),
        tm->colorHex(DesignTokens::Semantic::textPrimary),
        tm->colorHex(DesignTokens::Semantic::borderDefault),
        ThemeManager::lighten(tm->color(DesignTokens::Semantic::bgElevated), 0.10f).name(),
        ThemeManager::darken(tm->color(DesignTokens::Semantic::bgElevated), 0.10f).name(),
        true);

    // 恢复默认按钮 → Secondary
    applyButtonTheme(ui->btnRestore,
        tm->colorHex(DesignTokens::Semantic::bgElevated),
        tm->colorHex(DesignTokens::Semantic::textPrimary),
        tm->colorHex(DesignTokens::Semantic::borderDefault),
        ThemeManager::lighten(tm->color(DesignTokens::Semantic::bgElevated), 0.10f).name(),
        ThemeManager::darken(tm->color(DesignTokens::Semantic::bgElevated), 0.10f).name(),
        true);

    // 旋转按钮 → Primary
    applyButtonTheme(ui->btnRotate,
        tm->colorHex(DesignTokens::Semantic::brandPrimary),
        "#ffffff",
        "",
        tm->colorHex(DesignTokens::Semantic::brandPrimaryHover),
        tm->colorHex(DesignTokens::Semantic::brandPrimaryActive),
        false);
}

SetWindow::~SetWindow()
{
    delete ui;
}

void SetWindow::setupConnections()
{
    // 导航按钮信号连接
    connect(ui->btnNetwork, &QPushButton::clicked, this, &SetWindow::onNavButtonClicked);
    connect(ui->btnFace, &QPushButton::clicked, this, &SetWindow::onNavButtonClicked);
    connect(ui->btnStorage, &QPushButton::clicked, this, &SetWindow::onNavButtonClicked);
    connect(ui->btnDevice, &QPushButton::clicked, this, &SetWindow::onNavButtonClicked);
    connect(ui->btnSync, &QPushButton::clicked, this, &SetWindow::onNavButtonClicked);

    // 功能按钮信号连接
    connect(ui->btnRestore, &QPushButton::clicked, this, &SetWindow::onBtnRestoreClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SetWindow::onBtnCancelClicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &SetWindow::onBtnSaveClicked);

    // 阈值滑块信号连接
    connect(ui->sliderThreshold, &QSlider::valueChanged, this, &SetWindow::onSliderThresholdChanged);
    
    // 连接测试按钮信号连接
    connect(ui->btnTestConnection, &QPushButton::clicked, this, &SetWindow::onBtnTestConnectionClicked);
    connect(ui->btnDisconnect, &QPushButton::clicked, this, &SetWindow::onBtnDisconnectClicked);
    connect(ui->btnRotate, &QPushButton::clicked, this, &SetWindow::onBtnRotateClicked);
}

void SetWindow::onNavButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    if (btn == ui->btnNetwork) {
        switchToPage(0);
    } else if (btn == ui->btnFace) {
        switchToPage(1);
    } else if (btn == ui->btnStorage) {
        switchToPage(2);
    } else if (btn == ui->btnDevice) {
        switchToPage(3);
    } else if (btn == ui->btnSync) {
        switchToPage(4);
    }
}

void SetWindow::switchToPage(int index)
{
    ui->stackedWidget->setCurrentIndex(index);

    // 更新按钮选中状态
    ui->btnNetwork->setChecked(index == 0);
    ui->btnFace->setChecked(index == 1);
    ui->btnStorage->setChecked(index == 2);
    ui->btnDevice->setChecked(index == 3);
    ui->btnSync->setChecked(index == 4);
}

void SetWindow::onBtnRestoreClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "恢复默认",
        "确定要恢复所有设置为默认值吗？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        restoreDefaults();
    }
}

void SetWindow::onBtnCancelClicked()
{
    // 重新加载当前设置到UI，放弃修改
    saveToUI();
    this->close();
}

void SetWindow::onBtnSaveClicked()
{
    // 从UI加载值到成员变量
    loadFromUI();

    // 保存到配置文件
    saveToConfig();

    QMessageBox::information(this, "保存成功", "设置已保存！");
    this->close();
}

void SetWindow::onSliderThresholdChanged(int value)
{
    ui->labelThresholdValue->setText(QString::number(value) + "%");
}

void SetWindow::restoreDefaults()
{
    // 恢复网络连接设置
    m_serverIP = DEFAULT_SERVER_IP;
    m_serverPort = DEFAULT_SERVER_PORT;
    m_connectionTimeout = DEFAULT_CONNECTION_TIMEOUT;

    // 恢复人脸识别设置
    m_faceThreshold = DEFAULT_FACE_THRESHOLD;
    m_maxFaceCount = DEFAULT_MAX_FACE_COUNT;
    m_recognizeTimeout = DEFAULT_RECOGNIZE_TIMEOUT;

    // 恢复存储设置（路径不清空）
    // m_databasePath, m_logPath 保持当前值

    // 恢复设备信息
    m_deviceId = QString();
    m_deviceKey = QString();
    m_fwVersion = QStringLiteral("1.0.0");
    m_deviceName = QString();

    // 恢复同步设置
    m_autoSyncOnConnect = true;
    m_syncTimeout = 300;
    m_cameraRotation = 0;

    // 更新UI显示
    saveToUI();
}

// 网络连接设置 - Getter
QString SetWindow::getServerIP() const
{
    return m_serverIP;
}

int SetWindow::getServerPort() const
{
    return m_serverPort;
}

int SetWindow::getConnectionTimeout() const
{
    return m_connectionTimeout;
}

// 网络连接设置 - Setter
void SetWindow::setServerIP(const QString &ip)
{
    m_serverIP = ip;
    ui->lineEditIP->setText(ip);
}

void SetWindow::setServerPort(int port)
{
    m_serverPort = port;
    ui->spinBoxPort->setValue(port);
}

void SetWindow::setConnectionTimeout(int timeout)
{
    m_connectionTimeout = timeout;
    ui->spinBoxTimeout->setValue(timeout);
}

// 人脸识别设置 - Getter
int SetWindow::getFaceThreshold() const
{
    return m_faceThreshold;
}

int SetWindow::getMaxFaceCount() const
{
    return m_maxFaceCount;
}

int SetWindow::getRecognizeTimeout() const
{
    return m_recognizeTimeout;
}

// 人脸识别设置 - Setter
void SetWindow::setFaceThreshold(int threshold)
{
    m_faceThreshold = threshold;
    ui->sliderThreshold->setValue(threshold);
    ui->labelThresholdValue->setText(QString::number(threshold) + "%");
}

void SetWindow::setMaxFaceCount(int count)
{
    m_maxFaceCount = count;
    ui->spinBoxMaxFace->setValue(count);
}

void SetWindow::setRecognizeTimeout(int timeout)
{
    m_recognizeTimeout = timeout;
    ui->spinBoxRecognizeTimeout->setValue(timeout);
}

// ArcFace SDK配置 - Getter
QString SetWindow::getAppId() const
{
    return m_appId;
}

QString SetWindow::getSdkKey() const
{
    return m_sdkKey;
}

// ArcFace SDK配置 - Setter
void SetWindow::setAppId(const QString &appId)
{
    m_appId = appId;
    ui->lineEditAppId->setText(appId);
}

void SetWindow::setSdkKey(const QString &sdkKey)
{
    m_sdkKey = sdkKey;
    ui->lineEditSdkKey->setText(sdkKey);
}

// 存储设置 - Getter
QString SetWindow::getDatabasePath() const
{
    return m_databasePath;
}

QString SetWindow::getLogPath() const
{
    return m_logPath;
}



// 存储设置 - Setter
void SetWindow::setDatabasePath(const QString &path)
{
    m_databasePath = path;
    ui->lineEditDbPath->setText(path);
}

void SetWindow::setLogPath(const QString &path)
{
    m_logPath = path;
    ui->lineEditLogPath->setText(path);
}



// 从UI控件加载值到成员变量
void SetWindow::loadFromUI()
{
    // 网络连接设置
    m_serverIP = ui->lineEditIP->text();
    m_serverPort = ui->spinBoxPort->value();
    m_connectionTimeout = ui->spinBoxTimeout->value();

    // 人脸识别设置
    m_faceThreshold = ui->sliderThreshold->value();
    m_maxFaceCount = ui->spinBoxMaxFace->value();
    m_recognizeTimeout = ui->spinBoxRecognizeTimeout->value();

    // ArcFace SDK配置
    m_appId = ui->lineEditAppId->text();
    m_sdkKey = ui->lineEditSdkKey->text();

    // 存储设置
    m_databasePath = ui->lineEditDbPath->text();
    m_logPath = ui->lineEditLogPath->text();

    // 设备信息
    m_deviceId = ui->lineEditDeviceId->text();
    m_deviceKey = ui->lineEditDeviceKey->text();
    m_fwVersion = ui->lineEditFwVersion->text();
    m_deviceName = ui->lineEditDeviceName->text();

    // 同步设置
    m_autoSyncOnConnect = ui->checkBoxAutoSync->isChecked();
    m_syncTimeout = ui->spinBoxSyncTimeout->value();
}

// 将成员变量值设置到UI控件
void SetWindow::saveToUI()
{
    // 网络连接设置
    ui->lineEditIP->setText(m_serverIP);
    ui->spinBoxPort->setValue(m_serverPort);
    ui->spinBoxTimeout->setValue(m_connectionTimeout);

    // 人脸识别设置
    ui->sliderThreshold->setValue(m_faceThreshold);
    ui->labelThresholdValue->setText(QString::number(m_faceThreshold) + "%");
    ui->spinBoxMaxFace->setValue(m_maxFaceCount);
    ui->spinBoxRecognizeTimeout->setValue(m_recognizeTimeout);

    // ArcFace SDK配置
    ui->lineEditAppId->setText(m_appId);
    ui->lineEditSdkKey->setText(m_sdkKey);

    // 存储设置
    ui->lineEditDbPath->setText(m_databasePath);
    ui->lineEditLogPath->setText(m_logPath);

    // 设备信息
    ui->lineEditDeviceId->setText(m_deviceId);
    ui->lineEditDeviceKey->setText(m_deviceKey);
    ui->lineEditFwVersion->setText(m_fwVersion);
    ui->lineEditDeviceName->setText(m_deviceName);

    // 同步设置
    ui->checkBoxAutoSync->setChecked(m_autoSyncOnConnect);
    ui->spinBoxSyncTimeout->setValue(m_syncTimeout);

    updateRotationDisplay();
}

// 从配置文件加载设置
void SetWindow::loadFromConfig()
{
    ConfigManager* config = ConfigManager::instance();

    // 网络连接设置
    m_serverIP = config->getServerIP();
    m_serverPort = config->getServerPort();
    m_connectionTimeout = config->getConnectionTimeout();

    // 人脸识别设置
    m_faceThreshold = config->getFaceThreshold();
    m_maxFaceCount = config->getMaxFaceCount();
    m_recognizeTimeout = config->getRecognizeTimeout();

    // ArcFace SDK配置
    m_appId = config->getAppId();
    m_sdkKey = config->getSdkKey();

    // 存储设置
    m_databasePath = config->getDatabasePath();
    m_logPath = config->getLogPath();

    // 设备信息
    m_deviceId = config->getDeviceId();
    m_deviceKey = config->getDeviceKey();
    m_fwVersion = config->getFwVersion();
    m_deviceName = config->getDeviceName();

    // 同步设置
    m_autoSyncOnConnect = config->getAutoSyncOnConnect();
    m_syncTimeout = config->getSyncTimeout();
    m_cameraRotation = config->getCameraRotation();

    // 如果数据库路径为空，使用默认路径
    if(m_databasePath.isEmpty()){
        m_databasePath = ConfigManager::getDefaultDatabasePath();
    }
    // 如果日志路径为空，使用默认路径
    if(m_logPath.isEmpty()){
        m_logPath = ConfigManager::getDefaultLogPath();
    }

    // 更新UI显示
    saveToUI();
}

// 保存设置到配置文件
void SetWindow::saveToConfig()
{
    ConfigManager* config = ConfigManager::instance();

    // 网络连接设置
    config->setServerIP(m_serverIP);
    config->setServerPort(m_serverPort);
    config->setConnectionTimeout(m_connectionTimeout);

    // 人脸识别设置
    config->setFaceThreshold(m_faceThreshold);
    config->setMaxFaceCount(m_maxFaceCount);
    config->setRecognizeTimeout(m_recognizeTimeout);

    // ArcFace SDK配置
    config->setAppId(m_appId);
    config->setSdkKey(m_sdkKey);

    // 存储设置
    config->setDatabasePath(m_databasePath);
    config->setLogPath(m_logPath);

    // 设备信息
    config->setDeviceId(m_deviceId);
    config->setDeviceKey(m_deviceKey);
    config->setFwVersion(m_fwVersion);
    config->setDeviceName(m_deviceName);

    // 同步设置
    config->setAutoSyncOnConnect(m_autoSyncOnConnect);
    config->setSyncTimeout(m_syncTimeout);

    // 保存到文件
    config->saveConfig();
}

// 连接测试按钮槽函数 - 手动连接到服务器
void SetWindow::onBtnTestConnectionClicked()
{
    // 获取当前UI中的服务器地址和端口
    QString serverIP = ui->lineEditIP->text();
    int serverPort = ui->spinBoxPort->value();
    
    if (serverIP.isEmpty()) {
        QMessageBox::warning(this, "连接测试", "请输入服务器IP地址");
        return;
    }
    
    // 更新UI状态
    ui->btnTestConnection->setEnabled(false);
    ui->btnTestConnection->setText("连接中...");
    ui->labelConnectionStatus->setText("正在连接...");
    ui->labelConnectionStatus->setStyleSheet("color: orange;");
    
    // 获取NetworkClient实例（注意：它在另一个线程中，必须通过 QueuedConnection 调用）
    Networkclient *client = Networkclient::instance();

    // 连接信号（一次性连接，用于本次测试）
    connect(client, &Networkclient::connected, this, [=]() {
        ui->labelConnectionStatus->setText("✓ 已连接");
        ui->labelConnectionStatus->setStyleSheet("color: green; font-weight: bold;");
        ui->btnTestConnection->setText("已连接");
        ui->btnDisconnect->setEnabled(true);

        QMessageBox::information(this, "连接成功",
            QString("已成功连接到服务器\n地址: %1:%2").arg(serverIP).arg(serverPort));
    }, Qt::SingleShotConnection);

    connect(client, &Networkclient::disconnected, this, [=]() {
        ui->labelConnectionStatus->setText("未连接");
        ui->labelConnectionStatus->setStyleSheet("color: red; font-weight: bold;");
        ui->btnTestConnection->setEnabled(true);
        ui->btnTestConnection->setText("连接服务器");
        ui->btnDisconnect->setEnabled(false);
    }, Qt::SingleShotConnection);

    // 异步调用：先断开再连接（networkClient 在另一个线程，必须用 QueuedConnection）
    QMetaObject::invokeMethod(client, [=]() {
        if (client->isConnected()) {
            client->disconnect();
        }
        if (!client->connectToServer(serverIP, static_cast<quint16>(serverPort))) {
            // 连接失败，通过信号回到主线程更新UI
            QMetaObject::invokeMethod(this, [=]() {
                ui->labelConnectionStatus->setText("✗ 连接失败");
                ui->labelConnectionStatus->setStyleSheet("color: red; font-weight: bold;");
                ui->btnTestConnection->setEnabled(true);
                ui->btnTestConnection->setText("连接服务器");

                QMessageBox::critical(this, "连接失败", "无法连接到服务器，请检查地址和端口");
            }, Qt::QueuedConnection);
        }
    }, Qt::QueuedConnection);
}

// ---- 设备信息 ----

QString SetWindow::getDeviceId() const { return m_deviceId; }
QString SetWindow::getDeviceKey() const { return m_deviceKey; }
QString SetWindow::getFwVersion() const { return m_fwVersion; }
QString SetWindow::getDeviceName() const { return m_deviceName; }

void SetWindow::setDeviceId(const QString &id) {
    m_deviceId = id;
    ui->lineEditDeviceId->setText(id);
}
void SetWindow::setDeviceKey(const QString &key) {
    m_deviceKey = key;
    ui->lineEditDeviceKey->setText(key);
}
void SetWindow::setFwVersion(const QString &ver) {
    m_fwVersion = ver;
    ui->lineEditFwVersion->setText(ver);
}
void SetWindow::setDeviceName(const QString &name) {
    m_deviceName = name;
    ui->lineEditDeviceName->setText(name);
}

// ---- 同步设置 ----

bool SetWindow::getAutoSyncOnConnect() const { return m_autoSyncOnConnect; }
int  SetWindow::getSyncTimeout() const { return m_syncTimeout; }

void SetWindow::setAutoSyncOnConnect(bool enabled) {
    m_autoSyncOnConnect = enabled;
    ui->checkBoxAutoSync->setChecked(enabled);
}
void SetWindow::setSyncTimeout(int seconds) {
    m_syncTimeout = seconds;
    ui->spinBoxSyncTimeout->setValue(seconds);
}

// 断开连接按钮槽函数
void SetWindow::onBtnDisconnectClicked()
{
    Networkclient *client = Networkclient::instance();

    // 异步调用 disconnect（client 在另一个线程）
    QMetaObject::invokeMethod(client, [=]() {
        if (client->isConnected()) {
            client->disconnect();
        }
    }, Qt::QueuedConnection);

    // 立即更新UI（断开信号会通过 disconnected 信号回来更新UI）
    ui->labelConnectionStatus->setText("未连接");
    ui->labelConnectionStatus->setStyleSheet("color: red; font-weight: bold;");
    ui->btnTestConnection->setEnabled(true);
    ui->btnTestConnection->setText("连接服务器");
    ui->btnDisconnect->setEnabled(false);
}

void SetWindow::setFrameConverter(VideoFrameConverter *converter)
{
    m_converter = converter;
    if (m_converter) {
        m_converter->setRotation(m_cameraRotation);
    }
}

void SetWindow::updateRotationDisplay()
{
    ui->lineEditRotation->setText(QString::number(m_cameraRotation) + "°");
}

void SetWindow::onBtnRotateClicked()
{
    m_cameraRotation = (m_cameraRotation + 90) % 360;

    if (m_converter) {
        m_converter->setRotation(m_cameraRotation);
    }

    updateRotationDisplay();

    ConfigManager::instance()->setCameraRotation(m_cameraRotation);
    ConfigManager::instance()->saveConfig();
}
