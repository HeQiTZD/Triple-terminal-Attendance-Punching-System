#ifndef ARCFACEENGINE_H
#define ARCFACEENGINE_H

#include <QObject>
#include <QRect>
#include <QImage>
#include <QVector>
#include <QByteArray>
#include <QDebug>
#include <QMutex>

#include "arcsoft_face_sdk.h"
#include "merror.h"

/**
 * @brief ArcFace人脸识别引擎封装类
 * 
 * 提供人脸检测、特征提取、特征对比等功能
 * 采用单例模式，全局只有一个引擎实例
 */
class arcfaceengine
{
public:
    /**
     * @brief 人脸信息结构体
     * 存储单张人脸的检测结果
     */
    struct FaceInfo{
        QRect rect;     // 人脸矩形区域（左上角坐标 + 宽高）
        int orient;     // 人脸角度方向（0-360度，用于特征提取时的角度校正）
        int faceId;     // 人脸追踪ID（视频模式下用于追踪同一人脸）
    };

    /**
     * @brief 人脸特征结构体
     * 存储提取的人脸特征向量
     */
    struct FaceFeature{
        QByteArray data;    // 特征数据（二进制格式，SDK内部定义）
        int size;           // 特征数据大小（字节）
        FaceFeature() : size(0) {}
    };

    /**
     * @brief 获取单例实例
     * @return 引擎实例指针
     */
    static arcfaceengine* instance();

    /**
     * @brief 初始化引擎
     * @param appId 应用ID（从虹软官网申请）
     * @param sdkKey SDK密钥（从虹软官网申请）
     * @return 初始化成功返回true
     */
    bool initialize(const QString &appId, const QString &sdkKey);

    /**
     * @brief 释放引擎资源
     */
    void uninitialize();

    /**
     * @brief 检查引擎是否已初始化
     * @return 已初始化返回true
     */
    bool isInitialized() const;

    /**
     * @brief 检测图像中的人脸
     * @param image 输入图像（支持多种QImage格式）
     * @return 检测到的人脸信息列表
     */
    QVector<FaceInfo> detectFace(const QImage &image);

    /**
     * @brief 从指定人脸区域提取特征
     * @param image 输入图像
     * @param faceInfo 人脸位置信息（由detectFace返回）
     * @return 人脸特征数据
     */
    FaceFeature extractFeature(const QImage &image, const FaceInfo &faceInfo);

    /**
     * @brief 对比两个人脸特征的相似度
     * @param feature1 第一个人脸特征
     * @param feature2 第二个人脸特征
     * @return 相似度分数（0.0-1.0，一般阈值0.8以上认为是同一人）
     */
    float compareFeatures(const FaceFeature &feature1, const FaceFeature &feature2);

private:
    // 单例模式：私有构造函数
    arcfaceengine();
    // 析构时自动释放引擎资源
    ~arcfaceengine();
    // 禁止拷贝
    arcfaceengine(const arcfaceengine&) = delete;
    arcfaceengine& operator=(const arcfaceengine&) = delete;

    /**
     * @brief 获取QImage对应的SDK像素格式
     */
    MInt32 getPixelFormat(const QImage& image);

    MHandle m_engine;           // SDK引擎句柄，所有SDK调用的核心
    bool m_initialized;         // 引擎初始化状态标志
    QImage m_convertedImage;    // 格式转换后的图像缓存（确保SDK调用期间数据有效）

    static QMutex s_mutex;      // 单例模式线程安全互斥锁
    static arcfaceengine* s_instance;  // 单例实例指针
};

#endif
