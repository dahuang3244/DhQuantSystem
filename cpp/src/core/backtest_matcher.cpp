#include "dhquant/core/backtest_matcher.h"
#include "dhquant/core/event_loop.h"
#include <iostream>

namespace dhquant::core {

Result<void> BacktestMatcher::handle(const EventEnvelope &event) {
  if (event.event_type == EventType::kMarketBar) {
    const auto &bar = std::get<Bar>(event.payload);
    last_bars_[bar.instrument_id] = bar;
    match_orders(bar);
  } else if (event.event_type == EventType::kOrder) {
    const auto &order = std::get<Order>(event.payload);
    if (should_queue_order(order)) {
      pending_orders_.push_back(order);
    }
  }
  return Result<void>::Ok();
}

bool BacktestMatcher::should_queue_order(const Order &order) const {
  return !is_terminal(order.status) && order.quantity > order.filled_quantity;
}

void BacktestMatcher::match_orders(const Bar &bar) {
  auto it = pending_orders_.begin();
  while (it != pending_orders_.end()) {
    if (it->instrument_id == bar.instrument_id) {
      // 极简撮合：按当前 Bar 的收盘价全部成交
      Trade trade;
      trade.session_id = it->session_id;
      trade.order_id = it->order_id;
      trade.trade_id = "T_" + it->order_id + "_" + std::to_string(bar.ts_event);
      trade.instrument_id = it->instrument_id;
      trade.side = it->side;
      trade.offset = it->offset;
      trade.fill_quantity = it->quantity;
      trade.fill_price = bar.close;
      trade.ts_event = bar.ts_event;
      trade.ts_process = bar.ts_event; // 回测中处理时间通常等于事件时间

      // 修改订单状态为已成
      it->status = OrderStatus::kFilled;
      it->filled_quantity = it->quantity;
      it->average_fill_price = bar.close;

      // 投递 Trade 事件
      if (loop_) {
        EventEnvelope trade_env;
        trade_env.event_type = EventType::kTrade;
        trade_env.source = EventSource::kReplay;
        trade_env.ts_event = bar.ts_event;
        trade_env.payload = trade;
        auto res_t = loop_->post(trade_env);

        // 同时投递一个订单状态更新
        EventEnvelope order_env;
        order_env.event_type = EventType::kOrder;
        order_env.source = EventSource::kReplay;
        order_env.ts_event = bar.ts_event;
        order_env.payload = *it;
        auto res_o = loop_->post(order_env);

        (void)res_t;
        (void)res_o;
      }

      it = pending_orders_.erase(it);
    } else {
      ++it;
    }
  }
}

} // namespace dhquant::core
