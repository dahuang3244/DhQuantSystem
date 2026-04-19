#include "dhquant/core/journal.h"
#include "utils/Log.hpp"

namespace dhquant::core {

Result<void> InMemoryJournal::open(const JournalConfig &config) {
  if (is_open_) {
    return DH_ERR(ErrorCode::kStateError, "Journal is already open");
  }
  config_ = config;
  events_.clear();
  is_open_ = true;
  if (!config_.root_dir.empty() &&
      config_.root_dir.rfind("memory://", 0) != 0) {
    LOGW("journal",
         "InMemoryJournal ignores non-memory root_dir: " + config_.root_dir);
  }
  if (config_.segment_max_bytes > 0) {
    LOGW("journal", "InMemoryJournal ignores segment_max_bytes");
  }
  return DH_OK();
}

Result<std::uint64_t> InMemoryJournal::append(const EventEnvelope &event) {
  if (!is_open_) {
    return DH_ERR(ErrorCode::kStateError, "Journal is not open");
  }
  events_.push_back(event);
  return DH_OK_V(static_cast<std::uint64_t>(events_.size() - 1));
}

Result<EventEnvelope> InMemoryJournal::read(std::uint64_t offset) const {
  if (!is_open_) {
    return DH_ERR(ErrorCode::kStateError, "Journal is not open");
  }
  if (offset >= events_.size()) {
    return DH_ERR(ErrorCode::kNotFound, "Offset out of bounds");
  }
  return Result<EventEnvelope>::Ok(events_[offset]);
}

Result<void> InMemoryJournal::flush() {
  // 内存版本已经在内存里，无需flash到操作系统或磁盘
  if (!is_open_) {
    return DH_ERR(ErrorCode::kStateError, "Journal is not open");
  }
  // In-memory journal doesn't need to flush, but we can simulate it.
  return DH_OK();
}

Result<void> InMemoryJournal::close() {
  if (!is_open_) {
    return DH_ERR(ErrorCode::kStateError, "Journal is not open");
  }
  events_.clear();
  is_open_ = false;
  return DH_OK();
}
} // namespace dhquant::core
