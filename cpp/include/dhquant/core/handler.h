#pragma once

#include <memory>

#include "dhquant/core/event.h"
#include "utils/Result.hpp"

namespace dhquant::core {

// Handler 是业务扩展点。Core Engine 不直接写 OMS/Risk/Backtest 逻辑，
// 而是通过 Dispatcher 把统一事件交给具体 handler。
class IEventHandler {
public:
  virtual ~IEventHandler() = default;

  virtual Result<void> handle(const EventEnvelope &event) = 0;
};

using EventHandlerPtr = std::shared_ptr<IEventHandler>;

} // namespace dhquant::core
