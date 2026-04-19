#include "dhquant/engine.h"
#include "dhquant/core/backtest_matcher.h"
#include "dhquant/core/backtest_replay_reader.h"
#include "dhquant/core/clock.h"
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

class OmsTradeHandler : public core::IEventHandler {
public:
  OmsTradeHandler(oms::OrderManager *order_manager, portfolio::Ledger *ledger,
                  core::EventLoop *event_loop)
      : order_manager_(order_manager), ledger_(ledger),
        event_loop_(event_loop) {}

  Result<void> handle(const core::EventEnvelope &event) override {
    if (event.event_type != core::EventType::kTrade) {
      return Result<void>::Ok();
    }

    const auto &trade = std::get<Trade>(event.payload);
    auto updated_result =
        order_manager_->on_trade(trade, event.ts_event, event.ts_process);
    if (!updated_result.ok()) {
      return Result<void>::Err(updated_result.error());
    }

    const auto &updated_order = updated_result.value();
    ledger_->on_trade(trade, updated_order);

    if (!event_loop_) {
      return Result<void>::Ok();
    }

    core::EventEnvelope order_env;
    order_env.event_type = core::EventType::kOrder;
    order_env.source = event.source;
    order_env.ts_event = updated_order.ts_event;
    order_env.payload = updated_order;
    auto post_result = event_loop_->post(std::move(order_env));
    if (!post_result.ok()) {
      return Result<void>::Err(post_result.error());
    }

    core::EventEnvelope portfolio_env;
    portfolio_env.event_type = core::EventType::kPortfolio;
    portfolio_env.source = event.source;
    portfolio_env.ts_event = trade.ts_event;
    portfolio_env.payload = ledger_->snapshot();
    post_result = event_loop_->post(std::move(portfolio_env));
    if (!post_result.ok()) {
      return Result<void>::Err(post_result.error());
    }

    return Result<void>::Ok();
  }

private:
  oms::OrderManager *order_manager_{nullptr};
  portfolio::Ledger *ledger_{nullptr};
  core::EventLoop *event_loop_{nullptr};
};

std::string next_order_id(const std::uint64_t sequence) {
  return "ord-" + std::to_string(sequence);
}

Instrument make_default_instrument(const std::string &instrument_id) {
  Instrument instrument;
  instrument.instrument_id = instrument_id;
  instrument.instrument_type = InstrumentType::kStock;
  instrument.lot_size = 100;
  instrument.price_tick = 0.01;

  const auto dot_pos = instrument_id.find('.');
  if (dot_pos == std::string::npos) {
    instrument.exchange = "";
    instrument.symbol = instrument_id;
    return instrument;
  }

  instrument.exchange = instrument_id.substr(0, dot_pos);
  instrument.symbol = instrument_id.substr(dot_pos + 1);
  return instrument;
}

Order build_order_from_intent(const OrderIntent &intent,
                              const std::string &order_id,
                              const std::int64_t ts_now) {
  Order order;
  order.session_id = "default-session";
  order.order_id = order_id;
  order.instrument_id = intent.instrument_id;
  order.side = intent.side;
  order.offset = intent.offset;
  order.order_type = intent.order_type;
  order.quantity = intent.quantity;
  order.price = intent.price;
  order.ts_event = ts_now;
  order.ts_process = ts_now;
  return order;
}

core::EventEnvelope make_order_event(const Order &order,
                                     const core::EventSource source) {
  core::EventEnvelope env;
  env.event_type = core::EventType::kOrder;
  env.source = source;
  env.ts_event = order.ts_event;
  env.payload = order;
  return env;
}

core::EventEnvelope make_risk_event(const RiskEvent &risk_event,
                                    const core::EventSource source) {
  core::EventEnvelope env;
  env.event_type = core::EventType::kRisk;
  env.source = source;
  env.ts_event = risk_event.ts_event;
  env.payload = risk_event;
  return env;
}

core::EventEnvelope make_portfolio_event(const PortfolioSnapshot &snapshot,
                                         const std::int64_t ts_event,
                                         const core::EventSource source) {
  core::EventEnvelope env;
  env.event_type = core::EventType::kPortfolio;
  env.source = source;
  env.ts_event = ts_event;
  env.payload = snapshot;
  return env;
}

Result<void> post_event(core::EventLoop *event_loop,
                        core::EventEnvelope event) {
  if (!event_loop) {
    return Error::State("event loop is not initialized");
  }

  auto post_result = event_loop->post(std::move(event));
  if (!post_result.ok()) {
    return post_result.error();
  }
  return Result<void>::Ok();
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
  order_manager_ = oms::OrderManager{};
  ledger_ = portfolio::Ledger{};
  risk_gate_.replace_instruments(instruments_);

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
  instruments_.clear();
  for (const auto &bar : replay_bars_) {
    instruments_.try_emplace(bar.instrument_id,
                             make_default_instrument(bar.instrument_id));
  }
  risk_gate_.replace_instruments(instruments_);

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
  auto trade_oms = std::make_shared<OmsTradeHandler>(&order_manager_, &ledger_,
                                                     runtime_.event_loop.get());

  auto bar_chain = std::make_shared<ChainEventHandler>(
      std::vector<core::EventHandlerPtr>{bridge, matcher});
  auto order_chain = std::make_shared<ChainEventHandler>(
      std::vector<core::EventHandlerPtr>{bridge, matcher});
  auto trade_chain = std::make_shared<ChainEventHandler>(
      std::vector<core::EventHandlerPtr>{trade_oms, bridge});

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
    if (runtime_.clock) {
      auto advance_result = runtime_.clock->advance_to(bar.ts_event);
      if (!advance_result.ok()) {
        return advance_result;
      }
    }

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

Result<Order> Engine::submit_intent(const OrderIntent &intent) {
  if (status_.state != EngineLifecycleState::kRunning &&
      status_.state != EngineLifecycleState::kReplaying) {
    return Error::State("Engine::submit_intent: engine not accepting orders");
  }
  if (!runtime_.event_loop) {
    return Error::State("Engine::submit_intent: event_loop is not initialized");
  }

  const auto order_id = next_order_id(next_order_id_++);
  const auto ts_now = clock_now();
  auto order = build_order_from_intent(intent, order_id, ts_now);

  const auto risk_result = risk_gate_.check(intent, ledger_.snapshot());
  RiskEvent risk_event;
  risk_event.order_id = order_id;
  risk_event.instrument_id = intent.instrument_id;
  risk_event.passed = risk_result.passed;
  risk_event.reason = risk_result.reason;
  risk_event.detail = risk_result.detail;
  risk_event.ts_event = ts_now;
  risk_event.ts_process = ts_now;
  auto post_result =
      post_event(runtime_.event_loop.get(),
                 make_risk_event(risk_event, core::EventSource::kManual));
  if (!post_result.ok()) {
    return post_result.error();
  }

  if (!risk_result.passed) {
    order.status = OrderStatus::kRejected;
    order.reject_reason = risk_result.reason;
    order_manager_.on_new_order(order);
    auto rejected_result =
        post_event(runtime_.event_loop.get(),
                   make_order_event(order, core::EventSource::kManual));
    if (!rejected_result.ok()) {
      return rejected_result.error();
    }
    return order;
  }

  order.status = OrderStatus::kPendingNew;
  order_manager_.on_new_order(order);

  auto accepted_result = order_manager_.transition_order(
      order.order_id, OrderStatus::kNew, order.ts_event, order.ts_process);
  if (!accepted_result.ok()) {
    return accepted_result.error();
  }

  const auto &accepted_order = accepted_result.value();
  ledger_.on_order_new(accepted_order);

  post_result =
      post_event(runtime_.event_loop.get(),
                 make_order_event(accepted_order, core::EventSource::kManual));
  if (!post_result.ok()) {
    return post_result.error();
  }

  post_result = post_event(runtime_.event_loop.get(),
                           make_portfolio_event(ledger_.snapshot(),
                                                accepted_order.ts_event,
                                                core::EventSource::kManual));
  if (!post_result.ok()) {
    return post_result.error();
  }

  return accepted_order;
}

Result<Order> Engine::cancel_order(const std::string_view order_id) {
  if (status_.state != EngineLifecycleState::kRunning &&
      status_.state != EngineLifecycleState::kReplaying) {
    return Error::State("Engine::cancel_order: engine not accepting cancels");
  }
  if (!runtime_.event_loop) {
    return Error::State("Engine::cancel_order: event_loop is not initialized");
  }

  const auto ts_now = clock_now();
  auto pending_result = order_manager_.transition_order(
      order_id, OrderStatus::kCancelPending, ts_now, ts_now);
  if (!pending_result.ok()) {
    return pending_result.error();
  }

  auto cancelled_result = order_manager_.transition_order(
      order_id, OrderStatus::kCancelled, ts_now, ts_now);
  if (!cancelled_result.ok()) {
    return cancelled_result.error();
  }

  const auto &cancelled_order = cancelled_result.value();
  ledger_.on_order_cancelled(cancelled_order);

  auto post_result =
      post_event(runtime_.event_loop.get(),
                 make_order_event(cancelled_order, core::EventSource::kManual));
  if (!post_result.ok()) {
    return post_result.error();
  }

  post_result = post_event(runtime_.event_loop.get(),
                           make_portfolio_event(ledger_.snapshot(),
                                                cancelled_order.ts_event,
                                                core::EventSource::kManual));
  if (!post_result.ok()) {
    return post_result.error();
  }

  return cancelled_order;
}

std::optional<Order>
Engine::get_order(const std::string_view order_id) const noexcept {
  return order_manager_.find_order(order_id);
}

PortfolioSnapshot Engine::get_portfolio_snapshot() const {
  return ledger_.snapshot();
}

Result<core::EventEnvelope>
Engine::read_journal(const std::uint64_t offset) const {
  if (!runtime_.journal) {
    return Error::State("Engine::read_journal: journal is not initialized");
  }
  return runtime_.journal->read(offset);
}

std::size_t Engine::journal_size() const noexcept {
  if (!runtime_.journal) {
    return 0;
  }
  return runtime_.journal->size();
}

EngineStatus Engine::status() const noexcept { return status_; }

std::string_view Engine::name() const noexcept { return "dhquant_engine"; }

} // namespace dhquant
