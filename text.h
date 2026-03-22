#ifndef FACEREGISTRATIONTEST_H
#define FACEREGISTRATIONTEST_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QDebug>
#include "FaceRecognition/arcfaceengine.h"
#include "LocalStorage/localstorage.h"

/**
 * @brief 人脸注册测试类
 *
 * 功能：提供一张照片，提取人脸特征并保存到数据库
 * 使用场景：测试人员信息录入、人脸特征提取功能验证
 */
class FaceRegistrationTest : public QObject
{
    Q_OBJECT

public:
    explicit FaceRegistrationTest(QObject *parent = nullptr);

    /**
     * @brief 注册单个人脸
     *
     * @param imagePath 照片文件路径
     * @param employeeId 员工ID
     * @param name 员工姓名
     * @param department 部门（可选）
     * @return 注册成功返回true
     *
     * 示例：
     * FaceRegistrationTest tester;
     * tester.registerFace("D:/photos/zhangsan.jpg", "10001", "张三", "技术部");
     */
    bool registerFace(const QString &imagePath,
                      const QString &employeeId,
                      const QString &name,
                      const QString &department = QString());

    /**
     * @brief 批量注册人脸（从文件夹）
     *
     * @param folderPath 照片文件夹路径
     * @param nameList 人员信息列表（与照片文件名对应）
     * @return 成功注册的数量
     *
     * 照片命名格式：员工ID_姓名.jpg
     * 示例：10001_张三.jpg
     */
    int batchRegisterFromFolder(const QString &folderPath);

    /**
     * @brief 获取最后一次错误信息
     */
    QString lastError() const { return m_lastError; }

private:
    /**
     * @brief 验证引擎是否已初始化
     */
    bool checkEngine();

    /**
     * @brief 将特征数据保存到数据库
     */
    bool saveToDatabase(const QString &employeeId,
                        const QString &name,
                        const QString &department,
                        const arcfaceengine::FaceFeature &feature);

    /**
     * @brief 添加人员基本信息到数据库
     */
    bool addPersonInfo(const QString &employeeId,
                       const QString &name,
                       const QString &department);

private:
    QString m_lastError;  // 最后一次错误信息
};

#endif // FACEREGISTRATIONTEST_H
