#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <utils/Log.hpp>

namespace {

class CapturingSink final : public dhquant::LogSink {
public:
  void log(const dhquant::LogRecord &record) override {
    records.push_back(record);
  }

  std::vector<dhquant::LogRecord> records;
};

void EmitInfoLogFromHelper() { LOGI("macro-test", "macro message"); }

dhquant::Result::Result<int>
ReturnOnError(const dhquant::Result::Result<int> &result) {
  DH_RETURN_IF_ERROR(result, "macro-test", "propagating failure");
  return dhquant::Result::Result<int>::Ok(result.value() + 1);
}

TEST(LogTest, LoggerDispatchesRecordToSink) {
  auto sink = std::make_shared<CapturingSink>();
  dhquant::Logger::instance().AddSink(sink);

  dhquant::Logger::instance().Log(dhquant::LogLevel::QInfo, "core", "hello",
                                  "test_file.cpp", 12, "TestFunction");

  ASSERT_FALSE(sink->records.empty());
  const auto &record = sink->records.back();
  EXPECT_EQ(record.level, dhquant::LogLevel::QInfo);
  EXPECT_EQ(record.moudle, "core");
  EXPECT_EQ(record.message, "hello");
  EXPECT_EQ(record.file, "test_file.cpp");
  EXPECT_EQ(record.line, 12);
  EXPECT_EQ(record.function, "TestFunction");
}

TEST(LogTest, LoggerPreservesContextAndGeneratedFields) {
  auto sink = std::make_shared<CapturingSink>();
  dhquant::Logger::instance().AddSink(sink);

  dhquant::Logger::instance().Log(dhquant::LogLevel::QWarn, "risk",
                                  "check failed", "risk.cpp", 99, "CheckRisk",
                                  {{"symbol", "ETHUSDT"}, {"limit", "100"}});

  ASSERT_FALSE(sink->records.empty());
  const auto &record = sink->records.back();
  EXPECT_EQ(record.context.at("symbol"), "ETHUSDT");
  EXPECT_EQ(record.context.at("limit"), "100");
  EXPECT_GT(record.timestamp, 0);
  EXPECT_FALSE(record.threadId.empty());
}

TEST(LogTest, LoggerDispatchesToMultipleSinks) {
  auto sink_a = std::make_shared<CapturingSink>();
  auto sink_b = std::make_shared<CapturingSink>();
  dhquant::Logger::instance().AddSink(sink_a);
  dhquant::Logger::instance().AddSink(sink_b);

  dhquant::Logger::instance().Log(dhquant::LogLevel::QDebug, "multi", "fan out",
                                  "multi.cpp", 21, "FanOut");

  ASSERT_FALSE(sink_a->records.empty());
  ASSERT_FALSE(sink_b->records.empty());
  EXPECT_EQ(sink_a->records.back().message, "fan out");
  EXPECT_EQ(sink_b->records.back().message, "fan out");
}

TEST(LogTest, LogMacroCapturesSourceLocation) {
  auto sink = std::make_shared<CapturingSink>();
  dhquant::Logger::instance().AddSink(sink);
  const std::size_t initial_size = sink->records.size();

  EmitInfoLogFromHelper();

  ASSERT_GT(sink->records.size(), initial_size);
  const auto &record = sink->records.back();
  EXPECT_EQ(record.level, dhquant::LogLevel::QInfo);
  EXPECT_EQ(record.moudle, "macro-test");
  EXPECT_EQ(record.message, "macro message");
  EXPECT_NE(record.file.find("test_log.cpp"), std::string::npos);
  EXPECT_EQ(record.function, "EmitInfoLogFromHelper");
  EXPECT_GT(record.line, 0);
}

TEST(LogTest, ReturnIfErrorLogsAndPropagatesFailure) {
  auto sink = std::make_shared<CapturingSink>();
  dhquant::Logger::instance().AddSink(sink);
  const std::size_t initial_size = sink->records.size();

  auto result = ReturnOnError(dhquant::Result::Result<int>::Err(
      {ErrorCode::ErrorCode::kStateError, "engine offline"}));

  ASSERT_FALSE(result.ok());
  ASSERT_GT(sink->records.size(), initial_size);
  const auto &record = sink->records.back();
  EXPECT_EQ(record.level, dhquant::LogLevel::QError);
  EXPECT_EQ(record.moudle, "macro-test");
  EXPECT_NE(record.message.find("propagating failure"), std::string::npos);
  EXPECT_NE(record.message.find("engine offline"), std::string::npos);
  EXPECT_EQ(record.function, "ReturnOnError");
}

TEST(LogTest, ReturnIfErrorAllowsSuccessPath) {
  auto result = ReturnOnError(dhquant::Result::Result<int>::Ok(41));

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(result.value(), 42);
}

} // namespace
