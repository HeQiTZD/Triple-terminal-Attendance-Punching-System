#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CameraCapture/videoframecapture.h"
#include "FaceRecognition/arcfaceengine.h"
#include "UI/setwindow.h"
#include "LocalStorage/localstorage.h"

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
    void onRecognitionResult();//识别结果并更新UI
    void updateCameraDisplay();//更新摄像头画面
    void onSettingButtonClicked();//打开设置窗口

private:
    void init();//初始化
};
#endif // MAINWINDOW_H
