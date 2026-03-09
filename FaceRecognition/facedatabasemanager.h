#ifndef FACEDATABASEMANAGER_H
#define FACEDATABASEMANAGER_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlError>
#include "arcfaceengine.h"

class FaceDatabaseManager : public QObject
{
    Q_OBJECT
private:
    explicit FaceDatabaseManager(QObject* parent = nullptr);

public:

    //内存中的人脸信息结构体
    struct FaceRecord{
        QString personId;//员工ID
        QString name;//姓名
        QByteArray featureData;//人脸特征数据（二进制）
        int featureSize;//特征数据大小
    };

    static FaceDatabaseManager* instance();

    //程序启动时调用：从数据库加载所有特征到内存
    bool loadFromDatabase();

    //程序退出时调用：清空内存
    void clear();

    //快速1：N人脸对比
    //返回最佳匹配的人员ID和相似度
    QPair<QString,float> findBestMatch(const arcfaceengine::FaceFeature &targetFeature);

    //获取内存中的人数
    int getPersonCount() const;

private:
    QVector<FaceRecord> m_faceRecords;//内存特征库
    mutable QMutex m_mutex;//线程安全锁

};

#endif // FACEDATABASEMANAGER_H
