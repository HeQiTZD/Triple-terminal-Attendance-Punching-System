#include "videoframeconverter.h"
#include <QTransform>
#include <QDebug>

VideoFrameConverter::VideoFrameConverter() {}

/**
 * @brief 将视频帧转换为QImage
 *
 * 使用 Qt6 内置的 QVideoFrame::toImage() 进行转换，
 * 内部自动处理所有像素格式的色彩空间转换和行对齐。
 */
QImage VideoFrameConverter::convertToQImage(const QVideoFrame frame)
{
    if (!frame.isValid()) {
        qWarning() << "视频帧无效";
        return QImage();
    }

    // 使用 Qt6 内置方法，正确处理 NV12/YUV420P/YUYV 等所有格式的色彩转换
    QVideoFrame cloneFrame = frame;
    QImage image = cloneFrame.toImage();

    if (image.isNull() || image.width() == 0 || image.height() == 0) {
        qWarning() << "视频帧转换失败，格式:" << frame.pixelFormat();
        return QImage();
    }

    // 顺时针旋转 90°，横屏转竖屏
    image = image.transformed(QTransform().rotate(90));
    return image;
}
