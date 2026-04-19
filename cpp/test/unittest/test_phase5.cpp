#include <gtest/gtest.h>

#include <cmath>
#include <filesystem>
#include <fstream>

#include "dhquant/engine.h"
#include "dhquant/oms/order_state_machine.h"
#include "dhquant/portfolio/ledger.h"
#include "dhquant/risk/risk_gate.h"

namespace {

namespace fs = std::filesystem;

fs::path write_temp_csv(const std::string &name, const std::string &content) {
  const auto path = fs::temp_directory_path() / name;
  std::ofstream out(path);
  out << content;
  out.close();
  return path;
}

TEST(OrderStateMachineTest, ValidAndInvalidTransitionsAreChecked) {
  dhquant::oms::OrderStateMachine state_machine;

  EXPECT_TRUE(state_machine.is_valid_transition(
      dhquant::OrderStatus::kPendingNew, dhquant::OrderStatus::kNew));
  EXPECT_TRUE(state_machine.is_valid_transition(dhquant::OrderStatus::kNew,
                                                dhquant::OrderStatus::kFilled));
  EXPECT_FALSE(state_machine.is_valid_transition(dhquant::OrderStatus::kFilled,
                                                 dhquant::OrderStatus::kNew));
}

TEST(LedgerTest, BuyOrderFreezesCashAndTradeReleasesDifference) {
  dhquant::portfolio::Ledger ledger(1'000'000.0, 0.001);

  dhquant::Order order;
  order.order_id = "ord-1";
  order.instrument_id = "SSE.600000";
  order.side = dhquant::Side::kBuy;
  order.offset = dhquant::Offset::kOpen;
  order.order_type = dhquant::OrderType::kLimit;
  order.status = dhquant::OrderStatus::kNew;
  order.quantity = 100;
  order.price = 10.5;

  ledger.on_order_new(order);
  EXPECT_NEAR(ledger.cash(), 998948.95, 1e-6);
  EXPECT_NEAR(ledger.frozen_cash(), 1051.05, 1e-6);

  dhquant::Trade trade;
  trade.order_id = order.order_id;
  trade.instrument_id = order.instrument_id;
  trade.side = dhquant::Side::kBuy;
  trade.offset = dhquant::Offset::kOpen;
  trade.fill_quantity = 100;
  trade.fill_price = 10.2;
  trade.commission = 1.02;

  order.status = dhquant::OrderStatus::kFilled;
  order.filled_quantity = 100;
  order.average_fill_price = 10.2;

  ledger.on_trade(trade, order);
  EXPECT_NEAR(ledger.cash(), 998978.98, 1e-6);
  EXPECT_NEAR(ledger.frozen_cash(), 0.0, 1e-6);
  EXPECT_NEAR(ledger.equity(), 999998.98, 1e-6);

  const auto position = ledger.get_position("SSE.600000");
  ASSERT_TRUE(position.has_value());
  EXPECT_EQ(position->quantity_total, 100);
  EXPECT_NEAR(position->average_price, 10.2, 1e-6);
}

TEST(RiskGateTest, RejectsInsufficientFundsAndUnknownInstrument) {
  dhquant::risk::RiskGate risk_gate;
  risk_gate.replace_instruments(
      {{"SSE.600000", dhquant::Instrument{
                          .instrument_id = "SSE.600000",
                          .exchange = "SSE",
                          .symbol = "600000",
                          .instrument_type = dhquant::InstrumentType::kStock,
                          .lot_size = 100,
                          .price_tick = 0.01,
                          .currency = "CNY",
                      }}});

  dhquant::PortfolioSnapshot snapshot;
  snapshot.account_state.cash = 100.0;

  dhquant::OrderIntent too_expensive;
  too_expensive.instrument_id = "SSE.600000";
  too_expensive.side = dhquant::Side::kBuy;
  too_expensive.offset = dhquant::Offset::kOpen;
  too_expensive.order_type = dhquant::OrderType::kLimit;
  too_expensive.quantity = 100;
  too_expensive.price = 10.0;

  const auto funds_result = risk_gate.check(too_expensive, snapshot);
  EXPECT_FALSE(funds_result.passed);
  EXPECT_EQ(funds_result.reason, dhquant::RejectReason::kRiskRejected);

  dhquant::OrderIntent unknown = too_expensive;
  unknown.instrument_id = "SSE.999999";
  const auto instrument_result = risk_gate.check(unknown, snapshot);
  EXPECT_FALSE(instrument_result.passed);
  EXPECT_EQ(instrument_result.reason,
            dhquant::RejectReason::kInvalidInstrument);
}

TEST(BacktestEnginePhase5Test,
     SubmitIntentUpdatesLedgerAndArchivesRejectedOrders) {
  const auto csv_path = write_temp_csv(
      "dhquant_backtest_engine_phase5.csv",
      "ts_event,instrument_id,open,high,low,close,volume,turnover\n"
      "1713500000000,SSE.600000,10.0,10.5,9.9,10.2,1000,10200.0\n");

  dhquant::Engine engine(dhquant::RuntimeMode::kBacktest);
  ASSERT_TRUE(engine.load_replay(csv_path.string()).ok());
  engine.start();

  dhquant::OrderIntent intent;
  intent.instrument_id = "SSE.600000";
  intent.side = dhquant::Side::kBuy;
  intent.offset = dhquant::Offset::kOpen;
  intent.order_type = dhquant::OrderType::kLimit;
  intent.quantity = 100;
  intent.price = 10.5;

  const auto submit_result = engine.submit_intent(intent);
  ASSERT_TRUE(submit_result.ok()) << submit_result.error().message;
  const auto accepted = submit_result.value();
  EXPECT_EQ(accepted.status, dhquant::OrderStatus::kNew);

  const auto frozen_snapshot = engine.get_portfolio_snapshot();
  EXPECT_NEAR(frozen_snapshot.account_state.cash, 998948.95, 1e-6);
  EXPECT_NEAR(frozen_snapshot.account_state.frozen_cash, 1051.05, 1e-6);

  std::vector<dhquant::Trade> trades;
  ASSERT_TRUE(engine
                  .run_backtest([](const dhquant::Bar &) {},
                                [](const dhquant::Order &) {},
                                [&](const dhquant::Trade &trade) {
                                  trades.push_back(trade);
                                })
                  .ok());
  ASSERT_EQ(trades.size(), 1U);

  const auto final_order = engine.get_order(accepted.order_id);
  ASSERT_TRUE(final_order.has_value());
  EXPECT_EQ(final_order->status, dhquant::OrderStatus::kFilled);

  const auto final_snapshot = engine.get_portfolio_snapshot();
  EXPECT_NEAR(final_snapshot.account_state.cash, 998978.98, 1e-6);
  EXPECT_NEAR(final_snapshot.account_state.frozen_cash, 0.0, 1e-6);
  ASSERT_EQ(final_snapshot.positions.size(), 1U);
  EXPECT_EQ(final_snapshot.positions.front().quantity_total, 100);

  dhquant::OrderIntent rejected = intent;
  rejected.instrument_id = "SSE.999999";
  const auto rejected_result = engine.submit_intent(rejected);
  ASSERT_TRUE(rejected_result.ok()) << rejected_result.error().message;
  EXPECT_EQ(rejected_result.value().status, dhquant::OrderStatus::kRejected);
  EXPECT_EQ(rejected_result.value().reject_reason,
            dhquant::RejectReason::kInvalidInstrument);

  const auto rejected_order =
      engine.get_order(rejected_result.value().order_id);
  ASSERT_TRUE(rejected_order.has_value());
  EXPECT_EQ(rejected_order->status, dhquant::OrderStatus::kRejected);
}

TEST(BacktestEnginePhase5Test,
     JournalCapturesRiskOrderTradeAndPortfolioAuditEvents) {
  const auto csv_path = write_temp_csv(
      "dhquant_backtest_engine_phase5_journal.csv",
      "ts_event,instrument_id,open,high,low,close,volume,turnover\n"
      "1713500000000,SSE.600000,10.0,10.5,9.9,10.2,1000,10200.0\n");

  dhquant::Engine engine(dhquant::RuntimeMode::kBacktest);
  ASSERT_TRUE(engine.load_replay(csv_path.string()).ok());
  engine.start();

  dhquant::OrderIntent accepted;
  accepted.instrument_id = "SSE.600000";
  accepted.side = dhquant::Side::kBuy;
  accepted.offset = dhquant::Offset::kOpen;
  accepted.order_type = dhquant::OrderType::kLimit;
  accepted.quantity = 100;
  accepted.price = 10.5;
  const auto accepted_result = engine.submit_intent(accepted);
  ASSERT_TRUE(accepted_result.ok()) << accepted_result.error().message;

  dhquant::OrderIntent rejected = accepted;
  rejected.instrument_id = "SSE.999999";
  const auto rejected_result = engine.submit_intent(rejected);
  ASSERT_TRUE(rejected_result.ok()) << rejected_result.error().message;
  ASSERT_TRUE(engine
                  .run_backtest([](const dhquant::Bar &) {},
                                [](const dhquant::Order &) {},
                                [](const dhquant::Trade &) {})
                  .ok());

  bool saw_risk_accept = false;
  bool saw_risk_reject = false;
  bool saw_trade = false;
  bool saw_filled_order = false;
  bool saw_portfolio_position = false;

  for (std::size_t index = 0; index < engine.journal_size(); ++index) {
    const auto event_result = engine.read_journal(index);
    ASSERT_TRUE(event_result.ok()) << event_result.error().message;
    const auto &event = event_result.value();

    if (event.event_type == dhquant::core::EventType::kRisk) {
      const auto &risk_event = std::get<dhquant::RiskEvent>(event.payload);
      if (risk_event.order_id == accepted_result.value().order_id) {
        saw_risk_accept = risk_event.passed;
      }
      if (risk_event.order_id == rejected_result.value().order_id) {
        saw_risk_reject =
            !risk_event.passed &&
            risk_event.reason == dhquant::RejectReason::kInvalidInstrument;
      }
    }

    if (event.event_type == dhquant::core::EventType::kTrade) {
      const auto &trade = std::get<dhquant::Trade>(event.payload);
      if (trade.order_id == accepted_result.value().order_id &&
          trade.fill_quantity == 100) {
        saw_trade = true;
      }
    }

    if (event.event_type == dhquant::core::EventType::kOrder) {
      const auto &order = std::get<dhquant::Order>(event.payload);
      if (order.order_id == accepted_result.value().order_id &&
          order.status == dhquant::OrderStatus::kFilled) {
        saw_filled_order = true;
      }
    }

    if (event.event_type == dhquant::core::EventType::kPortfolio) {
      const auto &snapshot =
          std::get<dhquant::PortfolioSnapshot>(event.payload);
      for (const auto &position : snapshot.positions) {
        if (position.instrument_id == "SSE.600000" &&
            position.quantity_total == 100) {
          saw_portfolio_position = true;
        }
      }
    }
  }

  EXPECT_TRUE(saw_risk_accept);
  EXPECT_TRUE(saw_risk_reject);
  EXPECT_TRUE(saw_trade);
  EXPECT_TRUE(saw_filled_order);
  EXPECT_TRUE(saw_portfolio_position);
}

} // namespace
