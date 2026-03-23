#include "setwindow.h"
#include "ui_setwindow.h"

SetWindow::SetWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SetWindow)
{
    ui->setupUi(this);
}

SetWindow::~SetWindow()
{
    delete ui;
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

QString SetWindow::getCachePath() const
{
    return m_cachePath;
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

void SetWindow::setCachePath(const QString &path)
{
    m_cachePath = path;
    ui->lineEditCachePath->setText(path);
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

    // 考勤规则设置
    m_workStartTime = ui->timeEditWorkStart->time();
    m_workEndTime = ui->timeEditWorkEnd->time();
    m_lateAllowance = ui->spinBoxLate->value();
    m_earlyLeaveAllowance = ui->spinBoxEarly->value();

    // 存储设置
    m_databasePath = ui->lineEditDbPath->text();
    m_logPath = ui->lineEditLogPath->text();
    m_cachePath = ui->lineEditCachePath->text();
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

    // 考勤规则设置
    ui->timeEditWorkStart->setTime(m_workStartTime);
    ui->timeEditWorkEnd->setTime(m_workEndTime);
    ui->spinBoxLate->setValue(m_lateAllowance);
    ui->spinBoxEarly->setValue(m_earlyLeaveAllowance);

    // 存储设置
    ui->lineEditDbPath->setText(m_databasePath);
    ui->lineEditLogPath->setText(m_logPath);
    ui->lineEditCachePath->setText(m_cachePath);
}
