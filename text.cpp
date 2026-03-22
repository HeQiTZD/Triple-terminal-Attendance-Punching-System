#include "text.h"
#include <QFile>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QBuffer>

FaceRegistrationTest::FaceRegistrationTest(QObject *parent)
    : QObject(parent)
{
}

bool FaceRegistrationTest::registerFace(const QString &imagePath,
                                        const QString &employeeId,
                                        const QString &name,
                                        const QString &department)
{
    // 步骤1：检查引擎
    if (!checkEngine()) {
        return false;
    }

    // 步骤2：加载图片
    QImage image(imagePath);
    if (image.isNull()) {
        m_lastError = QString("无法加载图片: %1").arg(imagePath);
        qWarning() << m_lastError;
        return false;
    }
    qDebug() << "图片加载成功:" << imagePath
             << "尺寸:" << image.size()
             << "格式:" << image.format();

    // 步骤3：人脸检测
    arcfaceengine* engine = arcfaceengine::instance();
    QVector<arcfaceengine::FaceInfo> faces = engine->detectFace(image);

    if (faces.isEmpty()) {
        m_lastError = "未检测到人脸";
        qWarning() << m_lastError;
        return false;
    }
    if (faces.size() > 1) {
        m_lastError = QString("检测到多个人脸(%1个)，请使用单人照片").arg(faces.size());
        qWarning() << m_lastError;
        return false;
    }
    qDebug() << "人脸检测成功，位置:" << faces[0].rect;

    // 步骤4：特征提取
    arcfaceengine::FaceFeature feature = engine->extractFeature(image, faces[0]);
    if (feature.data.isEmpty() || feature.size == 0) {
        m_lastError = "特征提取失败";
        qWarning() << m_lastError;
        return false;
    }
    qDebug() << "特征提取成功，大小:" << feature.size << "字节";

    // 步骤5：保存人员基本信息
    if (!addPersonInfo(employeeId, name, department)) {
        return false;
    }

    // 步骤6：保存特征数据
    if (!saveToDatabase(employeeId, name, department, feature)) {
        return false;
    }

    qDebug() << "人脸注册成功:" << name << "(" << employeeId << ")";
    return true;
}

int FaceRegistrationTest::batchRegisterFromFolder(const QString &folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        m_lastError = "文件夹不存在: " + folderPath;
        qWarning() << m_lastError;
        return 0;
    }

    // 获取所有图片文件
    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);

    int successCount = 0;
    for (const QFileInfo &file : files) {
        // 解析文件名：员工ID_姓名.jpg
        QString baseName = file.baseName();  // 去掉扩展名
        QStringList parts = baseName.split("_");

        if (parts.size() >= 2) {
            QString employeeId = parts[0];
            QString name = parts[1];

            qDebug() << "处理文件:" << file.fileName()
                     << "ID:" << employeeId
                     << "姓名:" << name;

            if (registerFace(file.absoluteFilePath(), employeeId, name)) {
                successCount++;
            }
        } else {
            qWarning() << "文件名格式错误，跳过:" << file.fileName();
        }
    }

    qDebug() << "批量注册完成，成功:" << successCount << "/" << files.size();
    return successCount;
}

bool FaceRegistrationTest::checkEngine()
{
    arcfaceengine* engine = arcfaceengine::instance();

    if (!engine->isInitialized()) {
        // 尝试初始化
        QString appId = "JBT9EUHsd8RVuvbgwNLNFP1ezsdtsuUenhD6gjSkoKhG";
        QString sdkKey = "4szkxxMUBVRLirbAsTMzT9u2b5R9w5umHiucbPvTy91Z";

        if (!engine->initialize(appId, sdkKey)) {
            m_lastError = "ArcFace引擎初始化失败";
            qWarning() << m_lastError;
            return false;
        }
        qDebug() << "ArcFace引擎初始化成功";
    }
    return true;
}

bool FaceRegistrationTest::addPersonInfo(const QString &employeeId,
                                         const QString &name,
                                         const QString &department)
{
    LocalStorage* storage = LocalStorage::instance();

    QSqlQuery query(storage->connectDatabse() ?
                        QSqlDatabase::database() : QSqlDatabase());

    query.prepare(R"(
        INSERT OR REPLACE INTO persons (employee_id, name, department, created_at)
        VALUES (:employee_id, :name, :department, :created_at)
    )");

    query.bindValue(":employee_id", employeeId);
    query.bindValue(":name", name);
    query.bindValue(":department", department);
    query.bindValue(":created_at", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        m_lastError = QString("保存人员信息失败: %1").arg(query.lastError().text());
        qWarning() << m_lastError;
        return false;
    }

    qDebug() << "人员信息保存成功:" << name;
    return true;
}

bool FaceRegistrationTest::saveToDatabase(const QString &employeeId,
                                          const QString &name,
                                          const QString &department,
                                          const arcfaceengine::FaceFeature &feature)
{
    LocalStorage* storage = LocalStorage::instance();

    QSqlQuery query(storage->connectDatabse() ?
                        QSqlDatabase::database() : QSqlDatabase());

    query.prepare(R"(
        INSERT OR REPLACE INTO face_features
        (employee_id, feature_data, feature_size, created_at)
        VALUES (:employee_id, :feature_data, :feature_size, :created_at)
    )");

    query.bindValue(":employee_id", employeeId);
    query.bindValue(":feature_data", feature.data);
    query.bindValue(":feature_size", feature.size);
    query.bindValue(":created_at", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        m_lastError = QString("保存特征数据失败: %1").arg(query.lastError().text());
        qWarning() << m_lastError;
        return false;
    }

    qDebug() << "特征数据保存成功，大小:" << feature.size << "字节";
    return true;
}
