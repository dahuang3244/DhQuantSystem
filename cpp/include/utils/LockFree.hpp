#pragma once

#include <atomic>
#include <cassert>
#include <optional>
#include <vector>

namespace dhquant::utils {

/**
 * @brief 单生产者单消费者 (SPSC) 无锁环形缓冲区
 *
 * 适用于 EventLoop 中具体的“点对点”快速消息传递 (例如: 网络线程 -> 策略线程)。
 * 核心原理是利用 std::atomic 的内存序 (Acquire-Release)
 * 保证生产者和消费者的缓存一致性。
 *
 * @tparam T 存储的对象类型
 */
template <typename T> class LockFreeQueue {
public:
  explicit LockFreeQueue(size_t capacity)
      : capacity_(capacity + 1), buffer_(capacity + 1) {
    // 实际容量 +1 是为了区分队空和队满
    assert(capacity > 0);
  }

  // 禁用拷贝和赋值
  LockFreeQueue(const LockFreeQueue &) = delete;
  LockFreeQueue &operator=(const LockFreeQueue &) = delete;

  /**
   * @brief 生产者端入队
   */
  bool push(T &&value) {
    const size_t current_head = head_.load(std::memory_order_relaxed);
    const size_t next_head = (current_head + 1) % capacity_;

    if (next_head == tail_.load(std::memory_order_acquire)) {
      return false; // 队列已满
    }

    buffer_[current_head] = std::move(value);
    head_.store(next_head, std::memory_order_release);
    return true;
  }

  /**
   * @brief 消费者端出队
   */
  std::optional<T> pop() {
    const size_t current_tail = tail_.load(std::memory_order_relaxed);
    if (current_tail == head_.load(std::memory_order_acquire)) {
      return std::nullopt; // 队列为空
    }

    T value = std::move(buffer_[current_tail]);
    tail_.store((current_tail + 1) % capacity_, std::memory_order_release);
    return value;
  }

  bool empty() const {
    return head_.load(std::memory_order_acquire) ==
           tail_.load(std::memory_order_acquire);
  }

private:
  const size_t capacity_;
  std::vector<T> buffer_;

  // 使用缓存行对齐 (C++17 alignas) 防止伪共享 (False Sharing)
  alignas(64) std::atomic<size_t> head_{0};
  alignas(64) std::atomic<size_t> tail_{0};
};

/**
 * @brief 超轻量级自旋锁 (SpinLock)
 *
 * 在 EventLoop 的极短临界区（例如：简单的标志位切换或计数）比 std::mutex
 * 更高效， 因为它避免了由于上下文切换导致的内核态陷入。
 */
class SpinLock {
public:
  void lock() noexcept {
    while (flag_.test_and_set(std::memory_order_acquire)) {
      // 采用 pause 指令提示 CPU 处于自旋状态，降低能耗并提供负载建议
#if defined(__x86_64__) || defined(_M_X64)
      __builtin_ia32_pause();
#elif defined(__arm64__) || defined(__aarch64__)
      asm volatile("yield" ::: "memory");
#endif
    }
  }

  bool try_lock() noexcept {
    return !flag_.test_and_set(std::memory_order_acquire);
  }

  void unlock() noexcept { flag_.clear(std::memory_order_release); }

private:
  std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

} // namespace dhquant::utils
