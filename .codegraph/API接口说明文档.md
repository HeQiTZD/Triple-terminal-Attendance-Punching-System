# 考勤服务器对外接口说明文档

## 目录

1. [概述](#1-概述)
2. [通信协议](#2-通信协议)
3. [认证机制](#3-认证机制)
4. [错误码定义](#4-错误码定义)
5. [设备端接口](#5-设备端接口)
6. [管理端接口](#6-管理端接口)
7. [权限系统](#7-权限系统)
8. [数据结构](#8-数据结构)

---

## 1. 概述

本文档描述考勤服务器（AttendanceServer）的对外通信接口。服务器采用基于 TCP 的 JSON 行协议，支持设备端和管理端两种角色连接。

### 1.1 技术架构

- **传输层**: TCP/TLS
- **应用层协议**: JSON 行协议（每行一个完整 JSON 对象）
- **认证方式**: JWT (JSON Web Token)
- **数据库**: MySQL

### 1.2 连接角色

| 角色 | 标识 | 说明 |
|------|------|------|
| 设备端 | `device` | 考勤设备，上报考勤记录、同步人员数据 |
| 管理端 | `admin` | 管理客户端，执行 CRUD 操作、配置管理 |
| 服务端 | `server` | 服务器角色，用于响应和推送消息 |

---

## 2. 通信协议

### 2.1 消息格式（Envelope）

所有消息采用统一的信封格式：

```json
{
  "type": "消息类型",
  "role": "发送方角色",
  "from": "发送方标识",
  "to": "接收方标识",
  "msgId": "消息唯一ID",
  "inReplyTo": "被回复的消息ID（可选）",
  "ts": 1699900000000,
  "code": 0,
  "msg": "状态消息",
  "data": {
    // 业务数据
  }
}
```

### 2.2 字段说明

| 字段 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `type` | string | 是 | 消息类型，点分命名格式 |
| `role` | string | 是 | 发送方角色：`device`/`admin`/`server` |
| `from` | string | 否 | 发送方标识 |
| `to` | string | 否 | 接收方标识 |
| `msgId` | string | 否 | 消息唯一标识，用于请求-响应关联 |
| `inReplyTo` | string | 否 | 被回复的消息ID |
| `ts` | int64 | 否 | 时间戳（毫秒） |
| `code` | int | 否 | 状态码，0 表示成功 |
| `msg` | string | 否 | 状态消息 |
| `data` | object | 否 | 业务数据对象 |

### 2.3 二进制数据传输

对于大数据（如人脸特征、照片），采用"头部+二进制负载"模式：

1. 先发送 JSON 头部，包含 `payloadLength` 字段
2. 紧接着发送指定长度的二进制数据

---

## 3. 认证机制

### 3.1 JWT 认证流程

服务器使用 JWT (JSON Web Token) 进行身份认证：

1. 客户端发送 `auth` 请求
2. 服务器验证身份后返回 `accessToken` 和 `refreshToken`
3. 后续请求需携带 `accessToken`
4. Token 过期后使用 `refreshToken` 刷新

### 3.2 Token 结构

认证成功后返回的 Token 信息：

```json
{
  "accessToken": "eyJhbGciOiJIUzI1NiIs...",
  "refreshToken": "eyJhbGciOiJIUzI1NiIs...",
  "tokenType": "Bearer",
  "expiresIn": 3600
}
```

### 3.3 Token 使用方式

在请求消息的 `data` 中携带 Token：

```json
{
  "type": "person.query",
  "role": "admin",
  "msgId": "req-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    // 其他业务字段...
  }
}
```

---

## 4. 错误码定义

### 4.1 协议级错误码

| 错误码 | 常量名 | 说明 |
|--------|--------|------|
| 1001 | `kCodeParseError` | JSON 解析失败 |
| 1002 | `kCodePayloadTooLarge` | 负载超过限制 |

### 4.2 认证相关错误码

| 错误码 | 常量名 | 说明 |
|--------|--------|------|
| 2001 | `kCodeNotAuthenticated` | 未认证 |
| 2002 | `kCodeAuthFailed` | 认证失败 |
| 2003 | `kCodeDuplicateSession` | 重复会话 |
| 2004 | `kCodeTokenInvalid` | Token 无效 |
| 2005 | `kCodeTokenExpired` | Token 已过期 |
| 2006 | `kCodeRefreshTokenInvalid` | Refresh Token 无效 |
| 2007 | `kCodeInsufficientPermissions` | 权限不足 |

### 4.3 业务错误码

| 错误码 | 常量名 | 说明 |
|--------|--------|------|
| 3001 | `kCodeForbidden` | 禁止访问 |
| 4000 | `kCodeBusinessValidation` | 业务验证失败 |
| 4001 | `kCodeEmployeeNotFound` | 员工不存在 |
| 4002 | - | 检测到多张人脸 |
| 4003 | - | 人脸过小 |
| 4004 | - | 特征提取失败 |
| 4005 | - | 图片加载失败 |
| 4006 | - | 员工不存在（人脸注册） |
| 4007 | - | 人脸数据已存在 |
| 5001 | `kCodeDeviceOffline` | 设备离线 |
| 5002 | `kCodeForwardTimeout` | 消息转发超时 |

### 4.4 数据库错误码

| 错误码 | 常量名 | 说明 |
|--------|--------|------|
| 6001 | `kCodeDuplicateKey` | 唯一键冲突 |
| 6002 | `kCodeDbError` | 数据库错误 |

---

## 5. 设备端接口

设备端需要先完成认证，然后才能调用其他接口。

### 5.1 设备认证

**请求类型**: `auth`

**权限要求**: 无（未认证状态）

**请求示例**:

```json
{
  "type": "auth",
  "role": "device",
  "msgId": "auth-001",
  "data": {
    "deviceId": "DEVICE-001",
    "deviceKey": "your-device-key"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `deviceId` | string | 是 | 设备唯一标识 |
| `deviceKey` | string | 是 | 设备密钥 |

**响应示例**:

```json
{
  "type": "auth_response",
  "role": "server",
  "from": "server",
  "to": "DEVICE-001",
  "inReplyTo": "auth-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "refreshToken": "eyJhbGciOiJIUzI1NiIs...",
    "tokenType": "Bearer",
    "expiresIn": 3600,
    "heartbeatSec": 30,
    "serverTime": 1699900000000,
    "roles": [],
    "permissions": ["attendance.report", "sync.request", "device.status.report"]
  }
}
```

---

### 5.2 心跳

**请求类型**: `heartbeat`

**权限要求**: 已认证设备

**请求示例**:

```json
{
  "type": "heartbeat",
  "role": "device",
  "from": "DEVICE-001",
  "msgId": "hb-001",
  "ts": 1699900000000
}
```

**响应示例**:

```json
{
  "type": "heartbeat_response",
  "role": "server",
  "from": "server",
  "to": "DEVICE-001",
  "inReplyTo": "hb-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok"
}
```

**说明**: 
- 心跳超时时间由服务器配置决定（默认 30 秒）
- 超时后服务器将主动断开连接

---

### 5.3 上报考勤记录

**请求类型**: `attendance.report`

**权限要求**: 已认证设备

**请求示例**:

```json
{
  "type": "attendance.report",
  "role": "device",
  "from": "DEVICE-001",
  "msgId": "att-001",
  "ts": 1699900000000,
  "data": {
    "employeeId": "EMP-001",
    "checkTime": "2024-01-15 09:00:00",
    "status": "check_in",
    "deviceId": "DEVICE-001"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `employeeId` | string | 是 | 员工工号 |
| `checkTime` | string | 是 | 打卡时间（格式：YYYY-MM-DD HH:mm:ss） |
| `status` | string | 是 | 考勤状态：`check_in`/`check_out`/`late`/`early_leave` 等 |
| `deviceId` | string | 否 | 设备ID（可选，默认使用连接设备ID） |
| `awaitPhoto` | boolean | 否 | 是否等待照片数据 |

**响应示例**:

```json
{
  "type": "attendance.report.response",
  "role": "server",
  "from": "server",
  "to": "DEVICE-001",
  "inReplyTo": "att-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok"
}
```

---

### 5.4 上报考勤记录（带照片）

**流程**:
1. 发送考勤记录，设置 `awaitPhoto: true`
2. 发送照片头部消息
3. 发送二进制照片数据

**步骤 1 - 考勤记录请求**:

```json
{
  "type": "attendance.report",
  "role": "device",
  "from": "DEVICE-001",
  "msgId": "att-photo-001",
  "data": {
    "employeeId": "EMP-001",
    "checkTime": "2024-01-15 09:00:00",
    "status": "check_in",
    "awaitPhoto": true
  }
}
```

**步骤 2 - 照片头部**:

```json
{
  "type": "attendance.photo.header",
  "role": "device",
  "from": "DEVICE-001",
  "msgId": "photo-001",
  "data": {
    "payloadLength": 102400
  }
}
```

**步骤 3 - 二进制数据**: 发送指定长度的照片二进制数据

---

### 5.5 上报设备状态

**请求类型**: `device.status.report`

**权限要求**: 已认证设备

**请求示例**:

```json
{
  "type": "device.status.report",
  "role": "device",
  "from": "DEVICE-001",
  "msgId": "status-001",
  "data": {
    "deviceName": "前台考勤机",
    "ipAddress": "192.168.1.100"
  }
}
```

**响应示例**:

```json
{
  "type": "device.status.report.response",
  "role": "server",
  "from": "server",
  "to": "DEVICE-001",
  "inReplyTo": "status-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok"
}
```

---

### 5.6 请求同步数据

**请求类型**: `sync.request`

**权限要求**: 已认证设备

**请求示例**:

```json
{
  "type": "sync.request",
  "role": "device",
  "from": "DEVICE-001",
  "msgId": "sync-001",
  "data": {
    "deviceId": "DEVICE-001"
  }
}
```

**响应流程**:

服务器将依次发送以下消息：

1. **人员同步消息** (`person.sync`) - 分批发送
2. **人脸同步开始** (`face.sync.begin`)
3. **人脸数据头部** (`face.sync.item.header`) + 二进制特征数据
4. **人脸同步结束** (`face.sync.end`)

**人员同步消息示例**:

```json
{
  "type": "person.sync",
  "role": "server",
  "from": "server",
  "to": "DEVICE-001",
  "inReplyTo": "sync-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "deviceId": "DEVICE-001",
    "persons": [
      {
        "id": 1,
        "name": "张三",
        "employeeId": "EMP-001",
        "department": "技术部",
        "position": "工程师"
      }
    ]
  }
}
```

**人脸同步头部示例**:

```json
{
  "type": "face.sync.item.header",
  "role": "server",
  "from": "server",
  "to": "DEVICE-001",
  "inReplyTo": "sync-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "deviceId": "DEVICE-001",
    "employeeId": "EMP-001",
    "featureSize": 1024,
    "payloadLength": 1024,
    "contentType": "application/octet-stream",
    "payloadEncoding": "raw"
  }
}
```

---

### 5.7 同步确认

**请求类型**: `sync.ack`

**权限要求**: 已认证设备

**请求示例**:

```json
{
  "type": "sync.ack",
  "role": "device",
  "from": "DEVICE-001",
  "msgId": "ack-001",
  "inReplyTo": "sync-001",
  "data": {
    "status": "ok",
    "message": "同步成功"
  }
}
```

---

### 5.8 设备命令确认

**请求类型**: `device.command.ack`

**权限要求**: 已认证设备

**说明**: 设备收到 `device.command` 后，执行完毕发送此确认

**请求示例**:

```json
{
  "type": "device.command.ack",
  "role": "device",
  "from": "DEVICE-001",
  "msgId": "cmd-ack-001",
  "inReplyTo": "cmd-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "执行成功",
  "data": {
    "success": true,
    "result": "配置已更新"
  }
}
```

---

## 6. 管理端接口

### 6.1 管理端认证

**请求类型**: `auth`

**权限要求**: 无（未认证状态）

**请求示例**:

```json
{
  "type": "auth",
  "role": "admin",
  "msgId": "admin-auth-001",
  "data": {
    "username": "admin",
    "password": "your-password"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `username` | string | 是 | 用户名（员工工号） |
| `password` | string | 是 | 密码 |

**响应示例**:

```json
{
  "type": "auth_response",
  "role": "server",
  "from": "server",
  "to": "admin",
  "inReplyTo": "admin-auth-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "refreshToken": "eyJhbGciOiJIUzI1NiIs...",
    "tokenType": "Bearer",
    "expiresIn": 3600,
    "heartbeatSec": 30,
    "serverTime": 1699900000000,
    "roles": ["admin", "user_manager"],
    "permissions": ["person.create", "person.read", "person.update", "person.delete"]
  }
}
```

---

### 6.2 Token 刷新

**请求类型**: `token.refresh`

**权限要求**: 已认证

**请求示例**:

```json
{
  "type": "token.refresh",
  "role": "admin",
  "msgId": "refresh-001",
  "data": {
    "refreshToken": "eyJhbGciOiJIUzI1NiIs..."
  }
}
```

**响应示例**:

```json
{
  "type": "token.refresh.response",
  "role": "server",
  "from": "server",
  "to": "admin",
  "inReplyTo": "refresh-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "refreshToken": "eyJhbGciOiJIUzI1NiIs...",
    "tokenType": "Bearer",
    "expiresIn": 3600
  }
}
```

---

### 6.3 人员管理

#### 6.3.1 创建人员

**请求类型**: `person.create`

**权限要求**: `person.create`

**请求示例**:

```json
{
  "type": "person.create",
  "role": "admin",
  "msgId": "person-create-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "name": "张三",
    "employeeId": "EMP-001",
    "department": "技术部",
    "position": "工程师"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `name` | string | 是 | 姓名 |
| `employeeId` | string | 是 | 员工工号（唯一） |
| `department` | string | 是 | 部门 |
| `position` | string | 是 | 职位 |

**响应示例**:

```json
{
  "type": "person.create.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "person-create-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok"
}
```

---

#### 6.3.2 查询人员

**请求类型**: `person.query`

**权限要求**: `person.read`

**请求示例**:

```json
{
  "type": "person.query",
  "role": "admin",
  "msgId": "person-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "name": "张三",
    "employeeId": "EMP-001",
    "department": "技术部"
  }
}
```

**请求参数**（均为可选，用于筛选）:

| 参数 | 类型 | 说明 |
|------|------|------|
| `name` | string | 姓名（模糊匹配） |
| `employeeId` | string | 员工工号 |
| `department` | string | 部门 |
| `position` | string | 职位 |
| `createdAt` | string | 创建时间 |
| `updatedAt` | string | 更新时间 |

**响应示例**:

```json
{
  "type": "person.query.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "person-query-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "records": [
      {
        "id": 1,
        "name": "张三",
        "employeeId": "EMP-001",
        "department": "技术部",
        "position": "工程师",
        "createdAt": "2024-01-15 09:00:00",
        "updatedAt": "2024-01-15 09:00:00"
      }
    ]
  }
}
```

---

#### 6.3.3 更新人员

**请求类型**: `person.update`

**权限要求**: `person.update`

**请求示例**:

```json
{
  "type": "person.update",
  "role": "admin",
  "msgId": "person-update-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001",
    "name": "张三丰",
    "department": "产品部",
    "position": "产品经理"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `employeeId` | string | 是 | 员工工号（定位条件） |
| `name` | string | 否 | 新姓名 |
| `department` | string | 否 | 新部门 |
| `position` | string | 否 | 新职位 |

**说明**: 至少需要一个更新字段

---

#### 6.3.4 删除人员

**请求类型**: `person.delete`

**权限要求**: `person.delete`

**请求示例**:

```json
{
  "type": "person.delete",
  "role": "admin",
  "msgId": "person-delete-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001"
  }
}
```

---

### 6.4 考勤记录管理

#### 6.4.1 创建考勤记录

**请求类型**: `attendance.create`

**权限要求**: `attendance.create`

**请求示例**:

```json
{
  "type": "attendance.create",
  "role": "admin",
  "msgId": "att-create-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001",
    "checkTime": "2024-01-15 09:00:00",
    "status": "check_in",
    "deviceId": "DEVICE-001"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `employeeId` | string | 是 | 员工工号 |
| `checkTime` | string | 是 | 打卡时间 |
| `status` | string | 是 | 考勤状态 |
| `deviceId` | string | 否 | 设备ID |

---

#### 6.4.2 查询考勤记录

**请求类型**: `attendance.query`

**权限要求**: `attendance.read`

**请求示例**:

```json
{
  "type": "attendance.query",
  "role": "admin",
  "msgId": "att-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001",
    "checkTime": "2024-01-15",
    "status": "check_in"
  }
}
```

**请求参数**（均为可选）:

| 参数 | 类型 | 说明 |
|------|------|------|
| `id` | int | 记录ID |
| `employeeId` | string | 员工工号 |
| `checkTime` | string | 打卡时间 |
| `deviceId` | string | 设备ID |
| `status` | string | 考勤状态 |
| `receivedTime` | string | 接收时间 |

**响应示例**:

```json
{
  "type": "attendance.query.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "att-query-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "records": [
      {
        "id": 1,
        "employeeId": "EMP-001",
        "checkTime": "2024-01-15 09:00:00",
        "deviceId": "DEVICE-001",
        "status": "check_in",
        "receivedTime": "2024-01-15 09:00:05"
      }
    ]
  }
}
```

---

#### 6.4.3 更新考勤记录

**请求类型**: `attendance.update`

**权限要求**: `attendance.update`

**请求示例**:

```json
{
  "type": "attendance.update",
  "role": "admin",
  "msgId": "att-update-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "locate": {
      "id": 1,
      "employeeId": "EMP-001"
    },
    "updates": {
      "status": "late",
      "checkTime": "2024-01-15 09:05:00"
    }
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `locate` | object | 是 | 定位条件对象 |
| `updates` | object | 是 | 更新字段对象 |

**locate 对象字段**（至少一个）:

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 记录ID |
| `employeeId` | string | 员工工号 |
| `checkTime` | string | 打卡时间 |
| `deviceId` | string | 设备ID |
| `status` | string | 考勤状态 |
| `receivedTime` | string | 接收时间 |

---

#### 6.4.4 删除考勤记录

**请求类型**: `attendance.delete`

**权限要求**: `attendance.delete`

**请求示例**:

```json
{
  "type": "attendance.delete",
  "role": "admin",
  "msgId": "att-delete-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "id": 1
  }
}
```

**说明**: 必须提供至少一个条件，防止全表删除

---

#### 6.4.5 查询归档考勤记录

**请求类型**: `attendance.archive.query`

**权限要求**: `attendance.archive.read`

**请求示例**:

```json
{
  "type": "attendance.archive.query",
  "role": "admin",
  "msgId": "archive-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001",
    "archivedAt": "2024-01-01"
  }
}
```

**请求参数**（均为可选）:

| 参数 | 类型 | 说明 |
|------|------|------|
| `id` | int | 记录ID |
| `employeeId` | string | 员工工号 |
| `name` | string | 姓名 |
| `department` | string | 部门 |
| `position` | string | 职位 |
| `checkTime` | string | 打卡时间 |
| `deviceId` | string | 设备ID |
| `status` | string | 考勤状态 |
| `receivedTime` | string | 接收时间 |
| `archivedAt` | string | 归档时间 |
| `archiveReason` | string | 归档原因 |

---

#### 6.4.6 删除归档考勤记录

**请求类型**: `attendance.archive.delete`

**权限要求**: `attendance.archive.delete`

**请求示例**:

```json
{
  "type": "attendance.archive.delete",
  "role": "admin",
  "msgId": "archive-delete-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001"
  }
}
```

---

### 6.5 设备管理

#### 6.5.1 创建设备

**请求类型**: `device.create`

**权限要求**: `device.create`

**请求示例**:

```json
{
  "type": "device.create",
  "role": "admin",
  "msgId": "device-create-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "deviceId": "DEVICE-002",
    "deviceName": "会议室考勤机",
    "deviceKey": "device-secret-key",
    "ipAddress": "192.168.1.101",
    "status": "active"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `deviceId` | string | 是 | 设备唯一标识 |
| `deviceName` | string | 是 | 设备名称 |
| `deviceKey` | string | 是 | 设备密钥 |
| `ipAddress` | string | 否 | IP地址 |
| `lastOnline` | string | 否 | 最后在线时间 |
| `status` | string | 否 | 状态 |

---

#### 6.5.2 查询设备

**请求类型**: `device.query`

**权限要求**: `device.read`

**请求示例**:

```json
{
  "type": "device.query",
  "role": "admin",
  "msgId": "device-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "deviceId": "DEVICE-001",
    "deviceName": "前台"
  }
}
```

**响应示例**:

```json
{
  "type": "device.query.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "device-query-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "records": [
      {
        "id": 1,
        "deviceId": "DEVICE-001",
        "deviceName": "前台考勤机",
        "ipAddress": "192.168.1.100",
        "lastOnline": "2024-01-15 09:00:00",
        "status": "online"
      }
    ]
  }
}
```

---

#### 6.5.3 更新设备

**请求类型**: `device.update`

**权限要求**: `device.update`

**请求示例**:

```json
{
  "type": "device.update",
  "role": "admin",
  "msgId": "device-update-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "deviceId": "DEVICE-001",
    "deviceName": "新前台考勤机",
    "status": "maintenance"
  }
}
```

---

#### 6.5.4 删除设备

**请求类型**: `device.delete`

**权限要求**: `device.delete`

**请求示例**:

```json
{
  "type": "device.delete",
  "role": "admin",
  "msgId": "device-delete-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "deviceId": "DEVICE-001"
  }
}
```

---

### 6.6 用户账号管理

#### 6.6.1 创建用户

**请求类型**: `user.create`

**权限要求**: `user.create`

**请求示例**:

```json
{
  "type": "user.create",
  "role": "admin",
  "msgId": "user-create-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001",
    "password": "SecurePass123!",
    "name": "张三"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `employeeId` | string | 是 | 员工工号 |
| `password` | string | 是 | 密码（需满足强度要求） |
| `name` | string | 否 | 显示名称 |

**密码强度要求**:
- 最少 8 个字符
- 包含大小写字母
- 包含数字

---

#### 6.6.2 查询用户

**请求类型**: `user.query`

**权限要求**: `user.read`

**请求示例**:

```json
{
  "type": "user.query",
  "role": "admin",
  "msgId": "user-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001"
  }
}
```

**响应示例**:

```json
{
  "type": "user.query.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "user-query-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "records": [
      {
        "id": 1,
        "employeeId": "EMP-001",
        "name": "张三",
        "createdAt": "2024-01-15 09:00:00",
        "lastLoginTime": "2024-01-16 08:30:00"
      }
    ]
  }
}
```

---

#### 6.6.3 更新用户

**请求类型**: `user.update`

**权限要求**: `user.update`

**请求示例**:

```json
{
  "type": "user.update",
  "role": "admin",
  "msgId": "user-update-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001",
    "name": "张三丰",
    "password": "NewSecurePass456!"
  }
}
```

---

#### 6.6.4 删除用户

**请求类型**: `user.delete`

**权限要求**: `user.delete`

**请求示例**:

```json
{
  "type": "user.delete",
  "role": "admin",
  "msgId": "user-delete-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001"
  }
}
```

**注意**: 不能删除最后一个 super_admin 用户

---

### 6.7 人脸数据管理

#### 6.7.1 注册人脸

**请求类型**: `face.register`

**权限要求**: `face.register`

**请求示例**:

```json
{
  "type": "face.register",
  "role": "admin",
  "msgId": "face-reg-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001",
    "photoBase64": "/9j/4AAQSkZJRg...",
    "overwrite": false
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `employeeId` | string | 是 | 员工工号 |
| `photoBase64` | string | 是 | 照片 Base64 编码 |
| `overwrite` | boolean | 否 | 是否覆盖已存在的人脸（默认 false） |

**响应示例**:

```json
{
  "type": "face.register.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "face-reg-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "employeeId": "EMP-001",
    "featureSize": 1024,
    "faceRect": {
      "left": 100,
      "top": 100,
      "right": 200,
      "bottom": 200
    },
    "isNew": true
  }
}
```

**错误码**:

| 错误码 | 说明 |
|--------|------|
| 4001 | 未检测到人脸 |
| 4002 | 检测到多张人脸 |
| 4003 | 人脸过小 |
| 4004 | 特征提取失败 |
| 4005 | 图片加载失败 |
| 4006 | 员工不存在 |
| 4007 | 人脸数据已存在（需设置 overwrite=true） |

---

#### 6.7.2 查询人脸数据

**请求类型**: `face.query`

**权限要求**: `face.read`

**请求示例**:

```json
{
  "type": "face.query",
  "role": "admin",
  "msgId": "face-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001"
  }
}
```

**响应示例**:

```json
{
  "type": "face.query.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "face-query-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "found": true,
    "employeeId": "EMP-001",
    "featureSize": 1024,
    "createdAt": "2024-01-15 09:00:00",
    "updatedAt": "2024-01-15 09:00:00"
  }
}
```

---

#### 6.7.3 删除人脸数据

**请求类型**: `face.delete`

**权限要求**: `face.delete`

**请求示例**:

```json
{
  "type": "face.delete",
  "role": "admin",
  "msgId": "face-delete-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "employeeId": "EMP-001"
  }
}
```

---

### 6.8 角色管理（RBAC）

#### 6.8.1 创建角色

**请求类型**: `role.create`

**权限要求**: `super_admin` 角色

**请求示例**:

```json
{
  "type": "role.create",
  "role": "admin",
  "msgId": "role-create-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "roleKey": "department_manager",
    "roleName": "部门经理",
    "description": "部门管理权限"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `roleKey` | string | 是 | 角色键（唯一标识） |
| `roleName` | string | 是 | 角色名称 |
| `description` | string | 否 | 角色描述 |

---

#### 6.8.2 查询角色

**请求类型**: `role.query`

**权限要求**: `user.read`

**请求示例**:

```json
{
  "type": "role.query",
  "role": "admin",
  "msgId": "role-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs..."
  }
}
```

**响应示例**:

```json
{
  "type": "role.query.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "role-query-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "records": [
      {
        "id": 1,
        "roleKey": "super_admin",
        "roleName": "超级管理员",
        "description": "系统最高权限",
        "isSystem": true,
        "createdAt": "2024-01-01 00:00:00",
        "updatedAt": "2024-01-01 00:00:00"
      }
    ]
  }
}
```

---

#### 6.8.3 更新角色

**请求类型**: `role.update`

**权限要求**: `super_admin` 角色

**请求示例**:

```json
{
  "type": "role.update",
  "role": "admin",
  "msgId": "role-update-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "roleKey": "department_manager",
    "roleName": "部门主管",
    "description": "部门管理权限（更新）",
    "permissions": ["person.read", "attendance.read", "device.read"]
  }
}
```

---

#### 6.8.4 删除角色

**请求类型**: `role.delete`

**权限要求**: `super_admin` 角色

**请求示例**:

```json
{
  "type": "role.delete",
  "role": "admin",
  "msgId": "role-delete-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "roleKey": "department_manager"
  }
}
```

---

### 6.9 用户角色分配

#### 6.9.1 分配角色

**请求类型**: `user.role.assign`

**权限要求**: `super_admin` 角色

**请求示例**:

```json
{
  "type": "user.role.assign",
  "role": "admin",
  "msgId": "role-assign-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "userId": 2,
    "roleKey": "department_manager"
  }
}
```

---

#### 6.9.2 撤销角色

**请求类型**: `user.role.revoke`

**权限要求**: `super_admin` 角色

**请求示例**:

```json
{
  "type": "user.role.revoke",
  "role": "admin",
  "msgId": "role-revoke-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "userId": 2,
    "roleKey": "department_manager"
  }
}
```

**注意**: 不能撤销自己的 super_admin 角色，且至少保留一个 super_admin

---

#### 6.9.3 查询用户角色

**请求类型**: `user.role.query`

**权限要求**: `user.read`

**请求示例**:

```json
{
  "type": "user.role.query",
  "role": "admin",
  "msgId": "role-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "userId": 2
  }
}
```

**响应示例**:

```json
{
  "type": "user.role.query.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "role-query-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "roles": ["admin", "department_manager"]
  }
}
```

---

### 6.10 权限查询

#### 6.10.1 查询所有权限

**请求类型**: `permission.query`

**权限要求**: `user.read`

**请求示例**:

```json
{
  "type": "permission.query",
  "role": "admin",
  "msgId": "perm-query-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs..."
  }
}
```

**响应示例**:

```json
{
  "type": "permission.query.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "perm-query-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "records": [
      {
        "id": 1,
        "permKey": "person.create",
        "permName": "创建人员",
        "resource": "person",
        "description": "创建新人员记录"
      }
    ]
  }
}
```

---

#### 6.10.2 查询当前用户权限

**请求类型**: `permission.self`

**权限要求**: 已认证管理端

**请求示例**:

```json
{
  "type": "permission.self",
  "role": "admin",
  "msgId": "perm-self-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs..."
  }
}
```

**响应示例**:

```json
{
  "type": "permission.self.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "perm-self-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "permissions": ["person.create", "person.read", "person.update", "person.delete"]
  }
}
```

---

### 6.11 设备命令

#### 6.11.1 发送设备命令

**请求类型**: `device.command`

**权限要求**: `device.command`

**请求示例**:

```json
{
  "type": "device.command",
  "role": "admin",
  "msgId": "cmd-001",
  "to": "DEVICE-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "command": "restart",
    "params": {
      "delay": 5
    }
  }
}
```

**说明**: 
- 命令将转发给目标设备
- 设备执行后返回 `device.command.ack`
- 超时时间：10 秒

**响应示例**（设备返回后转发）:

```json
{
  "type": "device.command.ack",
  "role": "server",
  "from": "server",
  "to": "admin",
  "msgId": "cmd-ack-001",
  "inReplyTo": "cmd-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "执行成功",
  "data": {
    "success": true
  }
}
```

---

### 6.12 配置部署

#### 6.12.1 部署设备配置

**请求类型**: `config.deploy`

**权限要求**: `config.deploy`

**请求示例**:

```json
{
  "type": "config.deploy",
  "role": "admin",
  "msgId": "deploy-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "targetDevices": ["DEVICE-001", "DEVICE-002"],
    "configContent": "[FaceRecognition]\nFaceThreshold=80\n...",
    "configVersion": "2024.01.15.001",
    "description": "更新人脸识别阈值"
  }
}
```

**请求参数**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| `targetDevices` | string[] | 是 | 目标设备ID列表 |
| `configContent` | string | 是 | 配置内容（INI 格式） |
| `configVersion` | string | 否 | 配置版本（不填则自动生成） |
| `description` | string | 否 | 配置描述 |

**配置内容格式（INI）**:

```ini
[FaceRecognition]
FaceThreshold=80
MaxFaceCount=10
RecognizeTimeout=30

[Attendance]
WorkStartTime=09:00
WorkEndTime=18:00
CoreWorkStart=10:00
CoreWorkEnd=17:00
LunchBreakStart=12:00
LunchBreakEnd=13:00
FlexibleWorkEnabled=true
LunchBreakEnabled=true
AllowCrossDay=false
CheckInStartOffset=-30
LateAllowance=5
EarlyLeaveAllowance=5
CheckOutEndOffset=30
FlexibleRange=15
MinCheckInterval=60
MaxWorkHours=12
MaxRetryCount=3
RetryBackoffBaseMs=1000

[Device]
FwVersion=1.0.0
DeviceKey=your-device-key
```

**响应示例**:

```json
{
  "type": "config.deploy.response",
  "role": "server",
  "from": "server",
  "inReplyTo": "deploy-001",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "deployId": 1,
    "configVersion": "2024.01.15.001",
    "configHash": "sha256:abc123...",
    "targets": [
      {
        "deviceId": "DEVICE-001",
        "status": "sent",
        "forwardMsgId": "config.1.1.fw.1"
      },
      {
        "deviceId": "DEVICE-002",
        "status": "pending",
        "forwardMsgId": ""
      }
    ]
  }
}
```

---

### 6.13 事件订阅

#### 6.13.1 订阅事件

**请求类型**: `subscribe`

**权限要求**: `event.subscribe`

**请求示例**:

```json
{
  "type": "subscribe",
  "role": "admin",
  "msgId": "sub-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs...",
    "topics": ["attendance", "device"]
  }
}
```

**可用主题**:

| 主题 | 说明 |
|------|------|
| `attendance` | 考勤记录推送 |
| `device` | 设备状态推送 |

---

#### 6.13.2 取消订阅

**请求类型**: `unsubscribe`

**权限要求**: `event.subscribe`

**请求示例**:

```json
{
  "type": "unsubscribe",
  "role": "admin",
  "msgId": "unsub-001",
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIs..."
  }
}
```

---

#### 6.13.3 事件推送消息

**考勤推送** (`attendance.push`):

```json
{
  "type": "attendance.push",
  "role": "server",
  "from": "server",
  "msgId": "push-att-001",
  "ts": 1699900000000,
  "data": {
    "employeeId": "EMP-001",
    "checkTime": "2024-01-15 09:00:00",
    "status": "check_in",
    "deviceId": "DEVICE-001"
  }
}
```

**设备状态推送** (`device.status.push`):

```json
{
  "type": "device.status.push",
  "role": "server",
  "from": "server",
  "ts": 1699900000000,
  "code": 0,
  "msg": "ok",
  "data": {
    "deviceId": "DEVICE-001",
    "status": "online",
    "ipAddress": "192.168.1.100"
  }
}
```

---

## 7. 权限系统

### 7.1 权限列表

| 权限键 | 说明 |
|--------|------|
| `person.create` | 创建人员 |
| `person.read` | 查询人员 |
| `person.update` | 更新人员 |
| `person.delete` | 删除人员 |
| `attendance.create` | 创建考勤记录 |
| `attendance.read` | 查询考勤记录 |
| `attendance.update` | 更新考勤记录 |
| `attendance.delete` | 删除考勤记录 |
| `attendance.archive.read` | 查询归档考勤 |
| `attendance.archive.delete` | 删除归档考勤 |
| `device.create` | 创建设备 |
| `device.read` | 查询设备 |
| `device.update` | 更新设备 |
| `device.delete` | 删除设备 |
| `device.command` | 发送设备命令 |
| `user.create` | 创建用户 |
| `user.read` | 查询用户 |
| `user.update` | 更新用户 |
| `user.delete` | 删除用户 |
| `face.register` | 注册人脸 |
| `face.read` | 查询人脸 |
| `face.delete` | 删除人脸 |
| `config.deploy` | 部署配置 |
| `event.subscribe` | 订阅事件 |

### 7.2 系统角色

| 角色键 | 说明 |
|--------|------|
| `super_admin` | 超级管理员，拥有所有权限，可管理角色 |
| `admin` | 普通管理员，权限由分配的角色决定 |

---

## 8. 数据结构

### 8.1 人员记录 (Person)

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 自增主键 |
| `name` | string | 姓名 |
| `employeeId` | string | 员工工号（唯一） |
| `department` | string | 部门 |
| `position` | string | 职位 |
| `createdAt` | string | 创建时间 |
| `updatedAt` | string | 更新时间 |

### 8.2 考勤记录 (AttendanceRecord)

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 自增主键 |
| `employeeId` | string | 员工工号 |
| `checkTime` | string | 打卡时间 |
| `deviceId` | string | 设备ID |
| `status` | string | 考勤状态 |
| `receivedTime` | string | 接收时间 |
| `photo` | blob | 照片数据（可选） |

### 8.3 设备记录 (Device)

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 自增主键 |
| `deviceId` | string | 设备唯一标识 |
| `deviceName` | string | 设备名称 |
| `ipAddress` | string | IP地址 |
| `lastOnline` | string | 最后在线时间 |
| `status` | string | 状态：`online`/`offline`/`maintenance` |

### 8.4 用户账号 (UserAccount)

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 自增主键 |
| `employeeId` | string | 员工工号（关联人员） |
| `name` | string | 显示名称 |
| `password` | string | 密码哈希 |
| `createdAt` | string | 创建时间 |
| `lastLoginTime` | string | 最后登录时间 |

### 8.5 人脸数据 (FaceData)

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 自增主键 |
| `employeeId` | string | 员工工号 |
| `featureVector` | blob | 人脸特征向量 |
| `featureSize` | int | 特征向量大小 |
| `createdAt` | string | 创建时间 |
| `updatedAt` | string | 更新时间 |

### 8.6 角色记录 (Role)

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 自增主键 |
| `roleKey` | string | 角色键（唯一） |
| `roleName` | string | 角色名称 |
| `description` | string | 角色描述 |
| `isSystem` | boolean | 是否系统角色 |
| `createdAt` | string | 创建时间 |
| `updatedAt` | string | 更新时间 |

---

## 附录

### A. 消息类型汇总

| 类型 | 方向 | 说明 |
|------|------|------|
| `auth` | 双向 | 认证请求 |
| `auth_response` | 服务端→客户端 | 认证响应 |
| `heartbeat` | 客户端→服务端 | 心跳请求 |
| `heartbeat_response` | 服务端→客户端 | 心跳响应 |
| `token.refresh` | 客户端→服务端 | Token刷新 |
| `token.refresh.response` | 服务端→客户端 | Token刷新响应 |
| `error` | 服务端→客户端 | 错误消息 |
| `attendance.report` | 设备→服务端 | 上报考勤 |
| `attendance.report.response` | 服务端→设备 | 上报响应 |
| `attendance.photo.header` | 设备→服务端 | 照片头部 |
| `attendance.push` | 服务端→管理端 | 考勤推送 |
| `device.status.report` | 设备→服务端 | 设备状态上报 |
| `device.status.report.response` | 服务端→设备 | 状态上报响应 |
| `device.status.push` | 服务端→管理端 | 设备状态推送 |
| `device.command` | 管理端→设备 | 设备命令 |
| `device.command.ack` | 设备→管理端 | 命令确认 |
| `sync.request` | 设备→服务端 | 同步请求 |
| `sync.ack` | 设备→服务端 | 同步确认 |
| `person.sync` | 服务端→设备 | 人员同步 |
| `face.sync.begin` | 服务端→设备 | 人脸同步开始 |
| `face.sync.item.header` | 服务端→设备 | 人脸数据头部 |
| `face.sync.end` | 服务端→设备 | 人脸同步结束 |
| `config.deploy` | 管理端→服务端 | 配置部署 |
| `config.deploy.response` | 服务端→管理端 | 部署响应 |
| `subscribe` | 管理端→服务端 | 订阅事件 |
| `subscribe.response` | 服务端→管理端 | 订阅响应 |
| `unsubscribe` | 管理端→服务端 | 取消订阅 |
| `unsubscribe.response` | 服务端→管理端 | 取消订阅响应 |

### B. CRUD 接口类型映射

| 资源 | 创建 | 查询 | 更新 | 删除 |
|------|------|------|------|------|
| 人员 | `person.create` | `person.query` | `person.update` | `person.delete` |
| 考勤 | `attendance.create` | `attendance.query` | `attendance.update` | `attendance.delete` |
| 设备 | `device.create` | `device.query` | `device.update` | `device.delete` |
| 用户 | `user.create` | `user.query` | `user.update` | `user.delete` |
| 人脸 | `face.register` | `face.query` | - | `face.delete` |
| 角色 | `role.create` | `role.query` | `role.update` | `role.delete` |
| 归档考勤 | - | `attendance.archive.query` | - | `attendance.archive.delete` |

---

*文档版本: 1.0*
*最后更新: 2024-01-15*
