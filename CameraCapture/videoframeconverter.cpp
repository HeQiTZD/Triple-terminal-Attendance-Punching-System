#include "videoframeconverter.h"
#include <QVideoFrameFormat>
#include <QDebug>

VideoFrameConverter::VideoFrameConverter() {}

/**
 * @brief 将视频帧转换为QImage
 * 
 * 转换流程：
 * 1. 验证帧有效性
 * 2. 映射帧数据到内存
 * 3. 识别像素格式
 * 4. 根据格式选择转换方式
 */
QImage VideoFrameConverter::convertToQImage(const QVideoFrame frame)
{
    // 步骤1：验证帧有效性
    if (!frame.isValid()) {
        qWarning() << "视频帧无效";
        return QImage();
    }

    // 步骤2：复制并映射帧数据
    // QVideoFrame需要映射到内存才能访问像素数据
    QVideoFrame cloneFrame = frame;
    if (!cloneFrame.map(QVideoFrame::ReadOnly)) {
        qWarning() << "无法映射视频帧";
        return QImage();
    }

    QImage image;
    
    // 步骤3：获取像素格式并尝试直接转换
    QVideoFrameFormat::PixelFormat pixelFormat = cloneFrame.pixelFormat();
    QImage::Format imageFormat = QVideoFrameFormat::imageFormatFromPixelFormat(pixelFormat);

    // 输出帧信息用于调试
    // qDebug() << "视频帧格式:" << pixelFormat
    //          << "尺寸:" << cloneFrame.width() << "x" << cloneFrame.height()
    //          << "字节数:" << cloneFrame.mappedBytes(0)
    //          << "行字节数:" << cloneFrame.bytesPerLine(0);

    // 步骤4：根据格式选择转换方式
    if (imageFormat != QImage::Format_Invalid) {
        // Qt原生支持该格式，直接创建QImage
        // 使用bytesPerLine确保正确处理行对齐
        image = QImage(cloneFrame.bits(0), cloneFrame.width(), cloneFrame.height(),
                      cloneFrame.bytesPerLine(0), imageFormat);
        
        // 验证QImage创建是否成功
        if (image.isNull() || image.width() == 0 || image.height() == 0) {
            qWarning() << "QImage创建失败";
            cloneFrame.unmap();
            return QImage();
        }
        
        // 复制图像数据，因为原始帧数据会在unmap后失效
        image = image.copy();
        qDebug() << "直接转换成功:" << image.width() << "x" << image.height();
    } else {
        // Qt不原生支持该格式，需要手动转换
        //qWarning() << "不支持的像素格式，尝试手动转换";
        
        switch (pixelFormat) {
        case QVideoFrameFormat::Format_YUV420P:
        case QVideoFrameFormat::Format_NV12:
        case QVideoFrameFormat::Format_NV21:
            // YUV 4:2:0 格式转换
            image = convertYUVToRGB(cloneFrame);
            break;
        case QVideoFrameFormat::Format_YUYV:
            // YUV 4:2:2 打包格式转换
            image = convertYUYVToRGB(cloneFrame);
            break;
        default:
            qWarning() << "未知的像素格式:" << pixelFormat;
            break;
        }
    }

    // 步骤5：解除映射并返回结果
    cloneFrame.unmap();
    return image;
}

/**
 * @brief YUV到RGB的转换实现
 * 
 * YUV色彩空间转换公式：
 * R = Y + 1.402 * (V - 128)
 * G = Y - 0.344 * (U - 128) - 0.714 * (V - 128)
 * B = Y + 1.772 * (U - 128)
 * 
 * 其中：
 * - Y：亮度分量，范围0-255
 * - U/V：色度分量，范围0-255，中心值为128
 */
QImage VideoFrameConverter::convertYUVToRGB(const QVideoFrame &frame)
{
    int width = frame.width();
    int height = frame.height();
    const uchar *data = frame.bits(0);

    // 创建RGB32格式的目标图像
    QImage image(width, height, QImage::Format_RGB32);
    if (image.isNull()) {
        return QImage();
    }

    QVideoFrameFormat::PixelFormat format = frame.pixelFormat();
    
    if (format == QVideoFrameFormat::Format_YUV420P) {
        // YUV420P（I420）格式布局：
        // |---- Y 平面 ----|---- U 平面 ----|---- V 平面 ----|
        // Y平面：width * height 字节
        // U平面：width/2 * height/2 字节（4:2:0下采样）
        // V平面：width/2 * height/2 字节（4:2:0下采样）
        
        const uchar *yPlane = data;
        const uchar *uPlane = data + width * height;
        const uchar *vPlane = uPlane + (width * height) / 4;

        // 逐像素转换
        for (int y = 0; y < height; ++y) {
            QRgb *scanLine = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < width; ++x) {
                // Y分量索引（每个像素都有独立的Y值）
                int yIndex = y * width + x;
                // UV分量索引（2x2像素块共享一个UV值）
                int uvIndex = (y / 2) * (width / 2) + (x / 2);

                int yVal = yPlane[yIndex];
                int u = uPlane[uvIndex] - 128;  // U分量偏移到-128~127范围
                int v = vPlane[uvIndex] - 128;  // V分量偏移到-128~127范围

                // YUV转RGB公式
                int r = qBound(0, yVal + static_cast<int>(1.402 * v), 255);
                int g = qBound(0, yVal - static_cast<int>(0.344 * u - 0.714 * v), 255);
                int b = qBound(0, yVal + static_cast<int>(1.772 * u), 255);

                scanLine[x] = qRgb(r, g, b);
            }
        }
    } else if (format == QVideoFrameFormat::Format_NV12 || format == QVideoFrameFormat::Format_NV21) {
        // NV12/NV21 格式布局：
        // |---- Y 平面 ----|---- UV 交错平面 ----|
        // Y平面：width * height 字节
        // UV平面：width * height / 2 字节（UV交错存储）
        // NV12: UVUVUV... (U在前)
        // NV21: VUVUVU... (V在前)
        
        const uchar *yPlane = data;
        const uchar *uvPlane = data + width * height;

        for (int y = 0; y < height; ++y) {
            QRgb *scanLine = reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < width; ++x) {
                int yIndex = y * width + x;
                // UV交错存储，每两个字节为一组UV
                int uvIndex = (y / 2) * width + (x / 2) * 2;

                int yVal = yPlane[yIndex];
                int u, v;

                if (format == QVideoFrameFormat::Format_NV12) {
                    // NV12: UVUVUV...
                    v = uvPlane[uvIndex] - 128;
                    u = uvPlane[uvIndex + 1] - 128;
                } else {
                    // NV21: VUVUVU...
                    u = uvPlane[uvIndex] - 128;
                    v = uvPlane[uvIndex + 1] - 128;
                }

                int r = qBound(0, yVal + static_cast<int>(1.402 * v), 255);
                int g = qBound(0, yVal - static_cast<int>(0.344 * u - 0.714 * v), 255);
                int b = qBound(0, yVal + static_cast<int>(1.772 * u), 255);

                scanLine[x] = qRgb(r, g, b);
            }
        }
    }

    //qDebug() << "YUV转换成功:" << image.width() << "x" << image.height();
    return image;
}

/**
 * @brief YUYV到RGB的转换实现
 * 
 * YUYV（YUY2）格式布局：
 * 每4个字节表示2个像素：Y0 U0 Y1 V0
 * - Y0, Y1：两个像素的亮度值
 * - U0, V0：两个像素共享的色度值（4:2:2下采样）
 * 
 * 数据排列：YUYV YUYV YUYV ...
 */
QImage VideoFrameConverter::convertYUYVToRGB(const QVideoFrame &frame)
{
    int width = frame.width();
    int height = frame.height();
    const uchar *data = frame.bits(0);

    QImage image(width, height, QImage::Format_RGB32);
    if (image.isNull()) {
        return QImage();
    }

    // 逐行处理
    for (int y = 0; y < height; ++y) {
        QRgb *scanLine = reinterpret_cast<QRgb *>(image.scanLine(y));
        // 每次处理2个像素（YUYV每4字节表示2像素）
        for (int x = 0; x < width; x += 2) {
            // 计算在原始数据中的字节索引
            // 每行有width个像素，每个像素2字节（YUYV平均每像素2字节）
            int index = y * width * 2 + x * 2;
            
            // 提取YUYV分量
            int y0 = data[index];        // 第一个像素的Y
            int u = data[index + 1] - 128; // 共享的U
            int y1 = data[index + 2];    // 第二个像素的Y
            int v = data[index + 3] - 128; // 共享的V

            // 第一个像素的RGB转换
            int r0 = qBound(0, y0 + static_cast<int>(1.402 * v), 255);
            int g0 = qBound(0, y0 - static_cast<int>(0.344 * u - 0.714 * v), 255);
            int b0 = qBound(0, y0 + static_cast<int>(1.772 * u), 255);

            // 第二个像素的RGB转换（使用相同的UV）
            int r1 = qBound(0, y1 + static_cast<int>(1.402 * v), 255);
            int g1 = qBound(0, y1 - static_cast<int>(0.344 * u - 0.714 * v), 255);
            int b1 = qBound(0, y1 + static_cast<int>(1.772 * u), 255);

            // 写入两个像素
            scanLine[x] = qRgb(r0, g0, b0);
            if (x + 1 < width) {
                scanLine[x + 1] = qRgb(r1, g1, b1);
            }
        }
    }

    //qDebug() << "YUYV转换成功:" << image.width() << "x" << image.height();
    return image;
}
