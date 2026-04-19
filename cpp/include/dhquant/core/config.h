#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

#include "dhquant/domain.h"

namespace dhquant::core {

// 停机策略直接影响 EventLoop 和 Journal 的收尾语义。
// QDrain: 停止接收新事件，但尽量排空队列并 flush journal。
// QAbort: 立即终止，允许丢弃尚未处理的事件，仅适合故障/测试路径。
enum class StopMode : std::uint8_t {
  QDrain = 1,
  QAbort = 2,
};

// Journal 的最小配置集合。
// 当前阶段只先把配置入口固定住，具体文件切分、命名和索引逻辑后续再填。
struct JournalConfig {
  std::string root_dir;
  std::size_t segment_max_bytes{0};
  bool flush_on_append{false};
};

// EventLoop 的运行参数。
// queue_capacity 后续可以决定内存队列上限，enable_replay 用于区分运行模式。
struct EventLoopConfig {
  std::size_t queue_capacity{0};
  bool enable_replay{false};
  JournalConfig journal_config{};
};

// Engine 的聚合配置。后续从 yaml/python 侧映射时，优先映射到这个结构。
struct EngineConfig {
  RuntimeMode mode{RuntimeMode::kBacktest};
  StopMode stop_mode{StopMode::QDrain};
  EventLoopConfig event_loop{};
};

} // namespace dhquant::core
