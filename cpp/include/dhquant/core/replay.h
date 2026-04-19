#pragma once

#include <cstdint>

#include "utils/Result.hpp"

namespace dhquant::core {

// ReplayRunner 负责把 Journal 中的历史事件重新送回运行链路。
// 这里先只固定外部接口，内部是否通过 EventLoop 还是 Dispatcher 回灌，后续再定。
class IReplayRunner {
public:
  virtual ~IReplayRunner() = default;

  virtual Result<void> replay_from_offset(std::uint64_t offset) = 0;

  virtual Result<void> replay_between(std::int64_t begin_ts,
                                      std::int64_t end_ts) = 0;
};

} // namespace dhquant::core
