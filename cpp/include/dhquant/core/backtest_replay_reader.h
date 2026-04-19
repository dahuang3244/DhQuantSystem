#pragma once

#include "dhquant/domain.h"
#include "utils/Result.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace dhquant::core {

class BacktestReplayReader {
public:
  explicit BacktestReplayReader(const std::string &csv_path);

  Result<void> load();
  const std::vector<Bar> &get_bars() const { return bars_; }

private:
  std::string csv_path_;
  std::vector<Bar> bars_;
};

} // namespace dhquant::core
