#pragma once

#include <cstdint>

#include "dhquant/core/event.h"
#include "utils/Result.hpp"

namespace dhquant::core {

// Clock 只负责“时间语义”，不要把调度逻辑塞进这里。
// 你后续实现时，LiveClock 可以接 system_clock，BacktestClock/ManualClock
// 用于可控推进。
class IClock {
public:
  virtual ~IClock() = default;

  // 返回当前引擎看到的“现在”。
  [[nodiscard]] virtual std::int64_t now() const noexcept = 0;

  // 推进时间，主要给 backtest/manual 模式使用。
  virtual Result<void> advance_to(std::int64_t timestamp) = 0;

  // 注册一个未来触发事件。现在只先保留接口，不实现调度容器。
  virtual Result<void> schedule_at(std::int64_t timestamp,
                                   const EventEnvelope &event) = 0;
};

class LiveClock final : public IClock {
public:
  LiveClock() = default;
  ~LiveClock() override = default;

  [[nodiscard]] std::int64_t now() const noexcept override;
  Result<void> advance_to(std::int64_t timestamp) override;
  Result<void> schedule_at(std::int64_t timestamp,
                           const EventEnvelope &event) override;
};

class BacktestClock final : public IClock {
public:
  BacktestClock() = default;
  ~BacktestClock() override = default;

  [[nodiscard]] std::int64_t now() const noexcept override;
  Result<void> advance_to(std::int64_t timestamp) override;
  Result<void> schedule_at(std::int64_t timestamp,
                           const EventEnvelope &event) override;

private:
  std::int64_t now_{0};
};

class ManualClock final : public IClock {
public:
  explicit ManualClock(std::int64_t initial_timestamp = 0) noexcept;
  ~ManualClock() override = default;

  [[nodiscard]] std::int64_t now() const noexcept override;
  Result<void> advance_to(std::int64_t timestamp) override;
  Result<void> schedule_at(std::int64_t timestamp,
                           const EventEnvelope &event) override;
  // ManualClock 的最小状态就是一个当前时间点。
  // 后面如果你要做 timer heap，可以再往这里扩。
private:
  std::int64_t now_{0};
};

} // namespace dhquant::core
