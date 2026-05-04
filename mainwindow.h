#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CameraCapture/videoframecapture.h"
#include "FaceRecognition/arcfaceengine.h"
#include "FaceRecognition/facerecognizer.h"
#include "UI/setwindow.h"
#include "LocalStorage/localstorage.h"
#include "UI/setwindow.h"

#include <QMainWindow>
#include <QThread>

#include <QTimer>
#include <QDateTime>
#include <QMouseEvent>

#include <QWidget>
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

private:
    enum Edge {
        None = 0,
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

private slots:
    // 识别成功槽函数 - 更新UI显示
    void onRecognitionSuccess(const QString &employeeId,
                              const QString &name,
                              const QString &status,
                              const QString &checkTime,
                              const QImage &faceImage);
    // void onSettingButtonClicked();//打开设置窗口

    //处理保存打卡记录请求（主线程执行数据库操作）
    void onSaveAttendanceRequest(const QString &employeeId,const QString &status);

    //更新时间显示槽函数
    void updateTimeDisplay();

    //更新网络状态显示
    void onNetworkStateChanged(bool isOnline);

    //打开设置窗口
    void onSetPushButten();

    //窗口控制按钮槽函数
    void onMinimizeButtonClicked();
    void onMaximizeButtonClicked();
    void onCloseButtonClicked();

private:
    //初始化
    void init();
    void initNetworkClient();      //初始化网络客户端（创建和移到线程）
    void startNetworkConnection(); //启动网络连接

    //页面信息展示
    void InfoWidget();

    //开启人脸检测
    void FaceFeatureStart();

    //初始化时间显示
    void initTimeDisplay();

    //初始化网络状态显示
    void initNetWorkStatus();

    //从配置恢复窗口大小
    void restoreWindowSize();
    //保存窗口大小到配置
    void saveWindowSize();

protected:
    //窗口大小改变事件
    void resizeEvent(QResizeEvent *event) override;
    //窗口关闭事件
    void closeEvent(QCloseEvent *event) override;

    //窗口移动事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    //事件过滤器
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    LocalStorage* m_db;
    FaceRecognizer* m_FaceRecognizer;
    Networkclient* networkClient;
    CameraCapture* m_CameraCapture;
    VideoFrameCapture* m_VideoFrameCapture;
    QWidget* m_VideoWidget;
    QThread* m_faceThread;
    QThread* m_networkThread;
    QTimer* m_timeTimer;
    SetWindow* setwindow;  // 改为指针，在构造函数中创建并设置父对象
    QPoint m_dragPosition; //记录拖拽时的鼠标位置
    bool m_isDragging = false; //是否正在被拖拽
    bool m_isResizing = false;
    Edge m_currentEdge = None;
    QRect m_originalGeometry;


    Edge getEdge(const QPoint &pos);
    void updateCursor(const QPoint &pos);
};
#endif // MAINWINDOW_H
