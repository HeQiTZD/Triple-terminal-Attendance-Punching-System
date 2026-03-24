#include "facevideowidget.h"

FaceVideoWidget::FaceVideoWidget(QWidget *parent) : QVideoWidget(parent){}

void FaceVideoWidget::setCurrentFrame(const QImage &frame)
{
    QMutexLocker locker(&m_mutex);
    m_currentFrame = frame;
}

void FaceVideoWidget::setFaceRects(const QVector<FaceRectInfo> &faceRects)
{
    {
        QMutexLocker locker(&m_mutex);
        m_faceRects = faceRects;
    }
    //触发重绘
    update();
}

void FaceVideoWidget::clearFaceRects()
{
    {
        QMutexLocker locker(&m_mutex);
        m_faceRects.clear();
    }
    update();
}

void FaceVideoWidget::paintEvent(QPaintEvent *event)
{
    // 先调用父类的绘制（绘制视频帧）
    QVideoWidget::paintEvent(event);

    // 在视频上绘制人脸框
    QPainter painter(this);
    //setRenderHint设置渲染提示 / 渲染选项，告诉绘图引擎用什么规则绘制内容。
    painter.setRenderHint(QPainter::Antialiasing);//QPainter::Antialiasing抗锯齿

    QMutexLocker locker(&m_mutex);

    // 计算视频帧在控件中的显示区域（保持宽高比）
    QRect videoRect = rect();
    if(!m_currentFrame.isNull()){
        QSize frameSize = m_currentFrame.size();
        QSize widgetSize = size();

        // 计算缩放比例（保持宽高比）(qreal) 是强制类型转换
        qreal scalex = (qreal)widgetSize.width()/frameSize.width();
        qreal scaley = (qreal)widgetSize.height()/frameSize.height();
        qreal scale = qMin(scalex,scaley);//取最小，“等比例自适应、居中显示、完整显示” 的标准算法。

        // 计算居中显示的区域
        int displayWidget = frameSize.width() * scale;
        int displayHeight = frameSize.height() * scale;
        int offsetX = (widgetSize.width() - displayWidget) / 2;
        int offsetY = (widgetSize.height() - displayHeight) / 2;


        // 绘制每个人脸框
        for (const FaceRectInfo &faceInfo:m_faceRects){
            // 将人脸坐标从图像坐标系转换为控件坐标系
            QRect scaledRect;
            scaledRect.setX(faceInfo.rect.x() * scale + offsetX);
            scaledRect.setY(faceInfo.rect.y() * scale + offsetY);
            scaledRect.setWidth(faceInfo.rect.width() * scale);
            scaledRect.setHeight(faceInfo.rect.height() * scale);

            //根据识别状态选择颜色
            if(faceInfo.recognized){
                painter.setPen(QPen(Qt::green,3));
            }else{
                painter.setPen(QPen(Qt::red,3));
            }

            //绘制矩形框
            painter.drawRect(scaledRect);

            //姓名，绘制文字标签
            if(!faceInfo.name.isEmpty()){
                painter.setBrush(Qt::green);
                QRect textRect(scaledRect.x(),scaledRect.y() - 25,scaledRect.width(),25);
                painter.drawRect(textRect);

                painter.setPen(Qt::white);
                painter.setFont(QFont("Microsoft YaHei",10,QFont::Bold));
                painter.drawText(textRect,Qt::AlignCenter,faceInfo.name);
            }
        }
    }
}
