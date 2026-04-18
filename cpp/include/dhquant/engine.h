#pragma once

#include <string_view>

#include "dhquant/domain.h"

namespace dhquant {

struct EngineStatus {
  RuntimeMode mode{RuntimeMode::kUnknown};
  bool running{false};
};

class Engine {
public:
  explicit Engine(RuntimeMode mode = RuntimeMode::kBacktest) noexcept;

  void start() noexcept;
  void stop() noexcept;

  [[nodiscard]] EngineStatus status() const noexcept;
  [[nodiscard]] std::string_view name() const noexcept;

private:
  EngineStatus status_{};
};

} // namespace dhquant
