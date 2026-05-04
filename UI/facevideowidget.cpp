#include "facevideowidget.h"

FaceVideoWidget::FaceVideoWidget(QWidget *parent) : QWidget(parent){
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void FaceVideoWidget::setCurrentFrame(const QImage &frame)
{
    {
        QMutexLocker locker(&m_mutex);
    m_currentFrame = frame;
    }
    update();
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
    Q_UNUSED(event)

    // 在视频上绘制人脸框
    QPainter painter(this);
    //setRenderHint 设置渲染提示 / 渲染选项，告诉绘图引擎用什么规则绘制内容。
    painter.setRenderHint(QPainter::Antialiasing);//QPainter::Antialiasing 抗锯齿

    QMutexLocker locker(&m_mutex);

    // 无画面时绘制黑色背景
    if(m_currentFrame.isNull()){
        painter.fillRect(rect(), Qt::black);
        return;
    }

    QSize frameSize = m_currentFrame.size();
    QSize widgetSize = size();

    // 绘制黑色背景
    painter.fillRect(rect(), Qt::black);

    // 计算保持宽高比的绘制区域（类似 Qt::KeepAspectRatioByExpanding）
    QRect targetRect;
    if (frameSize.width() > 0 && frameSize.height() > 0) {
        qreal frameAspect = (qreal)frameSize.width() / frameSize.height();
        qreal widgetAspect = (qreal)widgetSize.width() / widgetSize.height();
        
        if (widgetAspect > frameAspect) {
            // 控件更宽，按高度缩放
            int targetWidth = qRound(widgetSize.height() * frameAspect);
            int xOffset = (widgetSize.width() - targetWidth) / 2;
            targetRect = QRect(xOffset, 0, targetWidth, widgetSize.height());
        } else {
            // 控件更高，按宽度缩放
            int targetHeight = qRound(widgetSize.width() / frameAspect);
            int yOffset = (widgetSize.height() - targetHeight) / 2;
            targetRect = QRect(0, yOffset, widgetSize.width(), targetHeight);
        }
    } else {
        targetRect = rect();
    }

    // 绘制视频帧（保持宽高比）
    painter.drawImage(targetRect, m_currentFrame);

    // 绘制每个人脸框
    for (const FaceRectInfo &faceInfo:m_faceRects){
        // 将人脸坐标从图像坐标系转换为控件坐标系（使用保持宽高比的缩放比例）
        qreal scaleX = (qreal)targetRect.width() / frameSize.width();
        qreal scaleY = (qreal)targetRect.height() / frameSize.height();
        
        QRect scaledRect;
        scaledRect.setX(targetRect.x() + faceInfo.rect.x() * scaleX);
        scaledRect.setY(targetRect.y() + faceInfo.rect.y() * scaleY);
        scaledRect.setWidth(faceInfo.rect.width() * scaleX);
        scaledRect.setHeight(faceInfo.rect.height() * scaleY);

        //根据识别状态选择颜色
        if(faceInfo.recognized){
            painter.setPen(QPen(Qt::green,3));
        }else{
            painter.setPen(QPen(Qt::red,3));
        }

        painter.setBrush(Qt::NoBrush);
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
