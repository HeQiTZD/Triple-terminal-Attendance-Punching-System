# 摄像头画面竖屏显示设计

## 目标

将摄像头画面从横屏（landscape）改为竖屏（portrait）显示，人脸识别也在竖屏画面上进行。

## 方案

在 `VideoFrameConverter::convertToQImage()` 中，`toImage()` 之后将图像顺时针旋转 90°，使 640×480 的横屏画面变为 480×640 的竖屏画面。

## 改动

**单文件：`CameraCapture/videoframeconverter.cpp`**

在 `convertToQImage` 返回前增加旋转：

```cpp
image = image.transformed(QTransform().rotate(90));
```

## 下游影响

无需任何改动：

- `FaceVideoWidget::paintEvent` — 已基于帧尺寸动态计算宽高比和缩放比例
- 人脸框坐标映射 — `scaleX/scaleY` 基于帧尺寸动态计算
- `FaceRecognizer` — 直接接收竖屏 QImage
