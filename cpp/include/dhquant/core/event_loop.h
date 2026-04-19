#pragma once

#include <cstdint>
#include <memory>
#include <queue>

#include "dhquant/core/clock.h"
#include "dhquant/core/config.h"
#include "dhquant/core/dispatcher.h"
#include "dhquant/core/event.h"
#include "dhquant/core/journal.h"
#include "utils/Result.hpp"

namespace dhquant::core {

// EventLoop 是 Phase 2 真正的执行中枢。
// 你后续实现时，建议先做“单线程 + FIFO 队列 + append before
// dispatch”的最小闭环。
class EventLoop {
public:
  EventLoop() = default;
  EventLoop(std::shared_ptr<IClock> clock, std::shared_ptr<IJournal> journal,
            std::shared_ptr<Dispatcher> dispatcher) noexcept;

  // 配置阶段通常发生在 Engine::start 前。
  Result<void> configure(const EventLoopConfig &config);

  // 初始化运行状态，不负责业务装配。
  Result<void> start();

  // 按停机策略进入 Drain/Abort 流程。
  Result<void> stop(StopMode mode);

  // 投递统一事件。这里后续要做的核心动作是：
  // 1. 分配 sequence_id
  // 2. 写 journal
  // 3. 推入待处理队列
  Result<std::uint64_t> post(EventEnvelope event);

  // 处理一个调度周期，便于做 deterministic tests。
  Result<void> poll_once();

  // 排空已入队但未消费的事件。
  Result<void> drain();

private:
  std::shared_ptr<IClock> clock_{};
  std::shared_ptr<IJournal> journal_{};
  std::shared_ptr<Dispatcher> dispatcher_{};
  EventLoopConfig config_{};
  std::uint64_t next_sequence_id_{1};
  bool running_{false};
  std::queue<EventEnvelope> queue_;
};

} // namespace dhquant::core
