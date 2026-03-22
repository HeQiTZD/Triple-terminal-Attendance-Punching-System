#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CameraCapture/videoframecapture.h"
#include "FaceRecognition/arcfaceengine.h"
#include "FaceRecognition/facerecognizer.h"
#include "UI/setwindow.h"
#include "LocalStorage/localstorage.h"

#include "text.h"

#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    // void onRecognitionResult();//识别结果并更新UI
    // void onSettingButtonClicked();//打开设置窗口

private:
    //初始化
    void init();

    //页面信息展示
    void InfoWidget();

private:
    LocalStorage* m_db;
    FaceRecognizer* m_FaceRecognizer;
    Networkclient* networkClient;
    CameraCapture* m_CameraCapture;
    VideoFrameCapture* m_VideoFrameCapture;
    QVideoWidget* m_VideoWidget;
};
#endif // MAINWINDOW_H
