#ifndef VIDEOFRAMECONVERTER_H
#define VIDEOFRAMECONVERTER_H

#include <QImage>
#include <QVideoFrame>

/**
 * @brief 视频帧格式转换工具类
 * 
 * 该类提供视频帧到QImage的格式转换功能，支持多种常见的视频帧格式：
 * - Qt原生支持的格式（如RGB32、ARGB32等）- 直接转换
 * - YUV420P（I420）- 手动转换
 * - NV12/NV21 - 手动转换（常见于移动设备摄像头）
 * - YUYV（YUY2）- 手动转换
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
     * 该方法会自动识别视频帧的像素格式，并选择合适的转换方式：
     * 1. 如果Qt原生支持该格式，则直接转换
     * 2. 如果是YUV格式，则使用手动算法转换为RGB
     * 
     * @param frame 输入的视频帧（会被复制，原帧不会被修改）
     * @return QImage 转换后的图像，如果转换失败则返回空QImage
     * 
     * @note 该方法会输出调试信息，包括帧格式、尺寸等
     */
    static QImage convertToQImage(const QVideoFrame frame);

private:
    /**
     * @brief 将YUV格式视频帧转换为RGB
     * 
     * 支持以下YUV格式：
     * - YUV420P（I420）：Y、U、V三个平面分开存储
     * - NV12：Y平面 + UV交错平面（UVUV...）
     * - NV21：Y平面 + VU交错平面（VUVU...）
     * 
     * @param frame 已映射的YUV格式视频帧
     * @return QImage 转换后的RGB32格式图像
     */
    static QImage convertYUVToRGB(const QVideoFrame &frame);

    /**
     * @brief 将YUYV格式视频帧转换为RGB
     * 
     * YUYV（也称YUY2）是一种打包的YUV 4:2:2格式：
     * - 每4个字节表示2个像素：Y0 U0 Y1 V0
     * - U和V分量在水平方向上2:1下采样
     * 
     * @param frame 已映射的YUYV格式视频帧
     * @return QImage 转换后的RGB32格式图像
     */
    static QImage convertYUYVToRGB(const QVideoFrame &frame);
};

#endif // VIDEOFRAMECONVERTER_H
