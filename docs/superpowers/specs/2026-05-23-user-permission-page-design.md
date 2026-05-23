# 用户权限管理页面 — PageRbac 改造

## 目标

将角色权限页面从"以角色为中心"改为"以用户为中心"，保留角色功能，通过给用户分配/撤销角色来管理权限。

## 页面布局

```
┌──────────────┬──────────────────────────┐
│  用户列表     │  用户权限管理              │
│  (320px)     │                          │
│              │  用户信息：ID / 姓名       │
│  [刷新]      │                          │
│              │  角色列表（CheckBox）：    │
│  - 用户1     │  ☑ 超级管理员              │
│  - 用户2     │  ☐ 人事管理员              │
│  - 用户3     │  ☑ 设备管理员              │
│  - ...       │  ☐ 考勤审核员              │
│              │  ☐ 只读观察者              │
│              │                          │
│              │  [保存角色变更]            │
└──────────────┴──────────────────────────┘
```

## 交互流程

1. 页面加载时查询用户列表（userServer.queryUsers）和角色列表（rbacServer.queryRoles）
2. 点击左侧用户 → 调用 rbacServer.queryUserRoles(userId) 获取该用户当前角色
3. 右侧显示全部角色 CheckBox，已有角色为勾选状态
4. 勾选/取消角色 → 点击"保存角色变更"
5. 保存时对比新旧角色集合，逐项调用 assignUserRole / revokeUserRole

## 变更范围

### 数据源
- `userServer.queryUsers("", "")` 获取用户列表
- `rbacServer.queryRoles()` 获取全部角色定义
- `rbacServer.queryUserRoles(userId)` 获取指定用户的角色
- `rbacServer.assignUserRole(userId, roleKey)` 分配角色
- `rbacServer.revokeUserRole(userId, roleKey)` 撤销角色

### 前端变更
- **PageRbac.qml**：整体改造布局和交互逻辑
- **PermissionCatalog.qml**：侧边栏导航标签改为"用户权限"

### 保留内容
- 角色 CRUD 通过弹窗或其他方式保留（后续处理）
- 权限列表展示保留（后续处理）
