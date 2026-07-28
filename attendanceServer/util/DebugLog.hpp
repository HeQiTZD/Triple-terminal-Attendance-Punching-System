#pragma once

#include <chrono>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

// 编译开关：定义 ATTENDANCE_DEBUG_LOG=0 可完全关闭调试日志（零开销）
#ifndef ATTENDANCE_DEBUG_LOG
#define ATTENDANCE_DEBUG_LOG 1
#endif

namespace util {

enum class LogLevel { Debug, Info, Warn, Error };

inline const char* level_tag(LogLevel lv) {
  switch (lv) {
    case LogLevel::Debug: return "DBG";
    case LogLevel::Info:  return "INF";
    case LogLevel::Warn:  return "WRN";
    case LogLevel::Error: return "ERR";
  }
  return "???";
}

// 返回 "HH:MM:SS.mmm" 格式的本地时间戳
inline std::string format_timestamp() {
  using clock = std::chrono::system_clock;
  const auto now = clock::now();
  const auto tt = clock::to_time_t(now);
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) % 1000;
  std::tm local{};
#ifdef _WIN32
  localtime_s(&local, &tt);
#else
  localtime_r(&tt, &local);
#endif
  char buf[24];
  std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d",
                local.tm_hour, local.tm_min, local.tm_sec,
                static_cast<int>(ms.count()));
  return buf;
}

// 全局日志锁，保证多线程输出不交错
inline std::mutex& logMutex() {
  static std::mutex m;
  return m;
}

inline void logImpl(LogLevel level, const char* category,
                    const std::string& msg) {
  std::lock_guard<std::mutex> lk(logMutex());
  std::cout << "[" << format_timestamp() << "] "
            << "[" << level_tag(level) << "] "
            << "[" << category << "] "
            << msg << std::endl;
}

}  // namespace util

// 便捷宏：DBG_LOG(level, category, message)
// category 为字符串字面量，如 "NET", "AUTH", "PROTO"
#if ATTENDANCE_DEBUG_LOG
#define DBG_LOG(level, category, msg)                                 \
  do {                                                                \
    std::ostringstream _dbg_oss;                                      \
    _dbg_oss << msg;                                                  \
    util::logImpl(util::level, category, _dbg_oss.str());             \
  } while (0)
#else
#define DBG_LOG(level, category, msg) ((void)0)
#endif

// 快捷宏 — DEBUG 级别
#define DBG_NET(msg)     DBG_LOG(LogLevel::Debug, "NET",     msg)
#define DBG_AUTH(msg)    DBG_LOG(LogLevel::Debug, "AUTH",    msg)
#define DBG_PROTO(msg)   DBG_LOG(LogLevel::Debug, "PROTO",   msg)
#define DBG_DB(msg)      DBG_LOG(LogLevel::Debug, "DB",      msg)
#define DBG_SESSION(msg) DBG_LOG(LogLevel::Debug, "SESSION", msg)

// 快捷宏 — INFO 级别
#define INFO_NET(msg)    DBG_LOG(LogLevel::Info,  "NET",     msg)
#define INFO_AUTH(msg)   DBG_LOG(LogLevel::Info,  "AUTH",    msg)
#define INFO_PROTO(msg)  DBG_LOG(LogLevel::Info,  "PROTO",   msg)
#define INFO_DB(msg)     DBG_LOG(LogLevel::Info,  "DB",      msg)
#define INFO_SESSION(msg) DBG_LOG(LogLevel::Info, "SESSION", msg)

// 快捷宏 — WARN 级别
#define WARN_NET(msg)    DBG_LOG(LogLevel::Warn,  "NET",     msg)
#define WARN_AUTH(msg)   DBG_LOG(LogLevel::Warn,  "AUTH",    msg)
#define WARN_PROTO(msg)  DBG_LOG(LogLevel::Warn,  "PROTO",   msg)
#define WARN_SESSION(msg) DBG_LOG(LogLevel::Warn, "SESSION", msg)

// 快捷宏 — ERROR 级别
#define ERR_NET(msg)     DBG_LOG(LogLevel::Error, "NET",     msg)
#define ERR_AUTH(msg)    DBG_LOG(LogLevel::Error, "AUTH",    msg)
#define ERR_PROTO(msg)   DBG_LOG(LogLevel::Error, "PROTO",   msg)
#define ERR_DB(msg)      DBG_LOG(LogLevel::Error, "DB",      msg)
#define ERR_SESSION(msg) DBG_LOG(LogLevel::Error, "SESSION", msg)

// 快捷宏 — ATTENDANCE 类别
#define DBG_ATTENDANCE(msg)  DBG_LOG(LogLevel::Debug, "ATTENDANCE", msg)
#define INFO_ATTENDANCE(msg) DBG_LOG(LogLevel::Info,  "ATTENDANCE", msg)
#define WARN_ATTENDANCE(msg) DBG_LOG(LogLevel::Warn,  "ATTENDANCE", msg)
#define ERR_ATTENDANCE(msg)  DBG_LOG(LogLevel::Error, "ATTENDANCE", msg)

// 快捷宏 — EVENT 类别
#define DBG_EVENT(msg)  DBG_LOG(LogLevel::Debug, "EVENT", msg)
#define INFO_EVENT(msg) DBG_LOG(LogLevel::Info,  "EVENT", msg)
#define WARN_EVENT(msg) DBG_LOG(LogLevel::Warn,  "EVENT", msg)
#define ERR_EVENT(msg)  DBG_LOG(LogLevel::Error, "EVENT", msg)
