#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <sys/_types/_int64_t.h>
#include <unordered_map>
#include <utils/Result.hpp>
#include <vector>

namespace dhquant {

enum class LogLevel { QDebug, QInfo, QWarn, QError, QFatal };

struct LogRecord {
  LogLevel level;       // 日志级别
  std::string module;   // 模块
  std::string message;  // 日志内容
  int64_t timestamp;    // 时间戳
  std::string threadId; // 线程ID
  std::string file;     // 文件名
  int line;             // 行号
  std::string function; // 函数名
  std::unordered_map<std::string, std::string>
      context; // 上下文信息,举例子：{"userId":"12345","orderId":"67890"}
};

static std::string level_str(LogLevel level) {
  switch (level) {
  case LogLevel::QDebug:
    return "[DEBUG]";
  case LogLevel::QInfo:
    return "[INFO ]";
  case LogLevel::QWarn:
    return "[WARN ]";
  case LogLevel::QError:
    return "[ERROR]";
  case LogLevel::QFatal:
    return "[FATAL]";
  }
  return "[?????]";
}
/*
日志接收器接口，定义了一个纯虚函数log()，用于接收日志记录并进行处理。不同的日志接收器可以实现不同的处理方式，例如将日志写入文件、发送到远程服务器等。
 */
class LogSink {
public:
  virtual ~LogSink() = default;
  virtual void log(const LogRecord &record) = 0;
};

/*
日志记录器类，提供了日志记录的功能。它可以接受多个日志接收器，并将日志记录发送给这些接收器进行处理。日志记录器还可以根据日志级别进行过滤，只记录指定级别以上的日志。
*/
class Logger {
public:
  static Logger &instance();
  void AddSink(std::shared_ptr<LogSink> sink);
  void Log(LogLevel level, const std::string &module,
           const std::string &message, const std::string &file, int line,
           const std::string &function,
           const std::unordered_map<std::string, std::string> &context = {});

  // 检查特定级别是否会被记录
  bool ShouldLog(LogLevel level) const;

private:
  Logger() = default;
  std::vector<std::shared_ptr<LogSink>> sinks;
};

} // namespace dhquant

// --- 宏定义：简化调用并自动捕获文件名、函数名和行号 ---

#define DH_LOG(level, module, msg)                                             \
  do {                                                                         \
    if (dhquant::Logger::instance().ShouldLog(level)) {                        \
      dhquant::Logger::instance().Log(level, module, msg, __FILE__, __LINE__,  \
                                      __FUNCTION__);                           \
    }                                                                          \
  } while (0)

#define LOGD(module, msg) DH_LOG(dhquant::LogLevel::QDebug, module, msg)
#define LOGI(module, msg) DH_LOG(dhquant::LogLevel::QInfo, module, msg)
#define LOGW(module, msg) DH_LOG(dhquant::LogLevel::QWarn, module, msg)
#define LOGE(module, msg) DH_LOG(dhquant::LogLevel::QError, module, msg)

// 针对 Result 体系的宏：如果结果错误则记录日志并返回
#define DH_RETURN_IF_ERROR(result, module, msg)                                \
  do {                                                                         \
    auto &&res = (result);                                                     \
    if (!res.ok()) {                                                           \
      if (dhquant::Logger::instance().ShouldLog(dhquant::LogLevel::QError)) {  \
        dhquant::Logger::instance().Log(dhquant::LogLevel::QError, module,     \
                                        std::string(msg) + ": " +              \
                                            res.error().message,               \
                                        __FILE__, __LINE__, __FUNCTION__);     \
      }                                                                        \
      return std::forward<decltype(res)>(res);                                 \
    }                                                                          \
  } while (0)
