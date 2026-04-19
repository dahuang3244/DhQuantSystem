#pragma once

#include <cstdint>
#include <vector>

#include "dhquant/core/config.h"
#include "dhquant/core/event.h"
#include "utils/Result.hpp"

namespace dhquant::core {

// Journal 是事件事实账本。第一版建议你先做 InMemoryJournal 或最简文件版。
// 关键是先把 append/read/flush 这组语义固定住。
class IJournal {
public:
  virtual ~IJournal() = default;

  virtual Result<void> open(const JournalConfig &config) = 0;

  // 追加一个统一事件，返回可供 replay 定位的 offset/sequence 位置。
  virtual Result<std::uint64_t> append(const EventEnvelope &event) = 0;

  // 按 offset 读取，保证 replay 可以顺序恢复。
  virtual Result<EventEnvelope> read(std::uint64_t offset) const = 0;

  virtual Result<void> flush() = 0;
  virtual Result<void> close() = 0;
};

// 初级内存journal
class InMemoryJournal final : public IJournal {
public:
  Result<void> open(const JournalConfig &config) override;
  Result<std::uint64_t> append(const EventEnvelope &event) override;
  Result<EventEnvelope> read(std::uint64_t offset) const override;
  Result<void> flush() override;
  Result<void> close() override;

private:
  JournalConfig config_{};
  std::vector<EventEnvelope> events_;
  bool is_open_ = false;
};

} // namespace dhquant::core
