#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "dhquant/domain.h"
#include "dhquant/engine.h"

namespace py = pybind11;

namespace dhquant {

PYBIND11_MODULE(dhquant_cpp_binding, module) {
  module.doc() = "DhQuantTradeSystem core bindings";

  py::enum_<RuntimeMode>(module, "RuntimeMode")
      .value("UNKNOWN", RuntimeMode::kUnknown)
      .value("BACKTEST", RuntimeMode::kBacktest)
      .value("PAPER", RuntimeMode::kPaper)
      .value("LIVE", RuntimeMode::kLive);

  py::enum_<Side>(module, "Side")
      .value("UNKNOWN", Side::kUnknown)
      .value("BUY", Side::kBuy)
      .value("SELL", Side::kSell);

  py::enum_<Offset>(module, "Offset")
      .value("UNKNOWN", Offset::kUnknown)
      .value("OPEN", Offset::kOpen)
      .value("CLOSE", Offset::kClose)
      .value("CLOSE_TODAY", Offset::kCloseToday)
      .value("CLOSE_YESTERDAY", Offset::kCloseYesterday);

  py::enum_<OrderType>(module, "OrderType")
      .value("UNKNOWN", OrderType::kUnknown)
      .value("LIMIT", OrderType::kLimit)
      .value("MARKET", OrderType::kMarket);

  py::enum_<RejectReason>(module, "RejectReason")
      .value("NONE", RejectReason::kNone)
      .value("INVALID_INSTRUMENT", RejectReason::kInvalidInstrument)
      .value("INVALID_PRICE", RejectReason::kInvalidPrice)
      .value("INVALID_QUANTITY", RejectReason::kInvalidQuantity)
      .value("RISK_REJECTED", RejectReason::kRiskRejected)
      .value("GATEWAY_REJECTED", RejectReason::kGatewayRejected)
      .value("UNKNOWN", RejectReason::kUnknown);

  py::enum_<EngineLifecycleState>(module, "EngineLifecycleState")
      .value("CREATED", EngineLifecycleState::kCreated)
      .value("STARTING", EngineLifecycleState::kStarting)
      .value("RUNNING", EngineLifecycleState::kRunning)
      .value("REPLAYING", EngineLifecycleState::kReplaying)
      .value("STOPPING", EngineLifecycleState::kStopping)
      .value("STOPPED", EngineLifecycleState::kStopped)
      .value("FAILED", EngineLifecycleState::kFailed);

  py::enum_<OrderStatus>(module, "OrderStatus")
      .value("UNKNOWN", OrderStatus::kUnknown)
      .value("PENDING_NEW", OrderStatus::kPendingNew)
      .value("NEW", OrderStatus::kNew)
      .value("PARTIALLY_FILLED", OrderStatus::kPartiallyFilled)
      .value("FILLED", OrderStatus::kFilled)
      .value("CANCEL_PENDING", OrderStatus::kCancelPending)
      .value("CANCELLED", OrderStatus::kCancelled)
      .value("REJECTED", OrderStatus::kRejected);

  py::class_<EngineStatus>(module, "EngineStatus")
      .def(py::init<>())
      .def_readwrite("mode", &EngineStatus::mode)
      .def_readwrite("running", &EngineStatus::running)
      .def_readwrite("state", &EngineStatus::state)
      .def_readwrite("last_error", &EngineStatus::last_error);

  py::class_<PriceLevel>(module, "PriceLevel")
      .def(py::init<>())
      .def_readwrite("price", &PriceLevel::price)
      .def_readwrite("quantity", &PriceLevel::quantity);

  py::class_<Instrument>(module, "Instrument")
      .def(py::init<>())
      .def_readwrite("instrument_id", &Instrument::instrument_id)
      .def_readwrite("exchange", &Instrument::exchange)
      .def_readwrite("symbol", &Instrument::symbol)
      .def_readwrite("lot_size", &Instrument::lot_size)
      .def_readwrite("price_tick", &Instrument::price_tick)
      .def_readwrite("currency", &Instrument::currency);

  py::class_<Bar>(module, "Bar")
      .def(py::init<>())
      .def_readwrite("instrument_id", &Bar::instrument_id)
      .def_readwrite("ts_event", &Bar::ts_event)
      .def_readwrite("ts_process", &Bar::ts_process)
      .def_readwrite("open", &Bar::open)
      .def_readwrite("high", &Bar::high)
      .def_readwrite("low", &Bar::low)
      .def_readwrite("close", &Bar::close)
      .def_readwrite("volume", &Bar::volume)
      .def_readwrite("turnover", &Bar::turnover);

  py::class_<OrderIntent>(module, "OrderIntent")
      .def(py::init<>())
      .def_readwrite("instrument_id", &OrderIntent::instrument_id)
      .def_readwrite("side", &OrderIntent::side)
      .def_readwrite("offset", &OrderIntent::offset)
      .def_readwrite("order_type", &OrderIntent::order_type)
      .def_readwrite("quantity", &OrderIntent::quantity)
      .def_readwrite("price", &OrderIntent::price);

  py::class_<Tick>(module, "Tick")
      .def(py::init<>())
      .def_readwrite("instrument_id", &Tick::instrument_id)
      .def_readwrite("ts_event", &Tick::ts_event)
      .def_readwrite("ts_process", &Tick::ts_process)
      .def_readwrite("last_price", &Tick::last_price)
      .def_readwrite("last_quantity", &Tick::last_quantity)
      .def_readwrite("bids", &Tick::bids)
      .def_readwrite("asks", &Tick::asks);

  py::class_<Order>(module, "Order")
      .def(py::init<>())
      .def_readwrite("session_id", &Order::session_id)
      .def_readwrite("order_id", &Order::order_id)
      .def_readwrite("instrument_id", &Order::instrument_id)
      .def_readwrite("side", &Order::side)
      .def_readwrite("offset", &Order::offset)
      .def_readwrite("order_type", &Order::order_type)
      .def_readwrite("status", &Order::status)
      .def_readwrite("reject_reason", &Order::reject_reason)
      .def_readwrite("quantity", &Order::quantity)
      .def_readwrite("filled_quantity", &Order::filled_quantity)
      .def_readwrite("price", &Order::price)
      .def_readwrite("average_fill_price", &Order::average_fill_price)
      .def_readwrite("ts_event", &Order::ts_event)
      .def_readwrite("ts_process", &Order::ts_process);

  py::class_<Trade>(module, "Trade")
      .def(py::init<>())
      .def_readwrite("session_id", &Trade::session_id)
      .def_readwrite("order_id", &Trade::order_id)
      .def_readwrite("trade_id", &Trade::trade_id)
      .def_readwrite("instrument_id", &Trade::instrument_id)
      .def_readwrite("side", &Trade::side)
      .def_readwrite("offset", &Trade::offset)
      .def_readwrite("fill_quantity", &Trade::fill_quantity)
      .def_readwrite("fill_price", &Trade::fill_price)
      .def_readwrite("commission", &Trade::commission)
      .def_readwrite("ts_event", &Trade::ts_event)
      .def_readwrite("ts_process", &Trade::ts_process);

  py::class_<RiskEvent>(module, "RiskEvent")
      .def(py::init<>())
      .def_readwrite("order_id", &RiskEvent::order_id)
      .def_readwrite("instrument_id", &RiskEvent::instrument_id)
      .def_readwrite("passed", &RiskEvent::passed)
      .def_readwrite("reason", &RiskEvent::reason)
      .def_readwrite("detail", &RiskEvent::detail)
      .def_readwrite("ts_event", &RiskEvent::ts_event)
      .def_readwrite("ts_process", &RiskEvent::ts_process);

  py::class_<Position>(module, "Position")
      .def(py::init<>())
      .def_readwrite("instrument_id", &Position::instrument_id)
      .def_readwrite("quantity_total", &Position::quantity_total)
      .def_readwrite("quantity_available", &Position::quantity_available)
      .def_readwrite("average_price", &Position::average_price)
      .def_readwrite("market_value", &Position::market_value);

  py::class_<Account>(module, "Account")
      .def(py::init<>())
      .def_readwrite("account_id", &Account::account_id)
      .def_readwrite("currency", &Account::currency)
      .def_readwrite("cash", &Account::cash)
      .def_readwrite("frozen_cash", &Account::frozen_cash)
      .def_readwrite("equity", &Account::equity);

  py::class_<PortfolioSnapshot>(module, "PortfolioSnapshot")
      .def(py::init<>())
      .def_readwrite("snapshot_id", &PortfolioSnapshot::snapshot_id)
      .def_readwrite("account_id", &PortfolioSnapshot::account_id)
      .def_readwrite("account_state", &PortfolioSnapshot::account_state)
      .def_readwrite("positions", &PortfolioSnapshot::positions)
      .def_readwrite("ts_snapshot", &PortfolioSnapshot::ts_snapshot);

  py::class_<Engine>(module, "Engine")
      .def(py::init<RuntimeMode>(), py::arg("mode") = RuntimeMode::kBacktest)
      .def("start", &Engine::start)
      .def("stop", &Engine::stop)
      .def("status", &Engine::status)
      .def("name", &Engine::name)
      .def(
          "submit_intent",
          [](Engine &self, const OrderIntent &intent) {
            auto result = self.submit_intent(intent);
            if (!result.ok()) {
              throw std::runtime_error(result.error().message);
            }
            return result.value();
          },
          py::arg("intent"))
      .def(
          "cancel_order",
          [](Engine &self, const std::string &order_id) {
            auto result = self.cancel_order(order_id);
            if (!result.ok()) {
              throw std::runtime_error(result.error().message);
            }
            return result.value();
          },
          py::arg("order_id"))
      .def("get_order", &Engine::get_order, py::arg("order_id"))
      .def("get_portfolio_snapshot", &Engine::get_portfolio_snapshot)
      .def(
          "read_journal",
          [](Engine &self, std::uint64_t offset) {
            auto result = self.read_journal(offset);
            if (!result.ok()) {
              throw std::runtime_error(result.error().message);
            }
            return result.value();
          },
          py::arg("offset"))
      .def("journal_size", &Engine::journal_size)
      .def(
          "load_replay",
          [](Engine &self, const std::string &csv_path) {
            auto result = self.load_replay(csv_path);
            if (!result.ok()) {
              throw std::runtime_error(result.error().message);
            }
          },
          py::arg("csv_path"))
      .def(
          "run_backtest",
          [](Engine &self, py::function py_on_bar, py::object py_on_order,
             py::object py_on_trade) {
            auto on_bar = [fn = std::move(py_on_bar)](const Bar &bar) {
              fn(bar);
            };
            std::function<void(const Order &)> on_order;
            if (!py_on_order.is_none()) {
              auto order_fn = py_on_order.cast<py::function>();
              on_order = [fn = std::move(order_fn)](const Order &order) {
                fn(order);
              };
            }

            std::function<void(const Trade &)> on_trade;
            if (!py_on_trade.is_none()) {
              auto trade_fn = py_on_trade.cast<py::function>();
              on_trade = [fn = std::move(trade_fn)](const Trade &trade) {
                fn(trade);
              };
            }

            auto result = self.run_backtest(
                std::move(on_bar), std::move(on_order), std::move(on_trade));
            if (!result.ok()) {
              throw std::runtime_error(result.error().message);
            }
          },
          py::arg("py_on_bar"), py::arg("py_on_order") = py::none(),
          py::arg("py_on_trade") = py::none())
      .def("clock_now", &Engine::clock_now)
      .def("submit", [](Engine &self, core::EventEnvelope &env) {
        auto result = self.submit(std::move(env));
        if (!result.ok()) {
          throw std::runtime_error(result.error().message);
        }
        return result.value();
      });

  py::enum_<core::EventType>(module, "EventType")
      .value("UNKNOWN", core::EventType::kUnknown)
      .value("SYSTEM", core::EventType::kSystem)
      .value("CONTROL", core::EventType::kControl)
      .value("MARKET_TICK", core::EventType::kMarketTick)
      .value("MARKET_BAR", core::EventType::kMarketBar)
      .value("ORDER", core::EventType::kOrder)
      .value("TRADE", core::EventType::kTrade)
      .value("RISK", core::EventType::kRisk)
      .value("PORTFOLIO", core::EventType::kPortfolio);

  py::enum_<core::EventSource>(module, "EventSource")
      .value("UNKNOWN", core::EventSource::kUnknown)
      .value("LIVE", core::EventSource::kLive)
      .value("REPLAY", core::EventSource::kReplay)
      .value("TEST", core::EventSource::kTest)
      .value("MANUAL", core::EventSource::kManual);

  py::class_<core::EventEnvelope>(module, "EventEnvelope")
      .def(py::init<>())
      .def_readwrite("event_type", &core::EventEnvelope::event_type)
      .def_readwrite("source", &core::EventEnvelope::source)
      .def_readwrite("ts_event", &core::EventEnvelope::ts_event)
      .def_property(
          "payload",
          [](const core::EventEnvelope &self) -> py::object {
            return std::visit(
                [](auto &&arg) -> py::object {
                  using T = std::decay_t<decltype(arg)>;
                  if constexpr (std::is_same_v<T, std::monostate>)
                    return py::none();
                  else
                    return py::cast(arg);
                },
                self.payload);
          },
          [](core::EventEnvelope &self, py::object obj) {
            if (obj.is_none())
              self.payload = std::monostate{};
            else if (py::isinstance<Tick>(obj))
              self.payload = obj.cast<Tick>();
            else if (py::isinstance<Bar>(obj))
              self.payload = obj.cast<Bar>();
            else if (py::isinstance<Order>(obj))
              self.payload = obj.cast<Order>();
            else if (py::isinstance<Trade>(obj))
              self.payload = obj.cast<Trade>();
            else if (py::isinstance<RiskEvent>(obj))
              self.payload = obj.cast<RiskEvent>();
            else if (py::isinstance<PortfolioSnapshot>(obj))
              self.payload = obj.cast<PortfolioSnapshot>();
          });

  module.def("is_terminal", &is_terminal, py::arg("status"));
}

} // namespace dhquant
