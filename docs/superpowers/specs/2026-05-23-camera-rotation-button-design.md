# 摄像头旋转按钮 — 设计规格

**日期**: 2026-05-23
**状态**: 已确认

## 概述

在设置界面的"人脸识别"页面添加摄像头旋转按钮，每次点击旋转 90°（0° → 90° → 180° → 270° 循环），实时预览生效并持久化到配置文件。

## 架构

### VideoFrameConverter：静态 → 实例

```
// 改动前
class VideoFrameConverter {
public:
    static QImage convertToQImage(const QVideoFrame frame);
};

// 改动后
class VideoFrameConverter {
public:
    QImage convertToQImage(const QVideoFrame frame);
    void setRotation(int degrees);   // 仅接受 0/90/180/270
    int rotation() const;
private:
    int m_rotationAngle = 0;
};
```

- `convertToQImage()` 内部使用 `QTransform().rotate(m_rotationAngle)` 旋转帧
- `setRotation()` 约束值为 0/90/180/270 之一

### 依赖注入链路

```
MainWindow
  ├── 创建 VideoFrameConverter (堆上，this 为父对象)
  ├── 注入到 VideoFrameCapture → 帧转换时使用
  └── 注入到 SetWindow → 点击旋转按钮时调用 setRotation()
```

### 数据流

```
[设置界面点击旋转按钮]
  → SetWindow::onRotationClicked()
    → m_rotationAngle = (m_rotationAngle + 90) % 360
    → updateRotationDisplay()
    → m_converter->setRotation(angle)       // 立即生效
    → ConfigManager::instance()->setCameraRotation(angle)
    → ConfigManager::instance()->saveConfig()  // 立即持久化到文件

[视频帧到达]
  → VideoFrameCapture::processFrame()
    → m_converter->convertToQImage(frame)   // 内部使用 m_rotationAngle
    → emit frameCaptured(image)

[设置窗口打开 / 保存]
  → 打开: loadFromConfig() → converter->setRotation(angle) → updateDisplay()
  → 保存: saveToConfig() → ConfigManager 全部写盘
```

## 文件改动

### 1. `CameraCapture/videoframeconverter.h/.cpp`

- 移除 `static` 关键字
- 新增 `void setRotation(int degrees)` 和 `int rotation() const`
- 新增成员 `int m_rotationAngle = 0`
- `convertToQImage()` 使用 `rotate(m_rotationAngle)` 替代 `rotate(0)`

### 2. `CameraCapture/videoframecapture.h/.cpp`

- 新增 `void setFrameConverter(VideoFrameConverter *converter)`
- 新增成员 `VideoFrameConverter* m_converter = nullptr`
- `processFrame()` 中通过 `m_converter->convertToQImage()` 调用（非静态）

### 3. `config/configmanager.h/.cpp`

- 新增 `int m_cameraRotation = 0` 成员和默认值常量 `DEFAULT_CAMERA_ROTATION = 0`
- 新增 `int getCameraRotation() const` / `void setCameraRotation(int degrees)`
- 在 `loadConfig()` 中从 `[FaceRecognition]` section 读取 `CameraRotation`
- 在 `saveConfig()` 中写入 `CameraRotation`
- 在 `restoreDefaults()` 中重置为 0

### 4. `UI/setwindow.ui`

在人脸识别页面（`pageFace`）的 `arcfaceGroup` 下方新增：

```xml
<widget class="QGroupBox" name="rotationGroup">
  <property name="title"><string>摄像头方向</string></property>
  <layout class="QVBoxLayout">
    <item>
      <layout class="QHBoxLayout" name="rotationLayout">
        <item><widget class="QLabel" name="labelRotation"><text>当前角度</text></widget></item>
        <item><widget class="QLineEdit" name="lineEditRotation"><readOnly>true</readOnly></widget></item>
        <item><widget class="QPushButton" name="btnRotate"><text>旋转 90°</text></widget></item>
        <item><spacer/></item>
      </layout>
    </item>
    <item><widget class="QLabel" name="labelRotationHint"><text>每次点击旋转 90°（0° → 90° → 180° → 270° 循环）</text></widget></item>
  </layout>
</widget>
```

### 5. `UI/setwindow.h/.cpp`

- 新增 `void setFrameConverter(VideoFrameConverter *converter)`
- 新增成员 `VideoFrameConverter* m_converter = nullptr` 和 `int m_cameraRotation = 0`
- 新增槽 `void onBtnRotateClicked()`
- 新增方法 `void updateRotationDisplay()`
- 在 `setupConnections()` 中连接 `btnRotate::clicked`
- 在 `loadFromConfig()` / `saveToConfig()` / `restoreDefaults()` 中处理 `m_cameraRotation`

### 6. `mainwindow.cpp`

- 创建 `VideoFrameConverter* converter = new VideoFrameConverter(this)`
- `m_VideoFrameCapture->setFrameConverter(converter)`
- `setwindow->setFrameConverter(converter)`

## 配置存储

配置项 `CameraRotation` 写入 `config.ini` 的 `[FaceRecognition]` section（与其他人脸相关配置保持一致）：

```ini
[FaceRecognition]
CameraRotation=90
```

## 约束

- 旋转角度仅接受 0, 90, 180, 270，非法值静默取最近的有效值
- 旋转操作立即调用 `ConfigManager::saveConfig()` 持久化到文件（用户选择了"实时预览 + 持久化"）
- `FaceVideoWidget` 的 paintEvent 基于帧尺寸动态计算宽高比和缩放因子，旋转后的竖屏帧自动适配，无需改动
- `FaceRecognizer` 直接接收 QImage，无需改动

## 下游影响

无。`FaceVideoWidget::paintEvent` 使用 `frame.width()/height()` 动态计算 `scaleX/scaleY`，人脸框坐标通过 `boundingBox * scale` 映射，旋转后的帧尺寸自动适配。
