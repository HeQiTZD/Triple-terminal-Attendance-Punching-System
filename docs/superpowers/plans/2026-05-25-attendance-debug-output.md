# 打卡流程控制台调试信息输出实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 为打卡流程添加完整的控制台调试信息输出，使用时间线+状态标记格式可视化追踪每个步骤

**Architecture:** 在现有代码的关键位置插入调试输出语句，使用统一的格式规范（时间戳+状态标记+键值对），覆盖打卡识别流程、上传流程和离线缓存流程

**Tech Stack:** Qt 6.x (qDebug, QTime), C++17

---

## 文件结构

### 需要修改的文件

| 文件路径 | 职责 | 修改内容 |
|----------|------|----------|
| `FaceRecognition/facerecognizer.cpp` | 人脸识别器 | 添加人脸检测、特征提取、特征比对的调试输出 |
| `Attendance/AttendanceRuleEngine.cpp` | 考勤规则引擎 | 添加规则判定各阶段的调试输出 |
| `Attendance/AttendanceReporter.cpp` | 打卡上报器 | 添加Outbox持久化、离线缓存的调试输出 |
| `NetworkClient/networkclient.cpp` | 网络客户端 | 添加网络发送、服务器响应的调试输出 |

---

## 任务分解

### Task 1: 为 FaceRecognizer 添加调试输出

**Files:**
- Modify: `FaceRecognition/facerecognizer.cpp`

- [ ] **Step 1: 添加头文件**

在 `facerecognizer.cpp` 顶部添加：
```cpp
#include <QTime>
```

- [ ] **Step 2: 为人脸检测添加调试输出**

在 `handleIdleState` 方法中，检测到人脸后添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 人脸检测";
```

在检测完成后添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 人脸检测"
         << "数量=" << faceCount
         << "位置=" << faceRect
         << "耗时=" << elapsed << "ms";
```

如果检测失败：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[失败] 人脸检测"
         << "原因=未检测到人脸"
         << "耗时=" << elapsed << "ms";
```

- [ ] **Step 3: 为特征提取添加调试输出**

在 `performRecognition` 方法中，开始提取时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 特征提取";
```

提取完成后添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 特征提取"
         << "大小=" << featureSize << "bytes"
         << "耗时=" << elapsed << "ms";
```

- [ ] **Step 4: 为特征比对添加调试输出**

开始比对时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 特征比对";
```

比对完成后添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 特征比对"
         << "employeeId=" << employeeId
         << "相似度=" << similarity
         << "阈值=" << threshold
         << "耗时=" << elapsed << "ms";
```

比对失败时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[失败] 特征比对"
         << "原因=相似度低于阈值"
         << "相似度=" << similarity
         << "阈值=" << threshold
         << "耗时=" << elapsed << "ms";
```

- [ ] **Step 5: 编译验证**

```bash
cd build && cmake --build . --target AttendanceSystem
```

- [ ] **Step 6: 提交**

```bash
git add FaceRecognition/facerecognizer.cpp
git commit -m "feat: 为FaceRecognizer添加打卡流程调试输出"
```

---

### Task 2: 为 AttendanceRuleEngine 添加调试输出

**Files:**
- Modify: `Attendance/AttendanceRuleEngine.cpp`

- [ ] **Step 1: 添加头文件**

在 `AttendanceRuleEngine.cpp` 顶部添加：
```cpp
#include <QTime>
```

- [ ] **Step 2: 为规则判定开始添加调试输出**

在 `evaluate` 或 `evaluateWithEmployee` 方法开始处添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 考勤规则判定";
```

- [ ] **Step 3: 为重复检测添加调试输出**

在重复检测逻辑处添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[进行中] 重复检测"
         << "结果=" << (isDuplicate ? "已存在" : "无历史记录");
```

- [ ] **Step 4: 为时间范围判定添加调试输出**

在时间范围判定逻辑处添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[进行中] 时间范围判定"
         << "结果=" << (inRange ? "在范围内" : "不在范围内");
```

- [ ] **Step 5: 为规则判定完成添加调试输出**

在规则判定完成后添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 考勤规则判定"
         << "status=" << status
         << "isValid=" << isValid
         << "message=" << message
         << "耗时=" << elapsed << "ms";
```

- [ ] **Step 6: 编译验证**

```bash
cd build && cmake --build . --target AttendanceSystem
```

- [ ] **Step 7: 提交**

```bash
git add Attendance/AttendanceRuleEngine.cpp
git commit -m "feat: 为AttendanceRuleEngine添加打卡流程调试输出"
```

---

### Task 3: 为 AttendanceReporter 添加调试输出

**Files:**
- Modify: `Attendance/AttendanceReporter.cpp`

- [ ] **Step 1: 添加头文件**

在 `AttendanceReporter.cpp` 顶部添加：
```cpp
#include <QTime>
```

- [ ] **Step 2: 为打卡记录上传开始添加调试输出**

在 `report` 方法开始处添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 打卡记录上传"
         << "employeeId=" << employeeId
         << "msgId=" << msgId;
```

- [ ] **Step 3: 为Outbox持久化添加调试输出**

开始保存时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[进行中] Outbox持久化"
         << "employeeId=" << employeeId;
```

保存完成后添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] Outbox持久化"
         << "employeeId=" << employeeId
         << "耗时=" << elapsed << "ms";
```

- [ ] **Step 4: 为离线缓存处理添加调试输出**

开始离线缓存时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 离线缓存处理"
         << "employeeId=" << employeeId
         << "msgId=" << msgId;
```

检查重复时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[进行中] 检查Outbox重复"
         << "employeeId=" << employeeId
         << "msgId=" << msgId;
```

记录已存在时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[跳过] 保存到Outbox"
         << "原因=记录已存在"
         << "outboxId=" << outboxId;
```

保存成功时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 保存到Outbox"
         << "employeeId=" << employeeId
         << "outboxId=" << outboxId
         << "耗时=" << elapsed << "ms";
```

保存失败时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[失败] 保存到Outbox"
         << "employeeId=" << employeeId
         << "原因=数据库写入失败"
         << "耗时=" << elapsed << "ms";
```

离线缓存完成时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 离线缓存处理"
         << "employeeId=" << employeeId
         << "outboxId=" << outboxId
         << "状态=" << status;
```

- [ ] **Step 5: 为Outbox重试机制添加调试输出**

开始重试检查时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] Outbox重试检查";
```

扫描Outbox时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[进行中] 扫描Outbox"
         << "待重试数量=" << retryCount;
```

开始重试上传时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 重试上传"
         << "outboxId=" << outboxId
         << "employeeId=" << employeeId
         << "重试次数=" << retryNumber;
```

重试成功时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 重试上传"
         << "outboxId=" << outboxId
         << "employeeId=" << employeeId
         << "状态=成功"
         << "耗时=" << elapsed << "ms";
```

删除Outbox记录时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[进行中] 删除Outbox记录"
         << "outboxId=" << outboxId;
```

删除完成时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 删除Outbox记录"
         << "outboxId=" << outboxId
         << "耗时=" << elapsed << "ms";
```

重试检查完成时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] Outbox重试检查"
         << "成功=" << successCount
         << "失败=" << failCount
         << "待重试=" << pendingCount;
```

- [ ] **Step 6: 编译验证**

```bash
cd build && cmake --build . --target AttendanceSystem
```

- [ ] **Step 7: 提交**

```bash
git add Attendance/AttendanceReporter.cpp
git commit -m "feat: 为AttendanceReporter添加打卡流程调试输出"
```

---

### Task 4: 为 Networkclient 添加调试输出

**Files:**
- Modify: `NetworkClient/networkclient.cpp`

- [ ] **Step 1: 添加头文件**

在 `networkclient.cpp` 顶部添加：
```cpp
#include <QTime>
```

- [ ] **Step 2: 为网络发送添加调试输出**

开始发送时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 发送打卡请求"
         << "employeeId=" << employeeId;
```

网络发送中添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[进行中] 网络发送"
         << "msgId=" << msgId
         << "目标=" << serverAddress << ":" << serverPort;
```

发送完成时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 网络发送"
         << "msgId=" << msgId
         << "耗时=" << elapsed << "ms";
```

发送失败时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[失败] 网络发送"
         << "msgId=" << msgId
         << "原因=" << errorString
         << "耗时=" << elapsed << "ms";
```

- [ ] **Step 3: 为服务器响应添加调试输出**

开始等待响应时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 等待服务器响应"
         << "msgId=" << msgId;
```

收到响应时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 收到服务器响应"
         << "msgId=" << msgId
         << "code=" << responseCode
         << "耗时=" << elapsed << "ms";
```

响应错误时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 收到服务器响应"
         << "msgId=" << msgId
         << "code=" << responseCode
         << "message=" << errorMessage
         << "耗时=" << elapsed << "ms";
```

- [ ] **Step 4: 为更新Outbox状态添加调试输出**

开始更新时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[开始] 更新Outbox状态"
         << "msgId=" << msgId;
```

更新完成时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 更新Outbox状态"
         << "msgId=" << msgId
         << "新状态=" << newStatus
         << "耗时=" << elapsed << "ms";
```

- [ ] **Step 5: 为打卡记录上传完成添加调试输出**

上传成功时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[完成] 打卡记录上传"
         << "employeeId=" << employeeId
         << "msgId=" << msgId
         << "状态=成功"
         << "总耗时=" << totalElapsed << "ms";
```

上传失败时添加：
```cpp
qDebug() << "[打卡流程]" << QTime::currentTime().toString("HH:mm:ss.zzz")
         << "[失败] 打卡记录上传"
         << "employeeId=" << employeeId
         << "msgId=" << msgId
         << "原因=" << reason;
```

- [ ] **Step 6: 编译验证**

```bash
cd build && cmake --build . --target AttendanceSystem
```

- [ ] **Step 7: 提交**

```bash
git add NetworkClient/networkclient.cpp
git commit -m "feat: 为Networkclient添加打卡流程调试输出"
```

---

### Task 5: 集成测试与验证

**Files:**
- None (手动测试)

- [ ] **Step 1: 编译整个项目**

```bash
cd build && cmake --build .
```

- [ ] **Step 2: 运行程序并触发打卡流程**

启动程序，进行一次完整的人脸识别打卡流程

- [ ] **Step 3: 在控制台查看调试输出**

在Qt Creator的应用程序输出面板中，验证调试信息格式是否正确：
- 时间戳格式：`HH:MM:SS.mmm`
- 状态标记：`[开始]`、`[进行中]`、`[完成]`、`[失败]`、`[跳过]`
- 键值对格式：`key=value`

- [ ] **Step 4: 使用日志过滤验证**

在Qt Creator中使用正则表达式过滤：
```
\[打卡流程\]
```

确认所有调试信息都能被正确过滤

- [ ] **Step 5: 测试异常场景**

- 测试人脸检测失败场景
- 测试特征比对失败场景（相似度低于阈值）
- 测试网络断开场景（离线缓存）
- 测试服务器响应错误场景

- [ ] **Step 6: 验证Outbox重试机制**

- 断开网络，进行打卡（应触发离线缓存）
- 恢复网络，等待Outbox重试
- 验证重试调试输出

- [ ] **Step 7: 提交最终版本**

```bash
git add -A
git commit -m "feat: 完成打卡流程控制台调试信息输出方案"
```

---

## 自我审查清单

### 1. 规格覆盖检查

| 规格要求 | 对应任务 | 状态 |
|----------|----------|------|
| 时间线+状态标记格式 | Task 1-4 | ✓ |
| 打卡识别流程调试 | Task 1 | ✓ |
| 打卡上传流程调试 | Task 3-4 | ✓ |
| 离线缓存流程调试 | Task 3 | ✓ |
| Outbox重试机制调试 | Task 3 | ✓ |
| 异常场景调试 | Task 1-4 | ✓ |
| 日志过滤支持 | Task 1-4 | ✓ |

### 2. 占位符扫描

- 无 "TBD"、"TODO" 或 "implement later"
- 所有步骤都包含具体的代码或命令
- 所有文件路径都是精确的

### 3. 类型一致性检查

- 调试输出格式在所有任务中保持一致
- 键值对名称统一（如 `employeeId`、`msgId`、`耗时`）
- 状态标记使用统一的中文标签

---

## 执行选项

**计划已完成并保存到 `docs/superpowers/plans/2026-05-25-attendance-debug-output.md`。两种执行方式：**

**1. Subagent-Driven（推荐）** - 每个任务分派一个新子代理，任务间进行审查，快速迭代

**2. Inline Execution** - 在当前会话中执行任务，批量执行并设置检查点

**选择哪种方式？**
