#include "dhquant/core/python_bridge_handler.h"
#include <exception>
#include <iostream>

namespace dhquant::core {

Result<void> PythonBridgeHandler::handle(const EventEnvelope &event) {
  try {
    switch (event.event_type) {
    case EventType::kMarketBar:
      if (on_bar_) {
        on_bar_(std::get<Bar>(event.payload));
      }
      break;
    case EventType::kOrder:
      if (on_order_) {
        on_order_(std::get<Order>(event.payload));
      }
      break;
    case EventType::kTrade:
      if (on_trade_) {
        on_trade_(std::get<Trade>(event.payload));
      }
      break;
    default:
      break;
    }
  } catch (const std::exception &e) {
    std::cerr << "Python error in bridge: " << e.what() << std::endl;
    return Result<void>::Err(Error::Internal(e.what()));
  }
  return Result<void>::Ok();
}

} // namespace dhquant::core
