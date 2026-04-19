#include "dhquant/engine.h"
#include "dhquant/core/backtest_matcher.h"
#include "dhquant/core/backtest_replay_reader.h"
#include "dhquant/core/python_bridge_handler.h"
#include "utils/Log.hpp"

namespace dhquant {

namespace {

core::EngineConfig normalize_engine_config(core::EngineConfig config) {
  if (config.event_loop.journal_config.root_dir.empty()) {
    config.event_loop.journal_config.root_dir = "memory://default";
  }
  return config;
}

class ChainEventHandler : public core::IEventHandler {
public:
  explicit ChainEventHandler(std::vector<core::EventHandlerPtr> handlers)
      : handlers_(std::move(handlers)) {}

  Result<void> handle(const core::EventEnvelope &event) override {
    for (const auto &handler : handlers_) {
      if (!handler) {
        continue;
      }
      auto result = handler->handle(event);
      if (!result.ok()) {
        return result;
      }
    }
    return Result<void>::Ok();
  }

private:
  std::vector<core::EventHandlerPtr> handlers_;
};

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

dhquant::Result<void> Engine::load_replay(const std::string &csv_path) {
  if (status_.mode != RuntimeMode::kBacktest) {
    return Result<void>::Err(
        Error::Invalid("load_replay only supported in BACKTEST mode"));
  }

  auto reader = std::make_unique<core::BacktestReplayReader>(csv_path);
  auto res = reader->load();
  if (!res.ok())
    return res;

  replay_bars_.clear();
  replay_bars_ = reader->get_bars();

  return Result<void>::Ok();
}

dhquant::Result<void>
Engine::run_backtest(std::function<void(const Bar &)> on_bar,
                     std::function<void(const Order &)> on_order,
                     std::function<void(const Trade &)> on_trade) {
  if (!status_.running)
    start();

  // 1. 设置 PythonBridgeHandler
  auto bridge = std::make_shared<core::PythonBridgeHandler>();
  bridge->set_on_bar(std::move(on_bar));
  bridge->set_on_order(std::move(on_order));
  bridge->set_on_trade(std::move(on_trade));

  // 2. 设置 BacktestMatcher
  auto matcher = std::make_shared<core::BacktestMatcher>();
  matcher->set_event_loop(runtime_.event_loop.get());

  auto bar_chain = std::make_shared<ChainEventHandler>(
      std::vector<core::EventHandlerPtr>{bridge, matcher});
  auto order_chain = std::make_shared<ChainEventHandler>(
      std::vector<core::EventHandlerPtr>{bridge, matcher});
  auto trade_chain = std::make_shared<ChainEventHandler>(
      std::vector<core::EventHandlerPtr>{bridge});

  // 3. 注册到 Dispatcher
  auto reg_bar = runtime_.dispatcher->register_handler(
      core::EventType::kMarketBar, bar_chain);
  if (!reg_bar.ok()) {
    return reg_bar;
  }
  auto reg_order = runtime_.dispatcher->register_handler(
      core::EventType::kOrder, order_chain);
  if (!reg_order.ok()) {
    return reg_order;
  }
  auto reg_trade = runtime_.dispatcher->register_handler(
      core::EventType::kTrade, trade_chain);
  if (!reg_trade.ok()) {
    return reg_trade;
  }

  if (replay_bars_.empty()) {
    return Result<void>::Err(
        Error::State("Replay bars not initialized. Call load_replay first."));
  }

  status_.state = EngineLifecycleState::kReplaying;

  for (const auto &bar : replay_bars_) {

    // 投递 Bar 事件
    core::EventEnvelope env;
    env.event_type = core::EventType::kMarketBar;
    env.source = core::EventSource::kReplay;
    env.ts_event = bar.ts_event;
    env.payload = bar;
    auto post_res = runtime_.event_loop->post(env);
    if (!post_res.ok()) {
      return Result<void>::Err(post_res.error());
    }

    // 处理当前时间点的所有事件
    auto res = runtime_.event_loop->drain();
    if (!res.ok())
      return res;
  }

  status_.state = EngineLifecycleState::kRunning;
  return Result<void>::Ok();
}

std::int64_t Engine::clock_now() const noexcept {
  if (runtime_.clock)
    return runtime_.clock->now();
  return 0;
}

Result<uint64_t> Engine::submit(core::EventEnvelope event) {
  if (status_.state != EngineLifecycleState::kRunning &&
      status_.state != EngineLifecycleState::kReplaying) {
    return DH_ERR(ErrorCode::kStateError,
                  "Engine::submit: engine not accepting events");
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
