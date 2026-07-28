#include "videoframeconverter.h"
#include <QTransform>

VideoFrameConverter::VideoFrameConverter() {}

QImage VideoFrameConverter::convertToQImage(const QVideoFrame &frame)
{
    if (!frame.isValid()) {
        return QImage();
    }

    QVideoFrame cloneFrame = frame;
    QImage image = cloneFrame.toImage();

    if (image.isNull() || image.width() == 0 || image.height() == 0) {
        return QImage();
    }

    if (m_rotationAngle != 0) {
        image = image.transformed(QTransform().rotate(m_rotationAngle));
    }
    return image;
}

void VideoFrameConverter::setRotation(int degrees)
{
    int clamped = degrees % 360;
    if (clamped < 0) clamped += 360;
    clamped = (clamped / 90) * 90;
    if (clamped != m_rotationAngle) {
        m_rotationAngle = clamped;
    }
}

int VideoFrameConverter::rotation() const
{
    return m_rotationAngle;
}
