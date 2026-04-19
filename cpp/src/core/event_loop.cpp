// cpp/src/core/event_loop.cpp
#include "dhquant/core/event_loop.h"
#include "utils/Log.hpp"

namespace dhquant::core {

EventLoop::EventLoop(std::shared_ptr<IClock> clock,
                     std::shared_ptr<IJournal> journal,
                     std::shared_ptr<Dispatcher> dispatcher) noexcept
    : clock_(std::move(clock)), journal_(std::move(journal)),
      dispatcher_(std::move(dispatcher)) {}

Result<void> EventLoop::configure(const EventLoopConfig &config) {
  if (running_) {
    return DH_ERR(ErrorCode::kStateError,
                  "EventLoop::configure: cannot configure while running");
  }
  if (config.queue_capacity > 0) {
    LOGW("event_loop", "queue_capacity is configured but not enforced by the "
                       "Phase 2 FIFO queue");
  }
  if (config.enable_replay) {
    LOGW("event_loop",
         "enable_replay is set, but replay wiring is not implemented yet");
  }
  config_ = config;
  return DH_OK();
}

Result<void> EventLoop::start() {
  if (running_) {
    return DH_ERR(ErrorCode::kStateError, "EventLoop::start: already running");
  }
  if (journal_) {
    auto r = journal_->open(config_.journal_config);
    if (!r.ok()) {
      LOGE("event_loop", "journal open failed: " + r.error().message);
      return r;
    }
  }
  running_ = true;
  LOGI("event_loop", "EventLoop started");
  return DH_OK();
}

Result<uint64_t> EventLoop::post(EventEnvelope event) {
  if (!running_) {
    return DH_ERR(ErrorCode::kStateError, "EventLoop::post: not running");
  }

  // ① 分配全局单调递增的 sequence_id
  event.sequence_id = next_sequence_id_++;

  // ② 填入引擎处理时间（通过时钟抽象，支持实盘/回测/测试三种模式）
  if (clock_) {
    event.ts_process = clock_->now();
  }

  // ③ 先写 Journal（append-before-dispatch 原则）
  if (journal_) {
    auto r = journal_->append(event);
    if (!r.ok()) {
      LOGE("event_loop",
           "journal append failed | seq=" + std::to_string(event.sequence_id) +
               " | " + r.error().message);
      return DH_ERR(r.error().code, r.error().message);
    }
    if (config_.journal_config.flush_on_append) {
      auto flush_r = journal_->flush();
      if (!flush_r.ok()) {
        LOGE("event_loop",
             "journal flush failed | seq=" + std::to_string(event.sequence_id) +
                 " | " + flush_r.error().message);
        return DH_ERR(flush_r.error().code, flush_r.error().message);
      }
    }
  }

  // ④ 入队，等待 poll_once 消费
  queue_.push(event);

  return DH_OK_V(event.sequence_id);
}

Result<void> EventLoop::poll_once() {
  if (queue_.empty()) {
    return DH_OK();
  }

  EventEnvelope event = queue_.front();
  queue_.pop();

  if (dispatcher_) {
    auto r = dispatcher_->dispatch(event);
    if (!r.ok()) {
      // dispatch 失败：记录日志但不停机
      LOGE("event_loop",
           "dispatch failed | seq=" + std::to_string(event.sequence_id) +
               " | " + r.error().message);
    }
  }

  return DH_OK();
}

Result<void> EventLoop::drain() {
  while (!queue_.empty()) {
    auto r = poll_once();
    if (!r.ok()) {
      LOGW("event_loop", "poll_once error during drain: " + r.error().message);
    }
  }
  return DH_OK();
}

Result<void> EventLoop::stop(StopMode mode) {
  if (!running_) {
    return DH_ERR(ErrorCode::kStateError, "EventLoop::stop: not running");
  }

  if (mode == StopMode::QDrain) {
    LOGI("event_loop", "stopping with QDrain, draining queue...");
    (void)drain();
    if (journal_)
      (void)journal_->flush();
  } else {
    LOGW("event_loop", "stopping with QAbort, discarding pending events");
    while (!queue_.empty())
      queue_.pop();
  }

  running_ = false;
  if (journal_)
    (void)journal_->close();
  LOGI("event_loop", "EventLoop stopped");
  return DH_OK();
}

} // namespace dhquant::core
