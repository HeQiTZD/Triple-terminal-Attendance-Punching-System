#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FaceRecognition/facerecognizer.h"
#include "NetworkClient/networkclient.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化
    init();
}

MainWindow::~MainWindow()
{
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
    //连接网络状态信号
    connect(networkClient, &Networkclient::connected, this, [=](){
        qDebug() << "网络已连接";
    });
    connect(networkClient, &Networkclient::disconnected, this, [=](){
        qDebug() << "网络已断开";
    });
    connect(networkClient, &Networkclient::networkStateChanged, this, [=](bool isOnline){
        qDebug() << "网络状态变化:" << (isOnline ? "在线" : "离线");
    });
    //连接服务器 (从配置中读取IP和端口，这里使用默认值示例)
    QString serverIp = "127.0.0.1";  // 可以从配置文件读取
    quint16 serverPort = 8080;        // 可以从配置文件读取
    if(networkClient->connectToServer(serverIp, serverPort)){
        qDebug() << "正在连接服务器...";
    } else {
        qWarning() << "连接服务器失败";
    }

    //摄像头初始化
    m_CameraCapture = new CameraCapture();
    if(!m_CameraCapture->initCamera()){
        qWarning() << "摄像头初始化失败";
        return;
    };
    m_VideoFrameCapture = new VideoFrameCapture();
    m_VideoFrameCapture->captureFrame(m_CameraCapture->getCamera());
}
