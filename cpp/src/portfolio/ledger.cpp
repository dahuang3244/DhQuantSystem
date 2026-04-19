#include "dhquant/portfolio/ledger.h"

#include <algorithm>

namespace dhquant::portfolio {

Ledger::Ledger(const double initial_cash, const double commission_rate) noexcept
    : commission_rate_(commission_rate) {
  account_.account_id = "default";
  account_.currency = "CNY";
  account_.cash = initial_cash;
  account_.frozen_cash = 0.0;
  account_.equity = initial_cash;
}

void Ledger::on_order_new(const Order &order) {
  const auto reserve = reserve_for_order(order);
  if (reserve <= 0.0) {
    return;
  }

  account_.cash -= reserve;
  account_.frozen_cash += reserve;
  frozen_by_order_[order.order_id] += reserve;
  recompute_equity();
}

void Ledger::on_trade(const Trade &trade, const Order &updated_order) {
  auto &position = positions_[trade.instrument_id];
  position.instrument_id = trade.instrument_id;

  if (trade.side == Side::kBuy) {
    const double actual_cost =
        trade.fill_price * static_cast<double>(trade.fill_quantity) +
        trade.commission;
    const double covered = release_frozen(trade.order_id, actual_cost);
    if (actual_cost > covered) {
      account_.cash -= (actual_cost - covered);
    }

    const auto prior_quantity = position.quantity_total;
    const double prior_cost =
        position.average_price * static_cast<double>(prior_quantity);
    position.quantity_total += trade.fill_quantity;
    position.quantity_available += trade.fill_quantity;
    if (position.quantity_total > 0) {
      position.average_price =
          (prior_cost +
           trade.fill_price * static_cast<double>(trade.fill_quantity)) /
          static_cast<double>(position.quantity_total);
    }
  } else if (trade.side == Side::kSell) {
    account_.cash +=
        trade.fill_price * static_cast<double>(trade.fill_quantity) -
        trade.commission;
    position.quantity_total -= trade.fill_quantity;
    position.quantity_available -= trade.fill_quantity;
    if (position.quantity_total <= 0) {
      position.quantity_total = 0;
      position.quantity_available = 0;
      position.average_price = 0.0;
    } else if (position.quantity_available < 0) {
      position.quantity_available = 0;
    }
  }

  position.market_value =
      position.average_price * static_cast<double>(position.quantity_total);

  if (updated_order.status == OrderStatus::kFilled ||
      updated_order.status == OrderStatus::kCancelled ||
      updated_order.status == OrderStatus::kRejected) {
    release_all_frozen(updated_order.order_id);
  }

  recompute_equity();
}

void Ledger::on_order_cancelled(const Order &order) {
  release_all_frozen(order.order_id);
  recompute_equity();
}

PortfolioSnapshot Ledger::snapshot() const {
  PortfolioSnapshot snapshot;
  snapshot.snapshot_id = "snapshot-" + std::to_string(++snapshot_seq_);
  snapshot.account_id = account_.account_id;
  snapshot.account_state = account_;
  snapshot.ts_snapshot = static_cast<std::int64_t>(snapshot_seq_);

  snapshot.positions.reserve(positions_.size());
  for (const auto &[_, position] : positions_) {
    snapshot.positions.push_back(position);
  }
  std::sort(snapshot.positions.begin(), snapshot.positions.end(),
            [](const Position &lhs, const Position &rhs) {
              return lhs.instrument_id < rhs.instrument_id;
            });
  return snapshot;
}

std::optional<Position>
Ledger::get_position(const std::string_view instrument_id) const noexcept {
  const auto it = positions_.find(std::string(instrument_id));
  if (it == positions_.end()) {
    return std::nullopt;
  }
  return it->second;
}

double Ledger::cash() const noexcept { return account_.cash; }

double Ledger::frozen_cash() const noexcept { return account_.frozen_cash; }

double Ledger::equity() const noexcept { return account_.equity; }

double Ledger::reserve_for_order(const Order &order) const noexcept {
  if (order.side != Side::kBuy || order.price <= 0.0 || order.quantity <= 0) {
    return 0.0;
  }

  return order.price * static_cast<double>(order.quantity) *
         (1.0 + commission_rate_);
}

double Ledger::release_frozen(const std::string_view order_id,
                              const double requested) noexcept {
  if (requested <= 0.0) {
    return 0.0;
  }

  const auto it = frozen_by_order_.find(std::string(order_id));
  if (it == frozen_by_order_.end() || it->second <= 0.0) {
    return 0.0;
  }

  const auto released = std::min(it->second, requested);
  it->second -= released;
  account_.frozen_cash -= released;
  if (it->second <= 0.0) {
    frozen_by_order_.erase(it);
  }
  return released;
}

void Ledger::release_all_frozen(const std::string_view order_id) noexcept {
  const auto it = frozen_by_order_.find(std::string(order_id));
  if (it == frozen_by_order_.end() || it->second <= 0.0) {
    return;
  }

  account_.cash += it->second;
  account_.frozen_cash -= it->second;
  frozen_by_order_.erase(it);
}

void Ledger::recompute_equity() noexcept {
  double market_value = 0.0;
  for (const auto &[_, position] : positions_) {
    market_value += position.market_value;
  }
  account_.equity = account_.cash + account_.frozen_cash + market_value;
}

} // namespace dhquant::portfolio
