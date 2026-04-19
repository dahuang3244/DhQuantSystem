#pragma once

#include <string>
#include <unordered_map>

#include "dhquant/domain.h"

namespace dhquant::risk {

struct RiskResult {
  bool passed{false};
  RejectReason reason{RejectReason::kNone};
  std::string detail;
};

class RiskGate {
public:
  void
  replace_instruments(std::unordered_map<std::string, Instrument> instruments);

  [[nodiscard]] RiskResult
  check(const OrderIntent &intent,
        const PortfolioSnapshot &portfolio) const noexcept;

private:
  [[nodiscard]] RiskResult
  check_instrument(const OrderIntent &intent) const noexcept;
  [[nodiscard]] RiskResult
  check_quantity(const OrderIntent &intent,
                 const Instrument &instrument) const noexcept;
  [[nodiscard]] RiskResult
  check_price(const OrderIntent &intent,
              const Instrument &instrument) const noexcept;
  [[nodiscard]] RiskResult
  check_funds(const OrderIntent &intent,
              const PortfolioSnapshot &portfolio) const noexcept;

  std::unordered_map<std::string, Instrument> instruments_{};
  double commission_rate_{0.001};
};

} // namespace dhquant::risk
