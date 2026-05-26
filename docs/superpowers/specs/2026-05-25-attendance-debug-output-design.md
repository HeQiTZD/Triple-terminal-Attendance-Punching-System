# 打卡流程控制台调试信息输出方案设计

## 一、设计目标

设计一个完整的控制台调试信息输出方案，用于可视化追踪打卡流程的每个步骤，包括：
- 打卡识别流程（从摄像头采集到人脸识别完成）
- 打卡上传流程（从发送打卡记录到服务器响应）
- 离线缓存流程（上传失败时保存到Outbox）

## 二、输出格式规范

### 2.1 统一格式

```
HH:MM:SS.mmm [状态] 步骤名称 key=value key=value
```

### 2.2 时间戳格式

使用 `QTime::currentTime().toString("HH:mm:ss.zzz")`

### 2.3 状态标记

| 状态 | 说明 |
|------|------|
| `[开始]` | 步骤开始 |
| `[进行中]` | 步骤中间状态 |
| `[完成]` | 步骤成功完成 |
| `[失败]` | 步骤失败 |
| `[跳过]` | 步骤被跳过 |

### 2.4 日志前缀

所有调试信息使用 `[打卡流程]` 前缀，便于在Qt Creator中过滤。

## 三、打卡识别流程调试信息

### 3.1 正常流程

```
09:00:00.000 [开始] 摄像头采集
09:00:00.050 [完成] 摄像头采集 格式=RGB888 分辨率=640x480 耗时=50ms

09:00:00.051 [开始] 人脸检测
09:00:00.100 [完成] 人脸检测 数量=1 位置=(100,50,200,200) 耗时=49ms

09:00:00.101 [开始] 特征提取
09:00:00.146 [完成] 特征提取 大小=1032bytes 耗时=45ms

09:00:00.147 [开始] 特征比对
09:00:00.159 [完成] 特征比对 employeeId=EMP001 相似度=0.92 阈值=0.80 耗时=12ms

09:00:00.160 [开始] 考勤规则判定
09:00:00.161 [进行中] 重复检测 结果=无历史记录
09:00:00.162 [进行中] 时间范围判定 结果=在范围内
09:00:00.163 [完成] 考勤规则判定 status=normal isValid=true message="上班打卡成功" 耗时=3ms

09:00:00.164 [开始] 打卡结果处理
09:00:00.165 [完成] 打卡结果处理 employeeId=EMP001 status=normal 总耗时=165ms
```

### 3.2 异常场景

**人脸检测失败**
```
09:00:00.051 [开始] 人脸检测
09:00:00.100 [失败] 人脸检测 原因=未检测到人脸 耗时=49ms
```

**特征比对失败**
```
09:00:00.147 [开始] 特征比对
09:00:00.159 [失败] 特征比对 原因=相似度低于阈值 相似度=0.65 阈值=0.80 耗时=12ms
```

## 四、打卡记录上传流程调试信息

### 4.1 正常流程

```
09:00:00.200 [开始] 打卡记录上传 employeeId=EMP001 msgId=abc123
09:00:00.201 [进行中] Outbox持久化 employeeId=EMP001
09:00:00.210 [完成] Outbox持久化 employeeId=EMP001 耗时=9ms

09:00:00.211 [开始] 发送打卡请求 employeeId=EMP001
09:00:00.250 [进行中] 网络发送 msgId=abc123 目标=192.168.1.100:8080
09:00:00.300 [完成] 网络发送 msgId=abc123 耗时=89ms

09:00:00.301 [开始] 等待服务器响应 msgId=abc123
09:00:00.500 [完成] 收到服务器响应 msgId=abc123 code=0 耗时=199ms

09:00:00.501 [开始] 更新Outbox状态 msgId=abc123
09:00:00.510 [完成] 更新Outbox状态 msgId=abc123 新状态=confirmed 耗时=9ms

09:00:00.511 [完成] 打卡记录上传 employeeId=EMP001 msgId=abc123 状态=成功 总耗时=311ms
```

### 4.2 异常场景

**网络发送失败**
```
09:00:00.211 [开始] 发送打卡请求 employeeId=EMP001
09:00:00.250 [失败] 网络发送 msgId=abc123 原因=连接超时 耗时=39ms
```

**服务器响应错误**
```
09:00:00.500 [完成] 收到服务器响应 msgId=abc123 code=500 message="服务器内部错误" 耗时=199ms
09:00:00.501 [失败] 打卡记录上传 employeeId=EMP001 msgId=abc123 原因=服务器拒绝
```

## 五、离线缓存流程调试信息

### 5.1 网络发送失败，保存到Outbox

```
09:00:00.211 [开始] 发送打卡请求 employeeId=EMP001
09:00:00.250 [失败] 网络发送 msgId=abc123 原因=连接超时 耗时=39ms

09:00:00.251 [开始] 离线缓存处理 employeeId=EMP001 msgId=abc123
09:00:00.252 [进行中] 保存到Outbox employeeId=EMP001
09:00:00.260 [完成] 保存到Outbox employeeId=EMP001 outboxId=456 耗时=8ms
09:00:00.261 [完成] 离线缓存处理 employeeId=EMP001 outboxId=456 状态=待重试
```

### 5.2 服务器响应错误，保存到Outbox

```
09:00:00.500 [完成] 收到服务器响应 msgId=abc123 code=500 message="服务器内部错误" 耗时=199ms

09:00:00.501 [开始] 离线缓存处理 employeeId=EMP001 msgId=abc123
09:00:00.502 [进行中] 保存到Outbox employeeId=EMP001
09:00:00.510 [完成] 保存到Outbox employeeId=EMP001 outboxId=457 耗时=8ms
09:00:00.511 [完成] 离线缓存处理 employeeId=EMP001 outboxId=457 状态=待重试
```

### 5.3 Outbox已存在该记录（重复保存检测）

```
09:00:00.251 [开始] 离线缓存处理 employeeId=EMP001 msgId=abc123
09:00:00.252 [进行中] 检查Outbox重复 employeeId=EMP001 msgId=abc123
09:00:00.255 [跳过] 保存到Outbox 原因=记录已存在 outboxId=456
09:00:00.256 [完成] 离线缓存处理 employeeId=EMP001 outboxId=456 状态=已存在
```

### 5.4 Outbox保存失败

```
09:00:00.251 [开始] 离线缓存处理 employeeId=EMP001 msgId=abc123
09:00:00.252 [进行中] 保存到Outbox employeeId=EMP001
09:00:00.260 [失败] 保存到Outbox employeeId=EMP001 原因=数据库写入失败 耗时=8ms
09:00:00.261 [失败] 离线缓存处理 employeeId=EMP001 msgId=abc123 原因=Outbox保存失败
```

### 5.5 离线重试机制

```
09:05:00.000 [开始] Outbox重试检查
09:05:00.010 [进行中] 扫描Outbox 待重试数量=3
09:05:00.011 [开始] 重试上传 outboxId=456 employeeId=EMP001 重试次数=1
09:05:00.050 [完成] 重试上传 outboxId=456 employeeId=EMP001 状态=成功 耗时=39ms
09:05:00.051 [进行中] 删除Outbox记录 outboxId=456
09:05:00.060 [完成] 删除Outbox记录 outboxId=456 耗时=9ms
09:05:00.061 [完成] Outbox重试检查 成功=1 失败=0 待重试=2
```

## 六、Outbox状态说明

| 状态 | 说明 |
|------|------|
| `pending` | 待发送，首次保存 |
| `retrying` | 重试中，已尝试发送但失败 |
| `confirmed` | 已确认，服务器成功响应 |
| `failed` | 最终失败，超过最大重试次数 |

## 七、实现要点

### 7.1 关键调试点

| 模块 | 调试点 | 输出内容 |
|------|--------|----------|
| FaceRecognizer | 人脸检测 | 数量、位置、耗时 |
| FaceRecognizer | 特征提取 | 大小、耗时 |
| FaceRecognizer | 特征比对 | employeeId、相似度、阈值、耗时 |
| AttendanceRuleEngine | 规则判定 | status、isValid、message、耗时 |
| AttendanceRuleEngine | 重复检测 | 结果 |
| AttendanceRuleEngine | 时间范围判定 | 结果 |
| AttendanceReporter | Outbox持久化 | employeeId、耗时 |
| Networkclient | 网络发送 | msgId、目标、耗时 |
| Networkclient | 服务器响应 | msgId、code、耗时 |
| AttendanceReporter | 离线缓存 | outboxId、状态 |
| AttendanceReporter | Outbox重试 | outboxId、重试次数、结果 |

### 7.2 日志过滤

在Qt Creator中使用正则表达式过滤：
```
\[打卡流程\]
```

### 7.3 性能考虑

- 时间戳使用 `QTime::currentTime()` 获取，性能开销极小
- 字符串拼接使用 `QString::arg()` 或流操作符，避免不必要的内存分配
- 调试信息仅在调试模式下输出，发布版本可通过编译宏禁用

## 八、设计文档信息

- **创建日期**: 2026-05-25
- **设计状态**: 已批准
- **下一步**: 编写实现计划
