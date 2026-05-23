# 摄像头旋转按钮 实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在设置界面"人脸识别"页面添加摄像头旋转按钮，每次点击旋转 90°，实时预览生效并持久化到配置文件。

**Architecture:** VideoFrameConverter 从静态工具类改为实例类，持有 m_rotationAngle。MainWindow 创建 Converter 实例并注入到 VideoFrameCapture 和 SetWindow。SetWindow 点击按钮时更新 Converter 和 ConfigManager。

**Tech Stack:** Qt 6.10 C++17, Qt Widgets, CMake

---

### Task 1: VideoFrameConverter 静态→实例改造

**Files:**
- Modify: `CameraCapture/videoframeconverter.h`
- Modify: `CameraCapture/videoframeconverter.cpp`

- [ ] **Step 1: 修改头文件 — 移除 static，新增 rotation 接口**

将 `CameraCapture/videoframeconverter.h` 替换为：

```cpp
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
```

- [ ] **Step 2: 修改实现文件 — 实例方法 + 旋转逻辑**

将 `CameraCapture/videoframeconverter.cpp` 替换为：

```cpp
#include "videoframeconverter.h"
#include <QTransform>
#include <QDebug>

VideoFrameConverter::VideoFrameConverter() {}

QImage VideoFrameConverter::convertToQImage(const QVideoFrame frame)
{
    if (!frame.isValid()) {
        qWarning() << "视频帧无效";
        return QImage();
    }

    QVideoFrame cloneFrame = frame;
    QImage image = cloneFrame.toImage();

    if (image.isNull() || image.width() == 0 || image.height() == 0) {
        qWarning() << "视频帧转换失败，格式:" << frame.pixelFormat();
        return QImage();
    }

    if (m_rotationAngle != 0) {
        image = image.transformed(QTransform().rotate(m_rotationAngle));
    }
    return image;
}

void VideoFrameConverter::setRotation(int degrees)
{
    // 约束为 0/90/180/270
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
```

- [ ] **Step 3: 提交**

```bash
git add CameraCapture/videoframeconverter.h CameraCapture/videoframeconverter.cpp
git commit -m "refactor: convert VideoFrameConverter from static to instance with rotation support"
```

---

### Task 2: VideoFrameCapture 接受 Converter 注入

**Files:**
- Modify: `CameraCapture/videoframecapture.h`
- Modify: `CameraCapture/videoframecapture.cpp`

- [ ] **Step 1: 修改头文件 — 新增 setFrameConverter 和成员**

在 `CameraCapture/videoframecapture.h` 中，`public:` 区域新增方法声明，`private:` 区域新增成员：

在 `public:` 区域（`getVideoWidget()` 之后）添加：
```cpp
    void setFrameConverter(VideoFrameConverter *converter);
```

在 `private:` 区域（`CameraCapture cameraCaptrue;` 之后）添加：
```cpp
    VideoFrameConverter *m_converter = nullptr;
```

- [ ] **Step 2: 修改实现文件 — 使用注入的 converter**

在 `CameraCapture/videoframecapture.cpp` 中：

添加 setter 实现（放在文件末尾或 `processFrame` 之前）：
```cpp
void VideoFrameCapture::setFrameConverter(VideoFrameConverter *converter)
{
    m_converter = converter;
}
```

修改 `processFrame` 方法（第 52-60 行）：
```cpp
void VideoFrameCapture::processFrame(const QVideoFrame &frame)
{
    QImage image;
    if (m_converter) {
        image = m_converter->convertToQImage(frame);
    } else {
        image = VideoFrameConverter().convertToQImage(frame);
    }

    if (!image.isNull() && image.width() > 0 && image.height() > 0) {
        currentFrame = image;
        emit frameCaptured(currentFrame);
    }
}
```

- [ ] **Step 3: 提交**

```bash
git add CameraCapture/videoframecapture.h CameraCapture/videoframecapture.cpp
git commit -m "feat: add converter injection to VideoFrameCapture"
```

---

### Task 3: ConfigManager 新增 cameraRotation 配置项

**Files:**
- Modify: `config/configmanager.h`
- Modify: `config/configmanager.cpp`

- [ ] **Step 1: 修改头文件 — 新增 getter/setter 和成员**

在 `config/configmanager.h` 中：

在 Getter 区域（人脸识别设置 Getter 之后，ArcFace SDK 配置 Getter 之前）添加：
```cpp
    int getCameraRotation() const { return m_cameraRotation; }
```

在 Setter 区域（人脸识别设置 Setter 之后，ArcFace SDK 配置 Setter 之前）添加：
```cpp
    void setCameraRotation(int degrees) { m_cameraRotation = degrees; }
```

在 private 成员区域（`m_recognizeTimeout` 之后）添加：
```cpp
    int m_cameraRotation;
```

在默认值常量区域（`DEFAULT_RECOGNIZE_TIMEOUT` 之后）添加：
```cpp
    static constexpr int DEFAULT_CAMERA_ROTATION = 0;
```

- [ ] **Step 2: 修改实现文件 — load/save/restore**

在 `config/configmanager.cpp` 中：

1. `loadConfig()` — 在 `[FaceRecognition]` section 的 `m_settings->endGroup()` 之前（第 98 行前）添加：
```cpp
        m_cameraRotation = m_settings->value("CameraRotation", DEFAULT_CAMERA_ROTATION).toInt();
```

2. `saveConfig()` — 在 `[FaceRecognition]` section 的 `m_settings->endGroup()` 之前（第 170 行前）添加：
```cpp
        m_settings->setValue("CameraRotation", m_cameraRotation);
```

3. `restoreDefaults()` — 在恢复人脸识别设置区域（第 257 行之后）添加：
```cpp
    m_cameraRotation = DEFAULT_CAMERA_ROTATION;
```

- [ ] **Step 3: 提交**

```bash
git add config/configmanager.h config/configmanager.cpp
git commit -m "feat: add cameraRotation to ConfigManager"
```

---

### Task 4: 设置界面 UI — 新增摄像头方向控件

**Files:**
- Modify: `UI/setwindow.ui`

- [ ] **Step 1: 在 facePage 中添加 rotationGroup**

在 `UI/setwindow.ui` 中，找到 `pageFace` 的 `facePageLayout`。

在 `arcfaceGroup` widget（第 548 行 `</widget>` 闭合标签之后、`faceSpacer` 之前）插入以下内容：

定位方式：搜索 `<widget class="QGroupBox" name="arcfaceGroup">`，找到其闭合标签 `</widget>`（大约第 606 行），在其后、`<spacer name="faceSpacer">` 之前插入：

```xml
          <item>
           <widget class="QGroupBox" name="rotationGroup">
            <property name="title">
             <string>摄像头方向</string>
            </property>
            <layout class="QVBoxLayout" name="rotationGroupLayout">
             <property name="spacing">
              <number>10</number>
             </property>
             <item>
              <layout class="QHBoxLayout" name="rotationLayout">
               <item>
                <widget class="QLabel" name="labelRotation">
                 <property name="minimumSize">
                  <size>
                   <width>120</width>
                   <height>0</height>
                  </size>
                 </property>
                 <property name="text">
                  <string>当前角度</string>
                 </property>
                </widget>
               </item>
               <item>
                <widget class="QLineEdit" name="lineEditRotation">
                 <property name="readOnly">
                  <bool>true</bool>
                 </property>
                 <property name="maximumSize">
                  <size>
                   <width>80</width>
                   <height>16777215</height>
                  </size>
                 </property>
                 <property name="alignment">
                  <set>Qt::AlignmentFlag::AlignCenter</set>
                 </property>
                </widget>
               </item>
               <item>
                <widget class="QPushButton" name="btnRotate">
                 <property name="text">
                  <string>旋转 90°</string>
                 </property>
                 <property name="minimumSize">
                  <size>
                   <width>100</width>
                   <height>0</height>
                  </size>
                 </property>
                </widget>
               </item>
               <item>
                <spacer name="rotationSpacer">
                 <property name="orientation">
                  <enum>Qt::Orientation::Horizontal</enum>
                 </property>
                </spacer>
               </item>
              </layout>
             </item>
             <item>
              <widget class="QLabel" name="labelRotationHint">
               <property name="text">
                <string>每次点击旋转 90°（0° → 90° → 180° → 270° 循环）</string>
               </property>
               <property name="styleSheet">
                <string notr="true">color: #888888; font-size: 11px;</string>
               </property>
              </widget>
             </item>
            </layout>
           </widget>
          </item>
```

- [ ] **Step 2: 提交**

```bash
git add UI/setwindow.ui
git commit -m "feat: add camera rotation controls to settings UI"
```

---

### Task 5: 设置界面逻辑 — 旋转按钮交互

**Files:**
- Modify: `UI/setwindow.h`
- Modify: `UI/setwindow.cpp`

- [ ] **Step 1: 修改头文件 — 新增成员和方法声明**

在 `UI/setwindow.h` 中：

在 `#include` 区域（文件头部）添加前向声明或 include：
```cpp
class VideoFrameConverter;
```

在 `public:` 区域（`restoreDefaults()` 之后）添加：
```cpp
    void setFrameConverter(VideoFrameConverter *converter);
```

在 `private slots:` 区域（`onBtnDisconnectClicked()` 之后）添加：
```cpp
    void onBtnRotateClicked();
```

在 `private:` 区域底部（`m_syncTimeout` 之后）添加：
```cpp
    VideoFrameConverter* m_converter = nullptr;
    int m_cameraRotation = 0;
```

在成员方法区域添加：
```cpp
    void updateRotationDisplay();
```

- [ ] **Step 2: 修改实现文件 — 实现旋转逻辑**

在 `UI/setwindow.cpp` 中：

1. 在文件头部 include 区域添加：
```cpp
#include "../CameraCapture/videoframeconverter.h"
```

2. 在 `setupConnections()` 方法中（`connect(ui->btnDisconnect, ...)` 之后）添加：
```cpp
    connect(ui->btnRotate, &QPushButton::clicked, this, &SetWindow::onBtnRotateClicked);
```

3. 在构造函数中（`setupConnections()` 调用之后，`loadFromConfig()` 之前）立即应用按钮主题：
在 `applyButtonTheme(ui->btnRestore, ...)` 代码块之后添加：
```cpp
    // 旋转按钮 → Primary
    applyButtonTheme(ui->btnRotate,
        tm->colorHex(DesignTokens::Semantic::brandPrimary),
        "#ffffff",
        "",
        tm->colorHex(DesignTokens::Semantic::brandPrimaryHover),
        tm->colorHex(DesignTokens::Semantic::brandPrimaryActive),
        false);
```

4. 添加 `setFrameConverter` 方法（放在文件末尾，`onBtnDisconnectClicked` 之后）：
```cpp
void SetWindow::setFrameConverter(VideoFrameConverter *converter)
{
    m_converter = converter;
    if (m_converter) {
        m_converter->setRotation(m_cameraRotation);
    }
}
```

5. 添加 `updateRotationDisplay` 方法：
```cpp
void SetWindow::updateRotationDisplay()
{
    ui->lineEditRotation->setText(QString::number(m_cameraRotation) + "°");
}
```

6. 添加 `onBtnRotateClicked` 槽函数：
```cpp
void SetWindow::onBtnRotateClicked()
{
    m_cameraRotation = (m_cameraRotation + 90) % 360;

    if (m_converter) {
        m_converter->setRotation(m_cameraRotation);
    }

    updateRotationDisplay();

    ConfigManager::instance()->setCameraRotation(m_cameraRotation);
    ConfigManager::instance()->saveConfig();
}
```

7. 在 `loadFromConfig()` 方法中（`m_syncTimeout = config->getSyncTimeout();` 之后）添加：
```cpp
    m_cameraRotation = config->getCameraRotation();
```

8. 在 `saveToUI()` 方法中（同步设置 UI 更新之后，方法末尾 `}` 之前）添加：
```cpp
    updateRotationDisplay();
```

9. 在 `restoreDefaults()` 方法中（`m_syncTimeout = 300;` 之后）添加：
```cpp
    m_cameraRotation = 0;
```

10. **重要：** `loadFromUI()` 中**不**读取 `m_cameraRotation`（旋转角度不通过"保存设置"按钮写盘，实时持久化）

- [ ] **Step 3: 提交**

```bash
git add UI/setwindow.h UI/setwindow.cpp
git commit -m "feat: implement camera rotation button logic in SetWindow"
```

---

### Task 6: MainWindow 创建并注入 Converter

**Files:**
- Modify: `mainwindow.h`
- Modify: `mainwindow.cpp`

- [ ] **Step 1: 修改头文件 — 新增成员**

在 `UI/setwindow.h` include 之后添加前向声明：
```cpp
class VideoFrameConverter;
```

在 private 成员区域（`SetWindow* setwindow = nullptr;` 之后）添加：
```cpp
    VideoFrameConverter* m_frameConverter = nullptr;
```

- [ ] **Step 2: 修改实现文件 — 创建并注入**

在 `mainwindow.cpp` 中：

1. 在文件头部 include 区域添加：
```cpp
#include "CameraCapture/videoframeconverter.h"
```

2. 在构造函数中，`setwindow = new SetWindow(this);`（第 64 行）之后添加：
```cpp
    m_frameConverter = new VideoFrameConverter(this);
    setwindow->setFrameConverter(m_frameConverter);
```

3. 在 `init()` 方法中，`m_VideoFrameCapture = new VideoFrameCapture(this);`（第 275 行）之后添加：
```cpp
    m_VideoFrameCapture->setFrameConverter(m_frameConverter);
```

- [ ] **Step 3: 提交**

```bash
git add mainwindow.h mainwindow.cpp
git commit -m "feat: create VideoFrameConverter in MainWindow and inject to consumers"
```

---

### Task 7: 构建验证

**Files:** 无

- [ ] **Step 1: 构建项目**

```bash
cmake --build build/Desktop_Qt_6_10_2_MinGW_64_bit-Debug --target AttendanceSystem
```

预期：编译成功，无错误。

- [ ] **Step 2: 验证功能点**

启动应用后手动验证：
1. 打开设置窗口 → 人脸识别页面 → 看到"摄像头方向"分组
2. 当前角度显示 "0°"
3. 点击"旋转 90°"按钮 → 摄像头画面立即旋转 90°
4. 角度显示更新为 "90°"
5. 继续点击 → 180°、270°、0° 循环
6. 关闭设置窗口 → 重新打开 → 角度保持上次的值（持久化）
7. 点击"恢复默认" → 角度恢复为 0°
8. 检查 `build/.../config/config.ini` 中 `[FaceRecognition]` section 的 `CameraRotation` 值

---

### 文件改动汇总

| 文件 | 改动类型 |
|------|---------|
| `CameraCapture/videoframeconverter.h` | 静态→实例，新增 setRotation/rotation |
| `CameraCapture/videoframeconverter.cpp` | 实例方法，rotate(m_rotationAngle) |
| `CameraCapture/videoframecapture.h` | 新增 setFrameConverter, m_converter |
| `CameraCapture/videoframecapture.cpp` | processFrame 使用注入的 converter |
| `config/configmanager.h` | 新增 getCameraRotation/setCameraRotation |
| `config/configmanager.cpp` | load/save/restoreDefaults |
| `UI/setwindow.ui` | 新增 rotationGroup 控件 |
| `UI/setwindow.h` | 新增 converter 注入、槽函数、成员 |
| `UI/setwindow.cpp` | 实现旋转按钮交互逻辑 |
| `mainwindow.h` | 新增 m_frameConverter 成员 |
| `mainwindow.cpp` | 创建 converter 并注入到 SetWindow/VideoFrameCapture |
