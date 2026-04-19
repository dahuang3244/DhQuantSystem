#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "dhquant/core/backtest_replay_reader.h"
#include "dhquant/core/clock.h"
#include "dhquant/core/dispatcher.h"
#include "dhquant/core/event_loop.h"
#include "dhquant/core/handler.h"
#include "dhquant/core/journal.h"
#include "dhquant/engine.h"

using namespace dhquant::core;
using testing::_;
using testing::Invoke;
using testing::Return;

namespace {

namespace fs = std::filesystem;

// Mock Handler 用于验证事件是否正确分发
class MockHandler : public IEventHandler {
public:
  MOCK_METHOD(dhquant::Result<void>, handle, (const EventEnvelope &event),
              (override));
};

// 单元测试类
class CoreEngineTest : public ::testing::Test {
protected:
  void SetUp() override {
    // 使用 ManualClock 以便精确控制时间
    clock = std::make_shared<ManualClock>(1000000); // 初始时间 1000000
    // 使用 InMemoryJournal 简化环境
    journal = std::make_shared<InMemoryJournal>();
    // 核心组件 Dispatcher
    dispatcher = std::make_shared<Dispatcher>();

    // 创建 EventLoop
    event_loop = std::make_unique<EventLoop>(clock, journal, dispatcher);

    // 配置并启动
    EventLoopConfig config;
    config.journal_config.root_dir = "memory://test";
    ASSERT_TRUE(event_loop->configure(config).ok());
    ASSERT_TRUE(event_loop->start().ok());
  }

  std::shared_ptr<ManualClock> clock;
  std::shared_ptr<InMemoryJournal> journal;
  std::shared_ptr<Dispatcher> dispatcher;
  std::unique_ptr<EventLoop> event_loop;
};

fs::path write_temp_csv(const std::string &name, const std::string &content) {
  const auto path = fs::temp_directory_path() / name;
  std::ofstream out(path);
  out << content;
  out.close();
  return path;
}

TEST_F(CoreEngineTest, BasicEventFlow) {
  // 1. 注册一个 Mock Handler 到 MarketTick 事件
  auto mock_handler = std::make_shared<MockHandler>();
  ASSERT_TRUE(
      dispatcher->register_handler(EventType::kMarketTick, mock_handler).ok());

  // 2. 准备一个测试事件
  EventEnvelope event;
  event.event_type = EventType::kMarketTick;
  event.source = EventSource::kTest;
  event.ts_event = 1000500;

  // 3. 预设期望：handle 应该被调用一次，且 sequence_id 应该递增
  EXPECT_CALL(*mock_handler, handle(_))
      .WillOnce(Invoke([](const EventEnvelope &env) {
        EXPECT_EQ(env.event_type, EventType::kMarketTick);
        EXPECT_EQ(env.sequence_id, 1);
        EXPECT_EQ(env.ts_process, 1000000); // 匹配 ManualClock 时间
        return dhquant::Result<void>(); // 使用默认构造函数返回 OK (void)
      }));

  // 4. 投递事件
  auto post_res = event_loop->post(event);
  ASSERT_TRUE(post_res.ok());
  EXPECT_EQ(post_res.value(), 1);

  // 5. 调用 poll_once 触发处理
  ASSERT_TRUE(event_loop->poll_once().ok());

  // 6. 验证 Journal 中是否保留了记录
  // 当前 InMemoryJournal::read 使用的是 0-based offset，而不是 sequence_id。
  auto read_res = journal->read(0);
  ASSERT_TRUE(read_res.ok());
  EXPECT_EQ(read_res.value().sequence_id, 1);
}

TEST_F(CoreEngineTest, SequenceIdIncrements) {
  EventEnvelope ev1;
  ev1.event_type = EventType::kSystem;

  EventEnvelope ev2;
  ev2.event_type = EventType::kSystem;

  auto res1 = event_loop->post(ev1);
  auto res2 = event_loop->post(ev2);

  ASSERT_TRUE(res1.ok());
  ASSERT_TRUE(res2.ok());
  EXPECT_EQ(res1.value(), 1);
  EXPECT_EQ(res2.value(), 2);
}

TEST_F(CoreEngineTest, ClockIntegration) {
  EventEnvelope event;
  event.event_type = EventType::kSystem;

  // 推进时钟
  (void)clock->advance_to(2000000);

  (void)event_loop->post(event);

  // 第一条写入对应 journal offset 0。
  auto read_res = journal->read(0);
  ASSERT_TRUE(read_res.ok());
  EXPECT_EQ(read_res.value().ts_process, 2000000);
}

TEST_F(CoreEngineTest, DrainProcessesAllEvents) {
  auto mock_handler = std::make_shared<MockHandler>();
  ASSERT_TRUE(
      dispatcher->register_handler(EventType::kOrder, mock_handler).ok());

  // 投递多个事件
  (void)event_loop->post({0, EventType::kOrder});
  (void)event_loop->post({0, EventType::kOrder});
  (void)event_loop->post({0, EventType::kOrder});

  // 期望调用 3 次
  EXPECT_CALL(*mock_handler, handle(_))
      .Times(3)
      .WillRepeatedly(Return(dhquant::Result<void>()));

  // 执行 Drain
  ASSERT_TRUE(event_loop->drain().ok());
}

TEST(BacktestReplayReaderTest, LoadRejectsMalformedCsvRows) {
  const auto csv_path = write_temp_csv(
      "dhquant_backtest_reader_invalid.csv",
      "ts_event,instrument_id,open,high,low,close,volume,turnover\n"
      "1713500000000,SSE.600000,10.0,10.5,9.9,10.2,1000\n");

  BacktestReplayReader reader(csv_path.string());
  const auto result = reader.load();

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(result.error().code, dhquant::ErrorCode::kInvalidConfig);
}

TEST(BacktestEngineTest, FilledOrderIsNotMatchedAgainOnLaterBars) {
  const auto csv_path = write_temp_csv(
      "dhquant_backtest_engine_replay.csv",
      "ts_event,instrument_id,open,high,low,close,volume,turnover\n"
      "1713500000000,SSE.600000,10.0,10.5,9.9,10.2,1000,10200.0\n"
      "1713500060000,SSE.600000,10.2,10.8,10.1,10.7,1200,12840.0\n");

  dhquant::Engine engine(dhquant::RuntimeMode::kBacktest);
  ASSERT_TRUE(engine.load_replay(csv_path.string()).ok());
  engine.start();
  ASSERT_TRUE(engine.status().running);

  dhquant::Order order;
  order.session_id = "sess-1";
  order.order_id = "ord-1";
  order.instrument_id = "SSE.600000";
  order.side = dhquant::Side::kBuy;
  order.offset = dhquant::Offset::kOpen;
  order.order_type = dhquant::OrderType::kMarket;
  order.quantity = 100;

  EventEnvelope env;
  env.event_type = EventType::kOrder;
  env.source = EventSource::kManual;
  env.ts_event = 1713499999000;
  env.payload = order;
  ASSERT_TRUE(engine.submit(env).ok());

  std::vector<dhquant::Trade> trades;
  std::vector<dhquant::Order> orders;
  std::vector<dhquant::Bar> bars;
  const auto run_result = engine.run_backtest(
      [&](const dhquant::Bar &bar) { bars.push_back(bar); },
      [&](const dhquant::Order &updated) { orders.push_back(updated); },
      [&](const dhquant::Trade &trade) { trades.push_back(trade); });

  ASSERT_TRUE(run_result.ok()) << run_result.error().message;
  EXPECT_EQ(bars.size(), 2U);
  ASSERT_EQ(trades.size(), 1U);
  EXPECT_EQ(trades.front().fill_price, 10.2);
  EXPECT_EQ(trades.front().fill_quantity, 100);

  int filled_updates = 0;
  for (const auto &updated : orders) {
    if (updated.status == dhquant::OrderStatus::kFilled) {
      ++filled_updates;
    }
  }
  EXPECT_EQ(filled_updates, 1);

  engine.stop();
  EXPECT_EQ(engine.status().state, dhquant::EngineLifecycleState::kStopped);
}

TEST(BacktestEngineTest, SubmitAcceptsOrdersDuringReplayCallbacks) {
  const auto csv_path = write_temp_csv(
      "dhquant_backtest_engine_callback_submit.csv",
      "ts_event,instrument_id,open,high,low,close,volume,turnover\n"
      "1713500000000,SSE.600000,10.0,10.5,9.9,10.2,1000,10200.0\n"
      "1713500060000,SSE.600000,10.2,10.8,10.1,10.7,1200,12840.0\n");

  dhquant::Engine engine(dhquant::RuntimeMode::kBacktest);
  ASSERT_TRUE(engine.load_replay(csv_path.string()).ok());

  bool submitted = false;
  std::vector<dhquant::Trade> trades;
  const auto run_result = engine.run_backtest(
      [&](const dhquant::Bar &bar) {
        if (submitted) {
          return;
        }
        dhquant::Order order;
        order.session_id = "sess-2";
        order.order_id = "ord-2";
        order.instrument_id = bar.instrument_id;
        order.side = dhquant::Side::kBuy;
        order.offset = dhquant::Offset::kOpen;
        order.order_type = dhquant::OrderType::kMarket;
        order.quantity = 50;

        EventEnvelope env;
        env.event_type = EventType::kOrder;
        env.source = EventSource::kManual;
        env.ts_event = bar.ts_event;
        env.payload = order;
        const auto submit_result = engine.submit(env);
        ASSERT_TRUE(submit_result.ok()) << submit_result.error().message;
        submitted = true;
      },
      [](const dhquant::Order &) {},
      [&](const dhquant::Trade &trade) { trades.push_back(trade); });

  ASSERT_TRUE(run_result.ok()) << run_result.error().message;
  EXPECT_TRUE(submitted);
  ASSERT_EQ(trades.size(), 1U);
  EXPECT_EQ(trades.front().fill_quantity, 50);
}

} // namespace
