#ifndef FACEVIDEOWIDGET_H
#define FACEVIDEOWIDGET_H

#include <QVideoWidget>
#include <QImage>
#include <QRect>
#include <QVector>
#include <QMutex>
#include <QPainter>

//人脸信息
struct FaceRectInfo{
    QRect rect;//人脸框位置
    QString name;//识别到的姓名
    bool recognized;//是否识别
};

class FaceVideoWidget : public QVideoWidget
{
    Q_OBJECT
public:
    explicit FaceVideoWidget(QWidget *parent = nullptr);

    //设置当前帧图像（用于人脸检测）
    void setCurrentFrame(const QImage &frame);

    // 设置人脸框信息
    void setFaceRects(const QVector<FaceRectInfo> &faceRects);

    //清除所有人脸框
    void clearFaceRects();

protected:
    // 重写绘制事件，在视频上绘制人脸框
    void paintEvent(QPaintEvent *event);

private:
    QImage m_currentFrame;//当前帧图像
    QVector<FaceRectInfo> m_faceRects;//人脸框列表
    QMutex m_mutex;// 线程安全
};

#endif // FACEVIDEOWIDGET_H
