#include "dhquant/engine.h"
#include "dhquant/core/clock.h"
#include "dhquant/core/journal.h"
#include "utils/Log.hpp"

namespace dhquant {

namespace {

core::EngineConfig normalize_engine_config(core::EngineConfig config) {
  if (config.event_loop.journal_config.root_dir.empty()) {
    config.event_loop.journal_config.root_dir = "memory://default";
  }
  return config;
}

} // namespace

Engine::Engine(RuntimeMode mode) noexcept
    : status_{.mode = mode,
              .running = false,
              .state = EngineLifecycleState::kCreated} {
  config_.mode = mode;
  config_ = normalize_engine_config(std::move(config_));
}

Engine::Engine(core::EngineConfig config) noexcept
    : config_(normalize_engine_config(std::move(config))) {
  status_.mode = config_.mode;
  status_.state = EngineLifecycleState::kCreated;
}

void Engine::start() noexcept {
  status_.state = EngineLifecycleState::kStarting;
  config_ = normalize_engine_config(std::move(config_));
  runtime_.config = config_;

  // ① 根据运行模式选择时钟实现
  switch (config_.mode) {
  case RuntimeMode::kLive:
    runtime_.clock = std::make_shared<core::LiveClock>();
    break;
  case RuntimeMode::kBacktest:
    runtime_.clock = std::make_shared<core::BacktestClock>();
    break;
  default:
    // Paper / Unknown → ManualClock 兜底
    runtime_.clock = std::make_shared<core::ManualClock>();
    break;
  }

  // ② 创建 Journal（Phase 2 内存版）
  runtime_.journal = std::make_shared<core::InMemoryJournal>();

  // ③ 创建 Dispatcher
  runtime_.dispatcher = std::make_shared<core::Dispatcher>();

  // ④ 装配 EventLoop
  runtime_.event_loop = std::make_shared<core::EventLoop>(
      runtime_.clock, runtime_.journal, runtime_.dispatcher);
  runtime_.shutdown_hook =
      std::make_shared<core::DefaultShutdownHook>(runtime_.event_loop);

  // ⑤ 配置 EventLoop
  auto cfg_r = runtime_.event_loop->configure(config_.event_loop);
  if (!cfg_r.ok()) {
    status_.state = EngineLifecycleState::kFailed;
    status_.last_error = cfg_r.error().message;
    LOGE("engine", "configure failed: " + cfg_r.error().message);
    return;
  }

  // ⑥ 启动 EventLoop
  auto start_r = runtime_.event_loop->start();
  if (!start_r.ok()) {
    status_.state = EngineLifecycleState::kFailed;
    status_.last_error = start_r.error().message;
    LOGE("engine", "event_loop start failed: " + start_r.error().message);
    return;
  }

  status_.running = true;
  status_.state = EngineLifecycleState::kRunning;
  LOGI("engine", "Engine started");
}

void Engine::stop() noexcept {
  if (status_.state != EngineLifecycleState::kRunning) {
    return;
  }
  status_.state = EngineLifecycleState::kStopping;
  LOGI("engine", "Engine stopping...");

  if (!runtime_.shutdown_hook) {
    status_.state = EngineLifecycleState::kFailed;
    status_.last_error = "Engine::stop: shutdown hook is not initialized";
    LOGE("engine", status_.last_error);
    return;
  }

  auto prepare_r = runtime_.shutdown_hook->prepare_stop(config_.stop_mode);
  if (!prepare_r.ok()) {
    status_.state = EngineLifecycleState::kFailed;
    status_.last_error = prepare_r.error().message;
    LOGE("engine", "shutdown prepare error: " + prepare_r.error().message);
    return;
  }

  auto r = runtime_.shutdown_hook->flush_and_join();
  if (!r.ok()) {
    LOGE("engine", "event_loop stop error: " + r.error().message);
  }

  status_.running = false;
  status_.state = EngineLifecycleState::kStopped;
  LOGI("engine", "Engine stopped");
}

Result<uint64_t> Engine::submit(core::EventEnvelope event) {
  if (status_.state != EngineLifecycleState::kRunning) {
    return DH_ERR(ErrorCode::kStateError, "Engine::submit: engine not running");
  }
  if (!runtime_.event_loop) {
    return DH_ERR(ErrorCode::kStateError,
                  "Engine::submit: event_loop is not initialized");
  }
  return runtime_.event_loop->post(std::move(event));
}

EngineStatus Engine::status() const noexcept { return status_; }

std::string_view Engine::name() const noexcept { return "dhquant_engine"; }

} // namespace dhquant
