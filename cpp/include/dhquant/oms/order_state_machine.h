#pragma once

#include <string>

#include "dhquant/domain.h"

namespace dhquant::oms {

struct TransitionResult {
  bool ok{false};
  std::string error;
};

class OrderStateMachine {
public:
  [[nodiscard]] TransitionResult try_transition(OrderStatus from,
                                                OrderStatus to) const noexcept;

  [[nodiscard]] bool is_valid_transition(OrderStatus from,
                                         OrderStatus to) const noexcept;
};

} // namespace dhquant::oms
