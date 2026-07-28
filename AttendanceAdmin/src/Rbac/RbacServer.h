#ifndef RBACSERVER_H
#define RBACSERVER_H
#include <QObject>
#include <QVariantList>//存储任意类型的列表数据 原因：与QML数据绑定更方便，网络传输时JSON序列号更简单，灵活性高，字段可动态变化
#include <QStringList>//存储字符串列表

#include <QJsonArray>
#include <QJsonObject>


class TcpConnectionManager; //声明类名但不引入完整定义，减少编译依赖

class RbacServer : public QObject {
	Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QVariantList roleRecords READ roleRecords NOTIFY roleRecordsChanged)
    Q_PROPERTY(QVariantList permissionRecords READ permissionRecords NOTIFY permissionRecordsChanged)
    Q_PROPERTY(QStringList userRoleKeys READ userRoleKeys NOTIFY userRoleKeysChanged)

public:
    explicit RbacServer(QObject* parent = nullptr);
    void setTcpManager(TcpConnectionManager* tcp);

    bool busy() const { return m_busy; }
    QVariantList roleRecords() const { return m_roleRecords; }
    QVariantList permissionRecords() const { return m_permissionRecords; }
    QStringList userRoleKeys() const { return m_userRoleKeys; }

    Q_INVOKABLE void queryRoles();//查询所有角色
    Q_INVOKABLE void queryPermissions();//查询所有权限
    Q_INVOKABLE void querySelfPermissions();//查询当前用户所有权限
    Q_INVOKABLE void queryUserRoles(int userId);//查询用户所有角色

    Q_INVOKABLE void createRole(const QString& roleKey,
        const QString& roleName,
        const QString& description);
    // fields 可含：roleName, description, permissions (QStringList)；未出现的键不更新；permissions 出现则整体替换
    Q_INVOKABLE void updateRole(const QString& roleKey, const QVariantMap& fields);

    Q_INVOKABLE void deleteRole(const QString& roleKey);
    Q_INVOKABLE void assignUserRole(int userId, const QString& roleKey);//为用户分配角色
    Q_INVOKABLE void revokeUserRole(int userId, const QString& roleKey);//撤销用户角色  

signals:
    void busyChanged();
    void roleRecordsChanged();
    void permissionRecordsChanged();
    void userRoleKeysChanged();
    void selfPermissionsChanged();
    void operationSucceeded(const QString& apiType, const QString& message);
    void operationFailed(const QString& apiType, int code, const QString& message);

private:
    void setBusy(bool v);
    static QVariantList parseRoleRecords(const QJsonObject& dataObj);
    static QVariantList parsePermissionRecords(const QJsonObject& dataObj);

    TcpConnectionManager* m_tcp = nullptr;//TCP连接管理器指针
    bool m_busy = false;//  是否正在执行操作
    QVariantList m_roleRecords;//角色记录列表
    QVariantList m_permissionRecords;//权限记录列表
    QStringList m_userRoleKeys;//用户角色键列表
    QStringList m_selfPermissions;//当前用户权限列表
};

#endif