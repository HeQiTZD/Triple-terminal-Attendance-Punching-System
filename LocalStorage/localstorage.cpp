#include "localstorage.h"

//静态成员初始化
LocalStorage* LocalStorage::s_instance = nullptr;
QMutex LocalStorage::s_mutex;

//单例实现
LocalStorage* LocalStorage::instance()
{
    if(!s_instance){
        if(!s_instance){
            QMutexLocker locker(&s_mutex);
            s_instance = new LocalStorage();
        }
    }
    return s_instance;
}

//构造函数
LocalStorage::LocalStorage(QObject *parent) : QObject(parent) {}

//析构函数，关闭数据库
LocalStorage::~LocalStorage()
{
    if(m_db.isOpen()){
        m_db.close();
    }
}

bool LocalStorage::connectDatabse()
{
    //检查数据库目录是否存在，不存在则创建
    QString appDir=QCoreApplication::applicationDirPath();
    QDir dataDir(appDir);
    if(dataDir.mkpath(appDir+"/data")){
        qDebug()<<"目录创建成功或已存在";
    }else {
        qDebug() << "目录创建失败";
        return false;
    }

    QString dbFilePath=appDir+"/data/attendance.db";
    qDebug()<<"数据库路径"+dbFilePath;

    //连接数据库（文件不存在则自动创建）
    QSqlDatabase m_db=QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbFilePath);//为刚刚创建的数据库连接对象 m_db 指定要连接的数据库文件的具体路径。

    if(!m_db.open()){
        qDebug()<<"数据库打开失败";
        qDebug()<<m_db.lastError().text();
        return false;
    }

    //初始化数据库
    QSqlQuery query;
    //启用外键约束
    if(!query.exec("PRAGMA foreign_keys=ON;")){
        qDebug()<<"启用外键约束失败"<<query.lastError().text();
        return false;
    }
    //设置编码为UTF-8
    if(!query.exec("PRAGMA encoding = 'UTF-8';")){
        qDebug()<<"设置编码失败"<<query.lastError().text();
        return false;
    }

    //创建人员表
    QString createPersonTable = "CREATE TABLE IF NOT EXISTS Person(" 
                              "employee_id TEXT NOT NULL PRIMARY KEY," 
                              "name TEXT NOT NULL,"
                              "face_feature BLOB NOT NULL,"
                              "face_feature_size INTEGER NOT NULL,"
                              "last_updated DATETIME DEFAULT CURRENT_TIMESTAMP" 
                              ");";
    if(!query.exec(createPersonTable)){
        qDebug()<<"创建Person表失败"<<query.lastError().text();
        m_db.close();
        return false;
    }
    //创建人员表索引
    QString createPersonIndex = "CREATE INDEX IF NOT EXISTS idx_person_name ON Person(name);";
    if(!query.exec(createPersonIndex)){
        qDebug()<<"创建Person表索引失败"<<query.lastError().text();
        m_db.close();
        return false;
    }

    //创建打卡记录表
    QString createRecordTable = "CREATE TABLE IF NOT EXISTS AttendanceRecord(" 
                              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                              "employee_id TEXT NOT NULL," 
                              "check_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," 
                              "status TEXT NOT NULL DEFAULT '正常'," 
                              "uploaded INTEGER NOT NULL DEFAULT 0," 
                              "upload_time DATETIME," 
                              "FOREIGN KEY (employee_id) REFERENCES Person(employee_id) ON DELETE CASCADE,"
                              "CONSTRAINT ck_attendance_status CHECK (status IN ('正常','迟到','早退','缺勤'))"
                              ");";
    if(!query.exec(createRecordTable)){
        qDebug()<<"创建AttendanceRecord表失败"<<query.lastError().text();
        m_db.close();
        return false;
    }
    //创建打卡记录表索引
    QString createRecordIndex1 = "CREATE INDEX IF NOT EXISTS idx_record_employee_id ON AttendanceRecord(employee_id);";
    if(!query.exec(createRecordIndex1)){
        qDebug()<<"创建AttendanceRecord表索引1失败"<<query.lastError().text();
        m_db.close();
        return false;
    }
    QString createRecordIndex2 = "CREATE INDEX IF NOT EXISTS idx_record_check_time ON AttendanceRecord(check_time);";
    if(!query.exec(createRecordIndex2)){
        qDebug()<<"创建AttendanceRecord表索引2失败"<<query.lastError().text();
        m_db.close();
        return false;
    }
    
    qDebug()<<"数据库初始化成功";
    return true;
}

//人员数据同步
bool LocalStorage::syncPersons(const QVector<Protocol::PersonData> &persons)
{
    QMutexLocker locker(&s_mutex);

    if(!m_db.isOpen()){
        qDebug()<<"数据库未连接";
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    //开启事务
    if(!m_db.transaction()){
        qDebug()<<"开启事务失败"<<m_db.lastError().text();
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    qDebug()<<"开启数据同步，共"<<persons.size()<<"条";

    //1.清空数据库
    QSqlQuery query(m_db);//与预先定义好的数据库连接 m_db 关联起来
    if(!query.exec("delete from Person")){
        qWarning()<<"清空数据库失败:"<<query.lastError().text();
        return false;
    }
    qDebug()<<"清空数据库成功";

    //2.批量插入数据
    query.prepare("insert into Person (employee_id,name,face_feature,face_feature_size)" "values(:employee,:name,:face_feature,:face_feature_size)");

    int successCount = 0;
    for(const auto &person:persons){
        query.bindValue(":employee",person.employeeId);
        query.bindValue(":name",person.name);
        query.bindValue(":face_feature",person.faceFeature);
        query.bindValue(":face_feature_size",person.featureSize);

        if(!query.exec()){
            qWarning()<<"插入人员失败"<<query.lastError().text()<<"人员ID"<<person.employeeId;
            m_db.rollback();
            emit personsSyncFailed(QString("插入人员失败：%1").arg(person.employeeId));
            return false;
        }

        successCount++;
    }

    //3.提交事务
    if(!m_db.commit()){
        qWarning()<<"提交事务失败:"<<m_db.lastError().text();
        m_db.rollback();
        emit personsSyncFailed("提交事务失败");
        return false;
    }

    qDebug()<<"人员数据同步成功，共"<<successCount<<"条";
    return true;
}

//添加打卡记录
bool LocalStorage::addAttendanceRecord(const QString &employeeId, const QString &status)
{
    QMutexLocker locker(&s_mutex);

    if(!m_db.isOpen()){
        qDebug()<<"数据库未连接";
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    //开启事务
    if(!m_db.transaction()){
        qDebug()<<"开启事务失败"<<m_db.lastError().text();
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("insert into AttendanceRecord (employee_id,status)" "values (:employee,:status)");

    query.bindValue(":employee",employeeId);
    query.bindValue(":status",status);

    if(!query.exec()){
        qWarning()<<"添加打卡记录失败："<<query.lastError().text();
        emit personsSyncFailed("添加打卡记录失败");
        m_db.rollback();
        return false;
    }

    //提交事务
    if(!m_db.commit()){
        qWarning()<<"提交事务失败:"<<m_db.lastError().text();
        m_db.rollback();
        emit personsSyncFailed("提交事务失败");
        return false;
    }

    qDebug()<<"添加打卡记录成功";
    return true;
}

//获取未上传的打卡记录
QVector<Protocol::AttendanceRecord> LocalStorage::getUnuploadedRecords()
{
    QMutexLocker locker(&s_mutex);
    QVector<Protocol::AttendanceRecord> records;

    if(!m_db.isOpen()){
        qDebug()<<"数据库未连接";
        emit personsSyncFailed("开启事务失败");
        return records;
    }

    QSqlQuery query(m_db);
    query.prepare("select id,employee_id,check_time,status from AttendanceRecord" "where uploaded = 0 order by check_time asc");

    if(!query.exec()){
        qWarning()<<"获取未上传的打卡记录失败："<<query.lastError().text();
        emit personsSyncFailed("获取未上传的打卡记录失败");
        return records;
    }

    while(query.next()){
        Protocol::AttendanceRecord record;
        record.employeeId = query.value("employee_id").toString();
        record.checktTime = query.value("check_time").toString();
        record.status = query.value("status").toString();

        records.append(record);
    }

    qDebug()<<"获取到"<<records.size()<<"条，未上传的打卡记录";
    return records;
}

//标记单条记录已上传
bool LocalStorage::markAsUploaded(int recordId)
{
    QMutexLocker locker(&s_mutex);

    if(!m_db.isOpen()){
        qDebug()<<"数据库未连接";
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    //开启事务
    if(!m_db.transaction()){
        qDebug()<<"开启事务失败"<<m_db.lastError().text();
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("update AttendanceRecord set uploaded = 1,upload_time = datetime('now')" "where id = :id ");
    query.bindValue(":id",recordId);

    if(!query.exec()){
        qWarning()<<"上传失败"<<query.lastError().text();
        m_db.rollback();
        return false;
    }

    //提交事务
    if(!m_db.commit()){
        qWarning()<<"提交事务失败:"<<m_db.lastError().text();
        m_db.rollback();
        emit personsSyncFailed("提交事务失败");
        return false;
    }

    qDebug()<<"打卡记录上传成功";
    return true;

}


//批量标记已上传
    bool LocalStorage::markBatchAsUploaded(const QVector<int> &recordIds)
{
    QMutexLocker locker(&s_mutex);

    if(!m_db.isOpen()){
        qDebug()<<"数据库未连接";
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    //开启事务
    if(!m_db.transaction()){
        qDebug()<<"开启事务失败"<<m_db.lastError().text();
        emit personsSyncFailed("开启事务失败");
        return false;
    }

    QVector<int> records;

    QSqlQuery query(m_db);
    query.prepare("update AttendanceRecord set uploaded = 1,upload_time = datetime('now')" "where id = :id ");

    for(const int &recordId:records){
        query.bindValue(":id",recordId);
        if(!query.exec()){
            qWarning()<<"批量标记失败，ID："<<recordId;
            m_db.rollback();
            return false;
        }
    }

    //提交事务
    if(!m_db.commit()){
        qWarning()<<"提交事务失败:"<<m_db.lastError().text();
        m_db.rollback();
        emit personsSyncFailed("提交事务失败");
        return false;
    }

    qDebug()<<"打卡记录批量上传成功，共："<<records.size()<<"条";
    return true;

}
