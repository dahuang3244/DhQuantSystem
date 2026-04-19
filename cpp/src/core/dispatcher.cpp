#include "dhquant/core/dispatcher.h"

namespace dhquant {

Result<void> core::Dispatcher::register_handler(EventType type,
                                                EventHandlerPtr handler) {
  if (!handler) {
    return DH_ERR(ErrorCode::kInvalidArgument, "Handler cannot be null");
  }
  handlers_[type] = std::move(handler);
  return DH_OK();
}

Result<void> core::Dispatcher::unregister_handler(EventType type) {
  auto it = handlers_.find(type);
  if (it == handlers_.end()) {
    return DH_ERR(ErrorCode::kNotFound,
                  "No handler registered for this event type");
  }
  handlers_.erase(it);
  return DH_OK();
}

Result<void> core::Dispatcher::dispatch(const EventEnvelope &event) const {
  auto it = handlers_.find(event.event_type);
  if (it == handlers_.end()) {
    return DH_ERR(
        ErrorCode::kNotFound,
        "No handler registered for event type: " +
            std::to_string(static_cast<std::uint8_t>(event.event_type)));
  }
  return it->second->handle(event);
}

} // namespace dhquant
