#include "dhquant/core/shutdown_hook.h"
#include "utils/Log.hpp"

namespace dhquant::core {

DefaultShutdownHook::DefaultShutdownHook(
    std::shared_ptr<EventLoop> event_loop) noexcept
    : event_loop_(std::move(event_loop)) {}

Result<void> DefaultShutdownHook::prepare_stop(StopMode mode) {
  pending_mode_ = mode;
  LOGI("shutdown_hook", "prepare_stop called");
  return DH_OK();
}

Result<void> DefaultShutdownHook::flush_and_join() {
  if (!event_loop_) {
    return DH_ERR(ErrorCode::kStateError,
                  "DefaultShutdownHook: event_loop is null");
  }
  // 委托给 EventLoop 按策略完成停机
  return event_loop_->stop(pending_mode_);
}
} // namespace dhquant::core
