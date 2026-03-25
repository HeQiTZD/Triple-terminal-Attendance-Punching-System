#include "setwindow.h"
#include "ui_setwindow.h"
#include "../Config/configmanager.h"
#include "../NetworkClient/networkclient.h"
#include <QMessageBox>
#include <QTcpSocket>
#include <QTimer>
#include <QElapsedTimer>

SetWindow::SetWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SetWindow)
{
    ui->setupUi(this);

    setupConnections();
    loadFromConfig();
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
    connect(ui->btnAttendance, &QPushButton::clicked, this, &SetWindow::onNavButtonClicked);
    connect(ui->btnStorage, &QPushButton::clicked, this, &SetWindow::onNavButtonClicked);

    // 功能按钮信号连接
    connect(ui->btnRestore, &QPushButton::clicked, this, &SetWindow::onBtnRestoreClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &SetWindow::onBtnCancelClicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &SetWindow::onBtnSaveClicked);

    // 阈值滑块信号连接
    connect(ui->sliderThreshold, &QSlider::valueChanged, this, &SetWindow::onSliderThresholdChanged);
    
    // 连接测试按钮信号连接
    connect(ui->btnTestConnection, &QPushButton::clicked, this, &SetWindow::onBtnTestConnectionClicked);
    connect(ui->btnDisconnect, &QPushButton::clicked, this, &SetWindow::onBtnDisconnectClicked);
}

void SetWindow::onNavButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    if (btn == ui->btnNetwork) {
        switchToPage(0);
    } else if (btn == ui->btnFace) {
        switchToPage(1);
    } else if (btn == ui->btnAttendance) {
        switchToPage(2);
    } else if (btn == ui->btnStorage) {
        switchToPage(3);
    }
}

void SetWindow::switchToPage(int index)
{
    ui->stackedWidget->setCurrentIndex(index);

    // 更新按钮选中状态
    ui->btnNetwork->setChecked(index == 0);
    ui->btnFace->setChecked(index == 1);
    ui->btnAttendance->setChecked(index == 2);
    ui->btnStorage->setChecked(index == 3);
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

    // 恢复考勤规则设置
    m_workStartTime = QTime(9, 0);
    m_workEndTime = QTime(18, 0);
    m_lateAllowance = DEFAULT_LATE_ALLOWANCE;
    m_earlyLeaveAllowance = DEFAULT_EARLY_LEAVE_ALLOWANCE;

    // 恢复存储设置（路径不清空）
    // m_databasePath, m_logPath, m_cachePath 保持当前值

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

// 考勤规则设置 - Getter
QTime SetWindow::getWorkStartTime() const
{
    return m_workStartTime;
}

QTime SetWindow::getWorkEndTime() const
{
    return m_workEndTime;
}

int SetWindow::getLateAllowance() const
{
    return m_lateAllowance;
}

int SetWindow::getEarlyLeaveAllowance() const
{
    return m_earlyLeaveAllowance;
}

// 考勤规则设置 - Setter
void SetWindow::setWorkStartTime(const QTime &time)
{
    m_workStartTime = time;
    ui->timeEditWorkStart->setTime(time);
}

void SetWindow::setWorkEndTime(const QTime &time)
{
    m_workEndTime = time;
    ui->timeEditWorkEnd->setTime(time);
}

void SetWindow::setLateAllowance(int minutes)
{
    m_lateAllowance = minutes;
    ui->spinBoxLate->setValue(minutes);
}

void SetWindow::setEarlyLeaveAllowance(int minutes)
{
    m_earlyLeaveAllowance = minutes;
    ui->spinBoxEarly->setValue(minutes);
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

    // 考勤规则设置
    m_workStartTime = ui->timeEditWorkStart->time();
    m_workEndTime = ui->timeEditWorkEnd->time();
    m_lateAllowance = ui->spinBoxLate->value();
    m_earlyLeaveAllowance = ui->spinBoxEarly->value();

    // 存储设置
    m_databasePath = ui->lineEditDbPath->text();
    m_logPath = ui->lineEditLogPath->text();
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

    // 考勤规则设置
    ui->timeEditWorkStart->setTime(m_workStartTime);
    ui->timeEditWorkEnd->setTime(m_workEndTime);
    ui->spinBoxLate->setValue(m_lateAllowance);
    ui->spinBoxEarly->setValue(m_earlyLeaveAllowance);

    // 存储设置
    ui->lineEditDbPath->setText(m_databasePath);
    ui->lineEditLogPath->setText(m_logPath);
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

    // 考勤规则设置
    m_workStartTime = config->getWorkStartTime();
    m_workEndTime = config->getWorkEndTime();
    m_lateAllowance = config->getLateAllowance();
    m_earlyLeaveAllowance = config->getEarlyLeaveAllowance();

    // 存储设置
    m_databasePath = config->getDatabasePath();
    m_logPath = config->getLogPath();
    
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

    // 考勤规则设置
    config->setWorkStartTime(m_workStartTime);
    config->setWorkEndTime(m_workEndTime);
    config->setLateAllowance(m_lateAllowance);
    config->setEarlyLeaveAllowance(m_earlyLeaveAllowance);

    // 存储设置
    config->setDatabasePath(m_databasePath);
    config->setLogPath(m_logPath);

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
    
    // 获取NetworkClient实例
    Networkclient *client = Networkclient::instance();
    
    // 先断开现有连接
    if (client->isConnected()) {
        client->disconnect();
    }
    
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
    
    // 尝试连接
    if (!client->connectToServer(serverIP, static_cast<quint16>(serverPort))) {
        ui->labelConnectionStatus->setText("✗ 连接失败");
        ui->labelConnectionStatus->setStyleSheet("color: red; font-weight: bold;");
        ui->btnTestConnection->setEnabled(true);
        ui->btnTestConnection->setText("连接服务器");
        
        QMessageBox::critical(this, "连接失败", "无法连接到服务器，请检查地址和端口");
    }
}

// 断开连接按钮槽函数
void SetWindow::onBtnDisconnectClicked()
{
    Networkclient *client = Networkclient::instance();
    
    if (client->isConnected()) {
        client->disconnect();
        
        ui->labelConnectionStatus->setText("未连接");
        ui->labelConnectionStatus->setStyleSheet("color: red; font-weight: bold;");
        ui->btnTestConnection->setEnabled(true);
        ui->btnTestConnection->setText("连接服务器");
        ui->btnDisconnect->setEnabled(false);
        
        QMessageBox::information(this, "断开连接", "已断开与服务器的连接");
    }
}
