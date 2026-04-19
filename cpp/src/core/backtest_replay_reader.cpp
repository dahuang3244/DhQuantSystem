#include "dhquant/core/backtest_replay_reader.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace dhquant::core {

BacktestReplayReader::BacktestReplayReader(const std::string &csv_path)
    : csv_path_(csv_path) {}

Result<void> BacktestReplayReader::load() {
  std::ifstream file(csv_path_);
  if (!file.is_open()) {
    return Result<void>::Err(DH_ERR(ErrorCode::kInvalidConfig,
                                    "Failed to open CSV file: " + csv_path_));
  }

  std::string line;
  // Skip header
  if (!std::getline(file, line)) {
    return Result<void>::Err(
        DH_ERR(ErrorCode::kInvalidConfig, "Empty CSV file: " + csv_path_));
  }

  bars_.clear();
  std::size_t line_no = 1;
  while (std::getline(file, line)) {
    ++line_no;
    if (line.empty())
      continue;

    std::stringstream ss(line);
    std::string item;
    Bar bar;
    int field_count = 0;

    // ts_event,instrument_id,open,high,low,close,volume,turnover
    try {
      if (std::getline(ss, item, ',')) {
        bar.ts_event = std::stoll(item);
        ++field_count;
      }
      if (std::getline(ss, item, ',')) {
        bar.instrument_id = item;
        ++field_count;
      }
      if (std::getline(ss, item, ',')) {
        bar.open = std::stod(item);
        ++field_count;
      }
      if (std::getline(ss, item, ',')) {
        bar.high = std::stod(item);
        ++field_count;
      }
      if (std::getline(ss, item, ',')) {
        bar.low = std::stod(item);
        ++field_count;
      }
      if (std::getline(ss, item, ',')) {
        bar.close = std::stod(item);
        ++field_count;
      }
      if (std::getline(ss, item, ',')) {
        bar.volume = std::stoll(item);
        ++field_count;
      }
      if (std::getline(ss, item, ',')) {
        bar.turnover = std::stod(item);
        ++field_count;
      }

      if (field_count != 8 || bar.instrument_id.empty()) {
        return Result<void>::Err(DH_ERR(
            ErrorCode::kInvalidConfig,
            "Malformed CSV line " + std::to_string(line_no) + ": " + line));
      }

      bars_.push_back(bar);
    } catch (const std::exception &e) {
      std::cerr << "Error parsing CSV line " << line_no << ": " << line
                << ", error: " << e.what() << std::endl;
      return Result<void>::Err(
          DH_ERR(ErrorCode::kInvalidConfig, "Failed to parse CSV line " +
                                                std::to_string(line_no) + ": " +
                                                e.what()));
    }
  }

  return Result<void>::Ok();
}

} // namespace dhquant::core
