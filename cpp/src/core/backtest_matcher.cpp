#include "dhquant/core/backtest_matcher.h"
#include "dhquant/core/event_loop.h"

namespace dhquant::core {

Result<void> BacktestMatcher::handle(const EventEnvelope &event) {
  if (event.event_type == EventType::kMarketBar) {
    const auto &bar = std::get<Bar>(event.payload);
    last_bars_[bar.instrument_id] = bar;
    match_orders(bar);
  } else if (event.event_type == EventType::kOrder) {
    const auto &order = std::get<Order>(event.payload);
    sync_order(order);
  }
  return Result<void>::Ok();
}

bool BacktestMatcher::should_queue_order(const Order &order) const {
  return !is_terminal(order.status) && order.quantity > order.filled_quantity;
}

void BacktestMatcher::sync_order(const Order &order) {
  const auto it = std::find_if(
      pending_orders_.begin(), pending_orders_.end(),
      [&](const Order &pending) { return pending.order_id == order.order_id; });
  if (!should_queue_order(order)) {
    if (it != pending_orders_.end()) {
      pending_orders_.erase(it);
    }
    return;
  }

  if (it != pending_orders_.end()) {
    *it = order;
    return;
  }

  pending_orders_.push_back(order);
}

void BacktestMatcher::match_orders(const Bar &bar) {
  auto it = pending_orders_.begin();
  while (it != pending_orders_.end()) {
    if (it->instrument_id == bar.instrument_id) {
      const auto remaining_quantity = it->quantity - it->filled_quantity;
      if (remaining_quantity <= 0) {
        it = pending_orders_.erase(it);
        continue;
      }

      // 极简撮合：按当前 Bar 的收盘价把剩余部分全部成交。
      Trade trade;
      trade.session_id = it->session_id;
      trade.order_id = it->order_id;
      trade.trade_id = "T_" + it->order_id + "_" + std::to_string(bar.ts_event);
      trade.instrument_id = it->instrument_id;
      trade.side = it->side;
      trade.offset = it->offset;
      trade.fill_quantity = remaining_quantity;
      trade.fill_price = bar.close;
      trade.commission =
          bar.close * static_cast<double>(remaining_quantity) * 0.001;
      trade.ts_event = bar.ts_event;
      trade.ts_process = bar.ts_event; // 回测中处理时间通常等于事件时间

      if (loop_) {
        EventEnvelope trade_env;
        trade_env.event_type = EventType::kTrade;
        trade_env.source = EventSource::kReplay;
        trade_env.ts_event = bar.ts_event;
        trade_env.payload = trade;
        (void)loop_->post(trade_env);
      }

      it = pending_orders_.erase(it);
    } else {
      ++it;
    }
  }
}

} // namespace dhquant::core
