#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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

  py::class_<Order>(module, "Order")
      .def(py::init<>())
      .def_readwrite("session_id", &Order::session_id)
      .def_readwrite("order_id", &Order::order_id)
      .def_readwrite("instrument_id", &Order::instrument_id)
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
      .def_readwrite("fill_quantity", &Trade::fill_quantity)
      .def_readwrite("fill_price", &Trade::fill_price)
      .def_readwrite("commission", &Trade::commission)
      .def_readwrite("ts_event", &Trade::ts_event)
      .def_readwrite("ts_process", &Trade::ts_process);

  py::class_<Engine>(module, "Engine")
      .def(py::init<RuntimeMode>(), py::arg("mode") = RuntimeMode::kBacktest)
      .def("start", &Engine::start)
      .def("stop", &Engine::stop)
      .def("status", &Engine::status)
      .def("name", &Engine::name);

  module.def("is_terminal", &is_terminal, py::arg("status"));
}

} // namespace dhquant
