#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include "dhquant/core/handler.h"

namespace dhquant::core {

class BacktestMatcher : public IEventHandler {
public:
  BacktestMatcher() = default;

  Result<void> handle(const EventEnvelope &event) override;

  // 注入 EventLoop 的引用以便发送 Trade 事件
  void set_event_loop(class EventLoop *loop) { loop_ = loop; }

private:
  class EventLoop *loop_{nullptr};
  std::map<std::string, Bar> last_bars_;
  std::vector<Order> pending_orders_;

  bool should_queue_order(const Order &order) const;
  void sync_order(const Order &order);
  void match_orders(const Bar &bar);
};

} // namespace dhquant::core
