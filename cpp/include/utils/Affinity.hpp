#pragma once

#include <thread>

#ifdef __linux__
#include <pthread.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif

namespace dhquant::utils {

/**
 * @brief 线程亲和性设置 (Thread Affinity Binding)
 *
 * 在高性能量化交易中，将 EventLoop 线程绑定到特定的物理核上，
 * 可以极限降低 CPU 上下文切换的开销，提高缓存命中率。
 */
class Affinity {
public:
  /**
   * @brief 绑定当前线程到指定的 CPU 核
   *
   * @param core_id CPU 核心编号 (从 0 开始)
   * @return bool 是否绑定成功
   */
  static bool bind_to_core(int core_id) {
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) ==
           0;
#elif defined(__APPLE__)
    // macOS 不支持直接绑定 CPU ID，但支持“关联集” (Affinity Set)
    // thread_affinity_policy
    // 的序号相同时，系统会倾向于调度到同一个一级或二级缓存域
    thread_affinity_policy_data_t policy = {core_id};
    return thread_policy_set(mach_thread_self(), THREAD_AFFINITY_POLICY,
                             (thread_policy_t)&policy,
                             THREAD_AFFINITY_POLICY_COUNT) == KERN_SUCCESS;
#else
    return false;
#endif
  }
};

} // namespace dhquant::utils
