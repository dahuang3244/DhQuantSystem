#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "dhquant/domain.h"

namespace dhquant::portfolio {

class Ledger {
public:
  explicit Ledger(double initial_cash = 1'000'000.0,
                  double commission_rate = 0.001) noexcept;

  void on_order_new(const Order &order);
  void on_trade(const Trade &trade, const Order &updated_order);
  void on_order_cancelled(const Order &order);

  [[nodiscard]] PortfolioSnapshot snapshot() const;
  [[nodiscard]] std::optional<Position>
  get_position(std::string_view instrument_id) const noexcept;

  [[nodiscard]] double cash() const noexcept;
  [[nodiscard]] double frozen_cash() const noexcept;
  [[nodiscard]] double equity() const noexcept;

private:
  [[nodiscard]] double reserve_for_order(const Order &order) const noexcept;
  [[nodiscard]] double release_frozen(std::string_view order_id,
                                      double requested) noexcept;
  void release_all_frozen(std::string_view order_id) noexcept;
  void recompute_equity() noexcept;

  Account account_{};
  std::unordered_map<std::string, Position> positions_{};
  std::unordered_map<std::string, double> frozen_by_order_{};
  double commission_rate_{0.001};
  mutable std::uint64_t snapshot_seq_{0};
};

} // namespace dhquant::portfolio
