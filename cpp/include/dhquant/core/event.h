#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "dhquant/domain.h"

namespace dhquant::core {

// 统一事件类型。它定义的是“调度语义”，不是业务状态。
// 后续 Dispatcher 主要基于这个字段做路由，而不是直接靠 variant 类型判断。
enum class EventType : std::uint8_t {
  kUnknown = 0,
  kSystem = 1,
  kControl = 2,
  kMarketTick = 3,
  kMarketBar = 4,
  kOrder = 5,
  kTrade = 6,
  kRisk = 7,
  kPortfolio = 8,
};

// 事件来源用于区分 live/replay/test/manual，便于日志和回放链路追踪。
enum class EventSource : std::uint8_t {
  kUnknown = 0,
  kLive = 1,
  kReplay = 2,
  kTest = 3,
  kManual = 4,
};

// 当前阶段只先覆盖 Phase 1 已有的领域对象，后续再扩成控制事件或系统事件变体。
using EventPayload =
    std::variant<std::monostate, Tick, Bar, Order, Trade, PortfolioSnapshot>;

// 统一事件信封，是 Core Engine 内部真正流动的对象。
// 你后续实现时可以把它理解成“进入 EventLoop 之前必须被标准化”的数据结构。
struct EventEnvelope {
  std::uint64_t sequence_id{0};
  EventType event_type{EventType::kUnknown};
  EventSource source{EventSource::kUnknown};
  std::string session_id;
  std::string trace_id;
  std::int64_t ts_event{0};
  std::int64_t ts_process{0};
  EventPayload payload{};
};

} // namespace dhquant::core
