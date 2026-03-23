#ifndef SETWINDOW_H
#define SETWINDOW_H

#include <QWidget>
#include <QString>
#include <QTime>

namespace Ui {
class SetWindow;
}

class SetWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SetWindow(QWidget *parent = nullptr);
    ~SetWindow();

    // 网络连接设置 - Getter
    QString getServerIP() const;
    int getServerPort() const;
    int getConnectionTimeout() const;

    // 网络连接设置 - Setter
    void setServerIP(const QString &ip);
    void setServerPort(int port);
    void setConnectionTimeout(int timeout);

    // 人脸识别设置 - Getter
    int getFaceThreshold() const;
    int getMaxFaceCount() const;
    int getRecognizeTimeout() const;

    // 人脸识别设置 - Setter
    void setFaceThreshold(int threshold);
    void setMaxFaceCount(int count);
    void setRecognizeTimeout(int timeout);

    // 考勤规则设置 - Getter
    QTime getWorkStartTime() const;
    QTime getWorkEndTime() const;
    int getLateAllowance() const;
    int getEarlyLeaveAllowance() const;

    // 考勤规则设置 - Setter
    void setWorkStartTime(const QTime &time);
    void setWorkEndTime(const QTime &time);
    void setLateAllowance(int minutes);
    void setEarlyLeaveAllowance(int minutes);

    // 存储设置 - Getter
    QString getDatabasePath() const;
    QString getLogPath() const;
    QString getCachePath() const;

    // 存储设置 - Setter
    void setDatabasePath(const QString &path);
    void setLogPath(const QString &path);
    void setCachePath(const QString &path);

    // 从UI控件加载值到成员变量
    void loadFromUI();
    // 将成员变量值设置到UI控件
    void saveToUI();

private:
    Ui::SetWindow *ui;

    // 网络连接设置
    QString m_serverIP = "192.168.1.100";   // 服务器地址
    int m_serverPort = 8080;                // 服务器端口
    int m_connectionTimeout = 30;           // 连接超时时间(秒)

    // 人脸识别设置
    int m_faceThreshold = 80;               // 相似度阈值(50-99)
    int m_maxFaceCount = 5;                 // 最大检测人脸数
    int m_recognizeTimeout = 10;            // 识别超时时间(秒)

    // 考勤规则设置
    QTime m_workStartTime = QTime(9, 0);    // 上班时间
    QTime m_workEndTime = QTime(18, 0);     // 下班时间
    int m_lateAllowance = 15;               // 迟到允许时间(分钟)
    int m_earlyLeaveAllowance = 15;         // 早退允许时间(分钟)

    // 存储设置
    QString m_databasePath;                 // 数据库路径
    QString m_logPath;                      // 日志路径
    QString m_cachePath;                    // 缓存目录路径
};

#endif // SETWINDOW_H
