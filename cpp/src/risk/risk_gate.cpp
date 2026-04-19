#include "dhquant/risk/risk_gate.h"

#include <cmath>

namespace dhquant::risk {

namespace {

bool matches_price_tick(const double price, const double tick) noexcept {
  if (tick <= 0.0) {
    return true;
  }
  const auto remainder = std::fmod(price, tick);
  return std::fabs(remainder) < 1e-9 || std::fabs(remainder - tick) < 1e-9;
}

} // namespace

void RiskGate::replace_instruments(
    std::unordered_map<std::string, Instrument> instruments) {
  instruments_ = std::move(instruments);
}

RiskResult RiskGate::check(const OrderIntent &intent,
                           const PortfolioSnapshot &portfolio) const noexcept {
  const auto instrument_result = check_instrument(intent);
  if (!instrument_result.passed) {
    return instrument_result;
  }

  const auto instrument_it = instruments_.find(intent.instrument_id);
  const auto &instrument = instrument_it->second;

  const auto quantity_result = check_quantity(intent, instrument);
  if (!quantity_result.passed) {
    return quantity_result;
  }

  const auto price_result = check_price(intent, instrument);
  if (!price_result.passed) {
    return price_result;
  }

  const auto funds_result = check_funds(intent, portfolio);
  if (!funds_result.passed) {
    return funds_result;
  }

  return {.passed = true};
}

RiskResult
RiskGate::check_instrument(const OrderIntent &intent) const noexcept {
  if (!instruments_.contains(intent.instrument_id)) {
    return {.passed = false,
            .reason = RejectReason::kInvalidInstrument,
            .detail = "unknown instrument: " + intent.instrument_id};
  }
  return {.passed = true};
}

RiskResult
RiskGate::check_quantity(const OrderIntent &intent,
                         const Instrument &instrument) const noexcept {
  if (intent.quantity <= 0) {
    return {.passed = false,
            .reason = RejectReason::kInvalidQuantity,
            .detail = "quantity must be positive"};
  }

  if (instrument.lot_size > 0 && intent.quantity % instrument.lot_size != 0) {
    return {.passed = false,
            .reason = RejectReason::kInvalidQuantity,
            .detail = "quantity must match lot size"};
  }

  return {.passed = true};
}

RiskResult RiskGate::check_price(const OrderIntent &intent,
                                 const Instrument &instrument) const noexcept {
  if (intent.order_type == OrderType::kMarket) {
    if (intent.price < 0.0) {
      return {.passed = false,
              .reason = RejectReason::kInvalidPrice,
              .detail = "market order price cannot be negative"};
    }
    return {.passed = true};
  }

  if (intent.price <= 0.0) {
    return {.passed = false,
            .reason = RejectReason::kInvalidPrice,
            .detail = "limit order price must be positive"};
  }

  if (!matches_price_tick(intent.price, instrument.price_tick)) {
    return {.passed = false,
            .reason = RejectReason::kInvalidPrice,
            .detail = "price does not match instrument tick"};
  }

  return {.passed = true};
}

RiskResult
RiskGate::check_funds(const OrderIntent &intent,
                      const PortfolioSnapshot &portfolio) const noexcept {
  if (intent.side != Side::kBuy || intent.price <= 0.0) {
    return {.passed = true};
  }

  const auto required = intent.price * static_cast<double>(intent.quantity) *
                        (1.0 + commission_rate_);
  if (portfolio.account_state.cash + 1e-9 < required) {
    return {.passed = false,
            .reason = RejectReason::kRiskRejected,
            .detail = "insufficient cash for order intent"};
  }

  return {.passed = true};
}

} // namespace dhquant::risk
