#include "facedatabasemanager.h"

FaceDatabaseManager::FaceDatabaseManager(QObject *parent) {}

//提供唯一实例化接口
FaceDatabaseManager* FaceDatabaseManager::instance()
{
    static FaceDatabaseManager* s_instance = nullptr;
    if(!s_instance){
        s_instance = new FaceDatabaseManager();
    }
    return s_instance;
}

//加载人脸特性到内存
bool FaceDatabaseManager::loadFromDatabase()
{
    QMutexLocker locker(&m_mutex);

    //清空现有内存
    m_faceRecords.clear();

    QSqlQuery query;//QSqkQuery用于执行SQL语句和处理查询结构

    //prepare用于预编译语句,可与bindvalue()结合使用进行参数绑定，防止SQL注入
    query.prepare("select employee_id,name,face_feature,face_feature_size from Person");

    //exec()执行成功后，结果集自动存储在query对象内部
    if(!query.exec()){
        qWarning()<<"加载人脸特征失败"<<query.lastError().text();
        return false;
    }

    //next()遍历结果集，移动到下一行记录，初始位置为第一行之前，到最后一行之后返回false
    //value()提取当前行的某一列
    while(query.next()){
        FaceRecord record;
        record.personId =query.value("employee_id").toString();
        record.name =query.value("name").toString();
        record.featureData =query.value("face_feature").toByteArray();
        record.featureSize =query.value("face_feature_size").toInt();

        //append()向数组末尾添加一个元素
        m_faceRecords.append(record);
    }

    return true;
}

void FaceDatabaseManager::clear()
{
    QMutexLocker locker(&m_mutex);
    m_faceRecords.clear();
}

QPair<QString, float> FaceDatabaseManager::findBestMatch(const arcfaceengine::FaceFeature &targetFeature)
{
    //防止多线程同时访问 m_faceRecords 导致数据混乱
    //原理 ：自动加锁，函数结束时自动解锁
    QMutexLocker locker(&m_mutex);

    QString bestMatchId;//代表最佳匹配的人员ID ，用于存储人脸比对后找到的最相似的人。
    float maxSimilarity = 0.0f;//初始化匹配值

    arcfaceengine* engine = arcfaceengine::instance();

    //C++范围for循环写法，用于便利容器元素。
    //auto&自动推导类型 + 引用（避免拷贝）。record循环变量名，用来访问当前元素的名字 ，就像给每个元素起个临时代号
    for(const auto& record : m_faceRecords){
        arcfaceengine::FaceFeature dbFeature;
        dbFeature.data = record.featureData;
        dbFeature.size = record.featureSize;

        //进行对比
        float similarity = engine->compareFeatures(targetFeature,dbFeature);

        //有更大的匹配度，更新maxSimilarity，记录对比的人脸特征的ID
        if(similarity > maxSimilarity){
            maxSimilarity = similarity;
            bestMatchId = record.personId;
        }
    }

    //qMakePair()将两个值打包成一个配对（pair）返回
    return qMakePair(bestMatchId,maxSimilarity);
}

int FaceDatabaseManager::getPersonCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_faceRecords.size();
}
