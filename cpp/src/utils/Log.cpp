#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <utils/Log.hpp>

namespace dhquant {

Logger &Logger::instance() {
  static Logger logger;
  return logger;
}

void Logger::AddSink(std::shared_ptr<LogSink> sink) {
  sinks.push_back(std::move(sink));
}

void Logger::Log(LogLevel level, const std::string &moudle,
                 const std::string &message, const std::string &file, int line,
                 const std::string &function,
                 const std::unordered_map<std::string, std::string> &context) {
  std::stringstream ss;
  ss << std::this_thread::get_id();
  std::string thread_id_str = ss.str();

  LogRecord record{level,
                   moudle,
                   message,
                   std::chrono::system_clock::now().time_since_epoch().count(),
                   thread_id_str,
                   file,
                   line,
                   function,
                   context};
  for (const auto &sink : sinks) {
    sink->log(record);
  }
}

} // namespace dhquant
