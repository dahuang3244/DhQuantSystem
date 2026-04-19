#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "dhquant/domain.h"
#include "dhquant/oms/order_state_machine.h"
#include "utils/Result.hpp"

namespace dhquant::oms {

class OrderManager {
public:
  void on_new_order(const Order &order);

  [[nodiscard]] Result<Order>
  transition_order(std::string_view order_id, OrderStatus next_status,
                   std::int64_t ts_event, std::int64_t ts_process,
                   RejectReason reject_reason = RejectReason::kNone);

  [[nodiscard]] Result<Order>
  on_trade(const Trade &trade, std::int64_t ts_event, std::int64_t ts_process);

  [[nodiscard]] std::optional<Order>
  find_order(std::string_view order_id) const noexcept;
  [[nodiscard]] std::vector<Order> active_orders() const;
  [[nodiscard]] std::vector<Order> archived_orders() const;

private:
  void store_order(const Order &order);

  std::unordered_map<std::string, Order> active_;
  std::vector<Order> archived_;
  OrderStateMachine state_machine_{};
};

} // namespace dhquant::oms
