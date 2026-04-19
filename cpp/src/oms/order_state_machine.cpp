#include "dhquant/oms/order_state_machine.h"

namespace dhquant::oms {

TransitionResult
OrderStateMachine::try_transition(const OrderStatus from,
                                  const OrderStatus to) const noexcept {
  if (is_valid_transition(from, to)) {
    return {.ok = true};
  }
  return {.ok = false,
          .error = "invalid order status transition from " +
                   std::string(to_string(from)) + " to " +
                   std::string(to_string(to))};
}

bool OrderStateMachine::is_valid_transition(
    const OrderStatus from, const OrderStatus to) const noexcept {
  switch (from) {
  case OrderStatus::kPendingNew:
    return to == OrderStatus::kNew || to == OrderStatus::kRejected;
  case OrderStatus::kNew:
    return to == OrderStatus::kPartiallyFilled || to == OrderStatus::kFilled ||
           to == OrderStatus::kCancelPending;
  case OrderStatus::kPartiallyFilled:
    return to == OrderStatus::kPartiallyFilled || to == OrderStatus::kFilled ||
           to == OrderStatus::kCancelPending;
  case OrderStatus::kCancelPending:
    return to == OrderStatus::kCancelled || to == OrderStatus::kFilled;
  default:
    return false;
  }
}

} // namespace dhquant::oms
