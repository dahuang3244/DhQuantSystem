#include "utils/Log.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace dhquant {
class FileSink : public LogSink {
public:
  explicit FileSink(std::string path) : file_path(std::move(path)) {
    // 确保目录存在
    std::filesystem::create_directories(
        std::filesystem::path(file_path).parent_path());
    // 打开文件
    file_stream.open(file_path, std::ios::app);
    if (!file_stream.is_open()) {
      throw std::runtime_error("Failed to open log file: " + file_path);
    }
  }

  ~FileSink() override {
    if (file_stream.is_open()) {
      file_stream.close();
    }
  }

  void log(const LogRecord &record) override {
    if (file_stream.is_open()) {
      std::string line =
          level_str(record.level) + "[" + record.module + "]:" + record.message;
      for (auto &kv : record.context) {
        line += " " + kv.first + "=" + kv.second;
      }
      file_stream << line << "\n";
      file_stream.flush(); // 确保日志及时写入文件
    }
  }

private:
  std::string file_path;
  std::ofstream file_stream;
};
} // namespace dhquant