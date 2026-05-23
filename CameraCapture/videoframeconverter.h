#ifndef VIDEOFRAMECONVERTER_H
#define VIDEOFRAMECONVERTER_H

#include <QImage>
#include <QVideoFrame>

class VideoFrameConverter
{
public:
    VideoFrameConverter();

    QImage convertToQImage(const QVideoFrame frame);

    void setRotation(int degrees);
    int rotation() const;

private:
    int m_rotationAngle = 0;
};

#endif // VIDEOFRAMECONVERTER_H
