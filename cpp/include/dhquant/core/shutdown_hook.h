#pragma once

#include "dhquant/core/config.h"
#include "dhquant/core/event_loop.h"
#include "utils/Result.hpp"
#include <memory>

namespace dhquant::core {

// ShutdownHook 用于集中定义停机收尾动作，避免 stop 逻辑散落在
// Engine/EventLoop/Journal。
class IShutdownHook {
public:
  virtual ~IShutdownHook() = default;

  virtual Result<void> prepare_stop(StopMode mode) = 0;
  virtual Result<void> flush_and_join() = 0;
};

class DefaultShutdownHook final : public IShutdownHook {
public:
  explicit DefaultShutdownHook(std::shared_ptr<EventLoop> event_loop) noexcept;

  Result<void> prepare_stop(StopMode mode) override;
  Result<void> flush_and_join() override;

private:
  std::shared_ptr<EventLoop> event_loop_;
  StopMode pending_mode_{StopMode::QDrain};
};

} // namespace dhquant::core
