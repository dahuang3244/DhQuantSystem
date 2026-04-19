#include "dhquant/core/clock.h"

namespace dhquant {

static int64_t system_clock_now() {
  return std::chrono::system_clock::now().time_since_epoch().count();
}

// 实盘模式
std::int64_t core::LiveClock::now() const noexcept {
  return system_clock_now();
}

Result<void> core::LiveClock::advance_to(std::int64_t timestamp) {
  // 实盘模式不支持时间推进
  return DH_ERR(ErrorCode::kStateError,
                "LiveClock does not support advance_to");
}

Result<void> core::LiveClock::schedule_at(std::int64_t timestamp,
                                          const EventEnvelope &event) {
  // 实盘模式暂不实现调度功能
  return DH_ERR(ErrorCode::kStateError,
                "LiveClock does not support schedule_at");
}

// 回测模式
std::int64_t core::BacktestClock::now() const noexcept {
  // 回测模式的当前时间由用户控制，初始值为0
  return now_;
}

Result<void> core::BacktestClock::advance_to(std::int64_t timestamp) {
  if (timestamp < now_) {
    return DH_ERR(ErrorCode::kInvalidArgument,
                  "Cannot advance to a past timestamp");
  }
  now_ = timestamp;
  return DH_OK();
}

Result<void> core::BacktestClock::schedule_at(std::int64_t timestamp,
                                              const EventEnvelope &event) {
  // 回测模式暂不实现调度功能
  return DH_ERR(ErrorCode::kStateError,
                "BacktestClock does not support schedule_at");
}

// 手动模式
core::ManualClock::ManualClock(std::int64_t initial_timestamp) noexcept
    : now_(initial_timestamp) {}

std::int64_t core::ManualClock::now() const noexcept { return now_; }

Result<void> core::ManualClock::advance_to(std::int64_t timestamp) {
  // 测试环境允许任意设置时间（包括回退），方便构造各种时序场景
  now_ = timestamp;
  return DH_OK();
}

Result<void>
core::ManualClock::schedule_at(std::int64_t /*timestamp*/,
                               const core::EventEnvelope & /*event*/) {
  return DH_ERR(ErrorCode::kStateError,
                "ManualClock::schedule_at: not implemented in Phase 2");
}

} // namespace dhquant
