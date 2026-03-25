#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FaceRecognition/facerecognizer.h"
#include "FaceRecognition/arcfaceengine.h"
#include "NetworkClient/networkclient.h"
#include "Config/configmanager.h"
#include "UI/facevideowidget.h"
#include <QCloseEvent>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,m_timeTimer(nullptr)
{
    ui->setupUi(this);

    //从配置恢复窗口大小
    restoreWindowSize();

    //初始化
    init();
    InfoWidget();
    FaceFeatureStart();

    // 初始化时间显示
    initTimeDisplay();

    //初始化网络状态
    initNetWorkStatus();

    //连接信号与槽
    connect(ui->settingButton,&QPushButton::clicked,this,&MainWindow::onSetPushButten);
}

MainWindow::~MainWindow()
{
    // 停止时间更新定时器
    if (m_timeTimer) {
        m_timeTimer->stop();
        delete m_timeTimer;
    }

    // 停止人脸识别线程
    if (m_faceThread) {
        //终止人脸线程事件循环的调用，向线程发送 “退出请求
        //必须配合 wait() 使用，确保线程真正结束，避免资源泄漏
        m_faceThread->quit();
        m_faceThread->wait();
    }

    // 停止网络线程
    if (m_networkThread) {
        m_networkThread->quit();
        m_networkThread->wait();
    }
    delete ui;
}

//初始化
void MainWindow::init()
{
    //数据库初始化
    m_db = LocalStorage::instance();
    if(!m_db->connectDatabse()){
        return;
    };

    //人脸识别引擎,加载特征到内存
    m_FaceRecognizer = new FaceRecognizer();
    m_FaceRecognizer->init();

    //网络客户端
    networkClient = Networkclient::instance();
    
    //多线程 - 将网络客户端移到独立线程（必须在连接信号之前）
    m_networkThread = new QThread(this);
    networkClient->moveToThread(m_networkThread);
    m_networkThread->start();
    
    //连接网络状态信号（使用Qt::QueuedConnection确保跨线程安全）
    connect(networkClient, &Networkclient::connected, this, [=](){
        qDebug() << "网络已连接";
    }, Qt::QueuedConnection);
    connect(networkClient, &Networkclient::disconnected, this, [=](){
        qDebug() << "网络已断开";
    }, Qt::QueuedConnection);
    connect(networkClient, &Networkclient::networkStateChanged, this, [=](bool isOnline){
        qDebug() << "网络状态变化:" << (isOnline ? "在线" : "离线");
    }, Qt::QueuedConnection);
    
    //连接服务器 (从配置中读取IP和端口)
    ConfigManager* config = ConfigManager::instance();
    QString serverIp = config->getServerIP();
    quint16 serverPort = static_cast<quint16>(config->getServerPort());
    qDebug() << "从配置文件读取服务器地址:" << serverIp << ":" << serverPort;

    //使用Qt::QueuedConnection异步调用连接（因为networkClient已在另一个线程）
    QMetaObject::invokeMethod(networkClient, [=](){
        if(networkClient->connectToServer(serverIp, serverPort)){
            qDebug() << "正在连接服务器...";
        } else {
            qWarning() << "连接服务器失败";
        }
    }, Qt::QueuedConnection);

    //摄像头初始化
    m_CameraCapture = new CameraCapture();
    if(!m_CameraCapture->initCamera()){
        qWarning() << "摄像头初始化失败";
        return;
    };
    m_VideoFrameCapture = new VideoFrameCapture();
    m_VideoFrameCapture->captureFrame(m_CameraCapture->getCamera());

    //多线程 - 将人脸识别移到独立线程
    m_faceThread = new QThread(this);
    m_FaceRecognizer->moveToThread(m_faceThread);
    m_faceThread->start();
}

//显示摄像头画面
void MainWindow::InfoWidget()
{
    m_VideoWidget = m_VideoFrameCapture->getVideoWidget();
    ui->cameraDisplay->setLayout(new QVBoxLayout());
    ui->cameraDisplay->layout()->addWidget(m_VideoWidget);
}

//开启人脸识别
void MainWindow::FaceFeatureStart()
{
    // 连接视频帧捕获到人脸识别器
    connect(m_VideoFrameCapture, &VideoFrameCapture::frameCaptured,
            m_FaceRecognizer, &FaceRecognizer::WanZhengYeWuLiuCheng);

    // 连接识别成功信号到UI更新槽
    connect(m_FaceRecognizer, &FaceRecognizer::recognitionSuccess,
            this, &MainWindow::onRecognitionSuccess);

    // 连接识别失败信号（可选，用于调试）
    connect(m_FaceRecognizer, &FaceRecognizer::recognitionFailed,
            this, [=](const QString &reason){
        qDebug() << "识别失败:" << reason;
    });

    //连接数据库保存请求信号
    connect(m_FaceRecognizer,&FaceRecognizer::requestSaveAttendance,this,&MainWindow::onSaveAttendanceRequest,Qt::QueuedConnection);

    connect(m_FaceRecognizer,&FaceRecognizer::faceDetected,this,[=](const QVector<arcfaceengine::FaceInfo> &faceInfos){
        QVector<FaceRectInfo> rectinfos;
        for(const auto &info : faceInfos){
            FaceRectInfo rectInfo;
            rectInfo.rect = info.rect;
            rectInfo.name = "";
            rectInfo.recognized = (m_FaceRecognizer->getbestMatch().second>0.8);
            rectinfos.append(rectInfo);
        }

        // 更新自定义视频控件的人脸框
        FaceVideoWidget* faceWidget = qobject_cast<FaceVideoWidget*>(m_VideoWidget);
        if(faceWidget){
            faceWidget->setFaceRects(rectinfos);
        }
    });

    // 连接视频帧信号，更新当前帧
    connect(m_VideoFrameCapture,&VideoFrameCapture::frameCaptured,this,[=](const QImage &frame){
        FaceVideoWidget* faceWidget = qobject_cast<FaceVideoWidget*> (m_VideoWidget);
        if(faceWidget){
            faceWidget->setCurrentFrame(frame);
        }
    });
}

//时间初始化
void MainWindow::initTimeDisplay()
{
    m_timeTimer = new QTimer();
    connect(m_timeTimer,&QTimer::timeout,this,&MainWindow::updateTimeDisplay);
    updateTimeDisplay();
    m_timeTimer->start(1000);
}

//网络状态初始化
void MainWindow::initNetWorkStatus()
{
    connect(networkClient,&Networkclient::networkStateChanged,this,&MainWindow::onNetworkStateChanged);
    onNetworkStateChanged(false);
}

//识别成功更新UI
void MainWindow::onRecognitionSuccess(const QString &employeeId,
                                      const QString &name,
                                      const QString &status,
                                      const QString &checkTime,
                                      const QImage &faceImage)
{
    // 更新员工号
    ui->employeeIdEdit->setText(employeeId);

    // 更新姓名
    ui->nameEdit->setText(name);

    // 更新打卡状态
    ui->statusEdit->setText(status);

    // 更新打卡时间
    ui->checkTimeEdit->setText(checkTime);

    // 显示人脸识别图像
    if(!faceImage.isNull()){
        // 将QImage转换为QPixmap并显示在faceImageLabel中
        QPixmap pixmap = QPixmap::fromImage(faceImage);
        // 缩放图像以适应标签大小，保持宽高比
        pixmap = pixmap.scaled(ui->faceImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->faceImageLabel->setPixmap(pixmap);
        ui->faceImageLabel->setAlignment(Qt::AlignCenter);
    }

    // 在状态栏显示识别成功信息
    ui->statusbar->showMessage(QString("识别成功 - 员工:%1 时间:%2").arg(employeeId, checkTime), 5000);

    qDebug() << "识别成功 - 员工:" << employeeId
             << "状态:" << status
             << "时间:" << checkTime;
}

//处理保存打卡记录请求
void MainWindow::onSaveAttendanceRequest(const QString &employeeId, const QString &status)
{
    bool saved = m_db->addAttendanceRecord(employeeId,status);

    if(saved){
        qDebug() << "打卡记录已保存:" << employeeId;

        // 上传到服务器
        Protocol::AttendanceRecord record;
        record.employeeId = employeeId;
        record.checktTime = QDateTime::currentDateTime().toString(Qt::ISODate);
        record.status = status;
        networkClient->uploadAttendance(record);

        // 更新状态栏
        ui->statusbar->showMessage(QString("打卡成功 - 员工:%1").arg(employeeId), 3000);
    }else{
        qWarning() << "保存打卡记录失败:" << employeeId;
        ui->statusbar->showMessage(QString("打卡失败 - 员工:%1").arg(employeeId), 3000);
    }
}

//更新时间显示
void MainWindow::updateTimeDisplay()
{
    QDateTime currenTime = QDateTime::currentDateTime();
    QString timeString = currenTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->timeLabel->setText(timeString);
}

void MainWindow::onNetworkStateChanged(bool isOnline)
{
    if(isOnline){
        ui->networkStatusLabel->setText("● 在线");
        ui->networkStatusLabel->setStyleSheet("color:green;font-weight:bold;");
    }else{
        ui->networkStatusLabel->setText("● 离线");
        ui->networkStatusLabel->setStyleSheet("color:red;font-weight:bold;");
    }
}

void MainWindow::onSetPushButten()
{
    setwindow.show();
}

//从配置恢复窗口大小
void MainWindow::restoreWindowSize()
{
    ConfigManager* config = ConfigManager::instance();
    int width = config->getMainWindowWidth();
    int height = config->getMainWindowHeight();

    //确保尺寸在合理范围内
    if (width < 800) width = 800;
    if (height < 600) height = 600;
    if (width > 1920) width = 1920;
    if (height > 1080) height = 1080;

    this->resize(width, height);
    qDebug() << "恢复窗口大小:" << width << "x" << height;
}

//保存窗口大小到配置
void MainWindow::saveWindowSize()
{
    ConfigManager* config = ConfigManager::instance();
    config->setMainWindowWidth(this->width());
    config->setMainWindowHeight(this->height());
    config->saveConfig();
    qDebug() << "保存窗口大小:" << this->width() << "x" << this->height();
}

//窗口大小改变事件
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    //可以在这里实时保存，或者只在关闭时保存
}

//窗口关闭事件
void MainWindow::closeEvent(QCloseEvent *event)
{
    //保存窗口大小
    saveWindowSize();
    event->accept();
}
