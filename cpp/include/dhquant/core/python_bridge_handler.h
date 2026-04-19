#pragma once

#include "dhquant/core/handler.h"
#include <functional>
#include <utility>

namespace dhquant::core {

class PythonBridgeHandler : public IEventHandler {
public:
  PythonBridgeHandler() = default;

  void set_on_bar(std::function<void(const Bar &)> cb) {
    on_bar_ = std::move(cb);
  }
  void set_on_order(std::function<void(const Order &)> cb) {
    on_order_ = std::move(cb);
  }
  void set_on_trade(std::function<void(const Trade &)> cb) {
    on_trade_ = std::move(cb);
  }

  Result<void> handle(const EventEnvelope &event) override;

private:
  std::function<void(const Bar &)> on_bar_;
  std::function<void(const Order &)> on_order_;
  std::function<void(const Trade &)> on_trade_;
};

} // namespace dhquant::core
