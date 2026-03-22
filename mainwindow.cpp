#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "FaceRecognition/facerecognizer.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//初始化
void MainWindow::init()
{
    //数据库初始化
    LocalStorage* m_db = LocalStorage::instance();
    if(!m_db->connectDatabse()){
        return;
    };

    //人脸识别引擎,加载特征到内存
    FaceRecognizer* m_FaceRecognizer = new FaceRecognizer();
    m_FaceRecognizer->init();

    //网络客户端
}
