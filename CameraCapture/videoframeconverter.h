#ifndef VIDEOFRAMECONVERTER_H
#define VIDEOFRAMECONVERTER_H

#include <QImage>
#include <QVideoFrame>

/**
 * @brief 视频帧格式转换工具类
 *
 * 使用 Qt6 内置的 QVideoFrame::toImage() 进行格式转换，
 * 内部通过 GPU/优化路径正确处理所有像素格式（NV12、YUV420P、YUYV等）。
 *
 * 使用示例：
 * @code
 * QVideoFrame frame = ...; // 从摄像头获取的视频帧
 * QImage image = VideoFrameConverter::convertToQImage(frame);
 * if (!image.isNull()) {
 *     // 使用转换后的图像
 * }
 * @endcode
 */
class VideoFrameConverter
{
public:
    VideoFrameConverter();

    /**
     * @brief 将视频帧转换为QImage
     *
     * 优先使用 Qt6 内置的 QVideoFrame::toImage() 进行转换，
     * 正确处理 YUV/NV12 等所有格式的色彩空间转换和行对齐。
     *
     * @param frame 输入的视频帧（会被复制，原帧不会被修改）
     * @return QImage 转换后的图像，如果转换失败则返回空QImage
     */
    static QImage convertToQImage(const QVideoFrame frame);
};

#endif // VIDEOFRAMECONVERTER_H
