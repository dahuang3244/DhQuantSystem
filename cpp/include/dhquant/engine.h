#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "dhquant/core/config.h"
#include "dhquant/core/event.h"
#include "dhquant/core/runtime_context.h"
#include "dhquant/domain.h"
#include "utils/Result.hpp"

namespace dhquant {

enum class EngineLifecycleState : std::uint8_t {
  kCreated = 0,
  kStarting = 1,
  kRunning = 2,
  kReplaying = 3,
  kStopping = 4,
  kStopped = 5,
  kFailed = 6,
};

struct EngineStatus {
  RuntimeMode mode{RuntimeMode::kUnknown};
  bool running{false};
  EngineLifecycleState state{EngineLifecycleState::kCreated};
  std::string last_error;
};

// Engine 是 C++ Core 的生命周期门面。
// 你后续实现时，推荐把真正的装配、回放、停机细节都放进 core::RuntimeContext，
// Engine 自己只保留 start/stop/submit/status 这类稳定控制接口。
class Engine {
public:
  explicit Engine(RuntimeMode mode = RuntimeMode::kBacktest) noexcept;
  explicit Engine(core::EngineConfig config) noexcept;

  // 目前保持 void 以兼容现有 binding.cpp；后续可以统一收敛为 Result<void>。
  void start() noexcept;
  void stop() noexcept;

  // submit 是统一事件入口。外部对象进入内核前，应先转成 core::EventEnvelope。
  dhquant::Result<std::uint64_t> submit(core::EventEnvelope event);

  // Phase 4: 回测接口
  dhquant::Result<void> load_replay(const std::string &csv_path);
  dhquant::Result<void>
  run_backtest(std::function<void(const Bar &)> on_bar,
               std::function<void(const Order &)> on_order,
               std::function<void(const Trade &)> on_trade);

  // 这两个接口先作为回放能力占位，后续由 ReplayRunner 真正承接。
  dhquant::Result<void> replay_from_offset(std::uint64_t offset);
  dhquant::Result<void> replay_between(std::int64_t begin_ts,
                                       std::int64_t end_ts);

  [[nodiscard]] EngineStatus status() const noexcept;
  [[nodiscard]] std::string_view name() const noexcept;
  [[nodiscard]] std::int64_t clock_now() const noexcept;

private:
  EngineStatus status_{};
  core::EngineConfig config_{};
  core::RuntimeContext runtime_{};
  std::vector<Bar> replay_bars_{};
};

} // namespace dhquant
