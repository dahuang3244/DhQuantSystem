#pragma once

#include <unordered_map>

#include "dhquant/core/event.h"
#include "dhquant/core/handler.h"
#include "utils/Result.hpp"

namespace dhquant::core {

// Dispatcher 只做路由，不做业务处理。
// 后续你实现时，最简单的版本就是一个 map<EventType, handler>。
class Dispatcher {
public:
  Dispatcher() = default;

  // 注册某类事件的处理器。若后续支持多 handler，可把 value 改成 vector。
  Result<void> register_handler(EventType type, EventHandlerPtr handler);

  // 卸载处理器，便于测试或运行时切换模块。
  Result<void> unregister_handler(EventType type);

  // 对外暴露的唯一分发入口。EventLoop 只需要调用这个方法。
  Result<void> dispatch(const EventEnvelope &event) const;

private:
  std::unordered_map<EventType, EventHandlerPtr> handlers_{};
};

} // namespace dhquant::core
