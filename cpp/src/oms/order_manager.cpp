#include "dhquant/oms/order_manager.h"

#include <algorithm>

namespace dhquant::oms {

void OrderManager::on_new_order(const Order &order) { store_order(order); }

Result<Order> OrderManager::transition_order(const std::string_view order_id,
                                             const OrderStatus next_status,
                                             const std::int64_t ts_event,
                                             const std::int64_t ts_process,
                                             const RejectReason reject_reason) {
  const auto it = active_.find(std::string(order_id));
  if (it == active_.end()) {
    return Error::NotFound("order not found: " + std::string(order_id));
  }

  auto transition =
      state_machine_.try_transition(it->second.status, next_status);
  if (!transition.ok) {
    return Error::Invalid(transition.error);
  }

  Order updated = it->second;
  updated.status = next_status;
  updated.ts_event = ts_event;
  updated.ts_process = ts_process;
  if (reject_reason != RejectReason::kNone) {
    updated.reject_reason = reject_reason;
  }

  store_order(updated);
  return updated;
}

Result<Order> OrderManager::on_trade(const Trade &trade,
                                     const std::int64_t ts_event,
                                     const std::int64_t ts_process) {
  const auto it = active_.find(trade.order_id);
  if (it == active_.end()) {
    return Error::NotFound("trade references unknown order: " + trade.order_id);
  }

  if (is_terminal(it->second.status)) {
    return Error::State("trade references terminal order: " + trade.order_id);
  }

  Order updated = it->second;
  const auto next_filled = updated.filled_quantity + trade.fill_quantity;
  if (next_filled > updated.quantity) {
    return Error::Invalid("trade fill exceeds order quantity: " +
                          trade.order_id);
  }

  const double prior_notional =
      updated.average_fill_price * static_cast<double>(updated.filled_quantity);
  const double trade_notional =
      trade.fill_price * static_cast<double>(trade.fill_quantity);
  updated.filled_quantity = next_filled;
  if (updated.filled_quantity > 0) {
    updated.average_fill_price = (prior_notional + trade_notional) /
                                 static_cast<double>(updated.filled_quantity);
  }

  const auto next_status = updated.filled_quantity == updated.quantity
                               ? OrderStatus::kFilled
                               : OrderStatus::kPartiallyFilled;
  auto transition = state_machine_.try_transition(updated.status, next_status);
  if (!transition.ok) {
    return Error::Invalid(transition.error);
  }

  updated.status = next_status;
  updated.ts_event = ts_event;
  updated.ts_process = ts_process;

  store_order(updated);
  return updated;
}

std::optional<Order>
OrderManager::find_order(const std::string_view order_id) const noexcept {
  if (const auto it = active_.find(std::string(order_id));
      it != active_.end()) {
    return it->second;
  }

  const auto archived_it =
      std::find_if(archived_.begin(), archived_.end(), [&](const Order &order) {
        return order.order_id == order_id;
      });
  if (archived_it == archived_.end()) {
    return std::nullopt;
  }
  return *archived_it;
}

std::vector<Order> OrderManager::active_orders() const {
  std::vector<Order> orders;
  orders.reserve(active_.size());
  for (const auto &[_, order] : active_) {
    orders.push_back(order);
  }
  return orders;
}

std::vector<Order> OrderManager::archived_orders() const { return archived_; }

void OrderManager::store_order(const Order &order) {
  if (is_terminal(order.status)) {
    active_.erase(order.order_id);
    const auto archived_it = std::find_if(
        archived_.begin(), archived_.end(), [&](const Order &candidate) {
          return candidate.order_id == order.order_id;
        });
    if (archived_it == archived_.end()) {
      archived_.push_back(order);
    } else {
      *archived_it = order;
    }
    return;
  }

  active_[order.order_id] = order;
}

} // namespace dhquant::oms
