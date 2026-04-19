#pragma once

#include <memory>

#include "dhquant/core/clock.h"
#include "dhquant/core/config.h"
#include "dhquant/core/dispatcher.h"
#include "dhquant/core/event_loop.h"
#include "dhquant/core/journal.h"
#include "dhquant/core/replay.h"
#include "dhquant/core/shutdown_hook.h"

namespace dhquant::core {

// RuntimeContext 是 Engine 持有的一组运行时依赖。
// 文档里提到的 RuntimeContext
// 这里先落成一个简单聚合结构，后续实现阶段再补装配逻辑。
struct RuntimeContext {
  std::shared_ptr<IClock> clock;
  std::shared_ptr<IJournal> journal;
  std::shared_ptr<Dispatcher> dispatcher;
  std::shared_ptr<IReplayRunner> replay_runner;
  std::shared_ptr<IShutdownHook> shutdown_hook;
  std::shared_ptr<EventLoop> event_loop;
  EngineConfig config{};
};

} // namespace dhquant::core
