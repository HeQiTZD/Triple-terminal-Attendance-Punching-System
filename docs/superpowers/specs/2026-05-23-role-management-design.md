# 角色管理界面 — PageRbac Tab 扩展

## 目标

在现有 PageRbac.qml 中增加"角色管理"Tab，提供角色的创建、删除和权限修改功能。

## 页面布局

```
┌──────────────────────────────────────────────────┐
│  [用户权限分配]  [角色管理]                        │
├──────────────────────────────────────────────────┤
│  角色列表 (Card)                                  │
│  ┌──────────────┬──────────┬────────┬─────┬────┐ │
│  │ 角色标识      │ 角色名称  │ 描述   │系统 │时间│ │
│  ├──────────────┼──────────┼────────┼─────┼────┤ │
│  │ super_admin  │ 超级管理员│ 全部…  │系统 │…   │ │
│  │ hr_admin     │ 人事管理员│ 人员…  │系统 │…   │ │
│  │ device_admin │ 设备管理员│ 设备…  │系统 │…   │ │  ← DataTable
│  │ viewer       │ 只读观察者│ 只读   │ —  │…   │ │
│  └──────────────┴──────────┴────────┴─────┴────┘ │
│                                    [+ 新建角色]   │
├──────────────────────────────────────────────────┤
│  角色详情 (Card，选中角色后显示)                    │
│  [角色标识] [角色名称] [描述]                       │
│                                                   │
│  权限分配                                          │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐          │
│  │ 人员管理  │ │ 考勤记录  │ │ 设备管理  │  ...     │  ← 按模块分组
│  │ ☐ read  │ │ ☐ read  │ │ ☑ read  │          │    CheckBox 网格
│  │ ☐ create│ │ ☐ create│ │ ☑ create│          │
│  │ ☐ update│ │ ☐ update│ │ ☑ update│          │
│  │ ☐ delete│ │ ☐ delete│ │ ☑ delete│          │
│  └──────────┘ └──────────┘ └──────────┘          │
│                                                   │
│  [保存修改]  [删除角色]                             │
└──────────────────────────────────────────────────┘
```

## 三种视图状态

1. **默认**：角色列表可见，详情区显示"请在列表中选择角色"
2. **选中角色**：详情区显示选中角色的表单 + 权限网格
3. **新建角色**：详情区切换为空表单，按钮变为"创建角色"/"取消"

## 权限分组

| 模块 | 权限键 |
|------|--------|
| 人员管理 | person.read, person.create, person.update, person.delete |
| 考勤记录 | attendance.read, attendance.create, attendance.update, attendance.delete |
| 考勤归档 | attendance.archive.read, attendance.archive.delete |
| 设备管理 | device.read, device.create, device.update, device.delete, device.command |
| 用户账号 | user.read, user.create, user.update, user.delete |
| 人脸库 | face.read, face.register, face.delete |
| 其他 | config.deploy, event.subscribe |

## 交互流程

1. 切换到"角色管理"Tab → 自动调用 `rbacServer.queryRoles()` 和 `rbacServer.queryPermissions()`
2. **新建角色**：点击"新建角色" → 清空表单 → 填写 roleKey/roleName/description + 勾选权限 → 点击"创建角色" → 调用 `rbacServer.createRole()`
3. **修改角色**：点击列表中某行 → 表单填入当前值 → 修改字段/权限 → 点击"保存修改" → 调用 `rbacServer.updateRole(roleKey, fields)`
4. **删除角色**：选中角色 → 点击"删除角色" → ConfirmDialog → 调用 `rbacServer.deleteRole(roleKey)`
5. 操作成功后自动刷新角色列表

## 系统角色保护

- `isSystem === true` 的角色：角色标识字段只读（readonly），删除按钮隐藏/禁用
- 自定义角色（`isSystem === false`）：全部字段可编辑，可删除

## 变更范围

### 前端变更
- **PageRbac.qml**：增加 TabBar 和"角色管理"Tab 的全部内容
  - 角色列表 DataTable
  - 角色详情表单 + 权限网格
  - 新建/编辑/删除逻辑

### 后端
- 无变更。`RbacServer` 的 `createRole` / `deleteRole` / `updateRole` / `queryRoles` / `queryPermissions` 完全覆盖需求。

### 不涉及的文件
- `PermissionCatalog.qml` — 不新增权限键，不修改导航
- `PageHost.qml` — 不新增页面
- `main.cpp` — 不新增上下文属性
