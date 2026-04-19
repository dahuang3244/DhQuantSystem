#include "utils/Log.hpp"
#include <iostream>
#include <string>

namespace dhquant {

class ConsoleSink : public LogSink {
public:
  void log(const LogRecord &record) override {
    std::string line =
        level_str(record.level) + "[" + record.module + "]:" + record.message;
    for (auto &kv : record.context) {
      line += " " + kv.first + "=" + kv.second;
    }
    std::cout << line << "\n";
  }
};

} // namespace dhquant