#include "localstorage.h"

LocalStorage::LocalStorage() {}

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
    QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbFilePath);

    if(!db.open()){
        qDebug()<<"数据库打开失败";
        qDebug()<<db.lastError().text();
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
                              "last_updated DATETIME DEFAULT CURRENT_TIMESTAMP" 
                              ");";
    if(!query.exec(createPersonTable)){
        qDebug()<<"创建Person表失败"<<query.lastError().text();
        db.close();
        return false;
    }
    //创建人员表索引
    QString createPersonIndex = "CREATE INDEX IF NOT EXISTS idx_person_name ON Person(name);";
    if(!query.exec(createPersonIndex)){
        qDebug()<<"创建Person表索引失败"<<query.lastError().text();
        db.close();
        return false;
    }

    //创建打卡记录表
    QString createRecordTable = "CREATE TABLE IF NOT EXISTS AttendanceRecord(" 
                              "employee_id TEXT NOT NULL," 
                              "check_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP," 
                              "status TEXT NOT NULL DEFAULT '正常'," 
                              "uploaded INTEGER NOT NULL DEFAULT 0," 
                              "upload_time DATETIME," 
                              "FOREIGN KEY (employee_id) REFERENCES Person(employee_id) ON DELETE CASCADE," 
                              "CONSTRAINT ck_attendance_status CHECK (status IN ('正常','异常'))" 
                              ");";
    if(!query.exec(createRecordTable)){
        qDebug()<<"创建AttendanceRecord表失败"<<query.lastError().text();
        db.close();
        return false;
    }
    //创建打卡记录表索引
    QString createRecordIndex1 = "CREATE INDEX IF NOT EXISTS idx_record_employee_id ON AttendanceRecord(employee_id);";
    if(!query.exec(createRecordIndex1)){
        qDebug()<<"创建AttendanceRecord表索引1失败"<<query.lastError().text();
        db.close();
        return false;
    }
    QString createRecordIndex2 = "CREATE INDEX IF NOT EXISTS idx_record_check_time ON AttendanceRecord(check_time);";
    if(!query.exec(createRecordIndex2)){
        qDebug()<<"创建AttendanceRecord表索引2失败"<<query.lastError().text();
        db.close();
        return false;
    }
    
    qDebug()<<"数据库初始化成功";
    return true;
}
