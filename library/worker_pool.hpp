//
// Created by scros on 12/5/24.
//

#ifndef WORKER_POOL_HPP
#define WORKER_POOL_HPP
#include <condition_variable>
#include <functional>

#include "thread_safe_queue.hpp"

class WorkerPool
{
private:
    std::atomic<bool> m_stop{false};
    std::vector<std::thread> m_threads;

    ThreadSafeQueue<std::function<void()>> m_jobs;
    std::mutex m_workerMutex;
    std::condition_variable m_conditionVariable;

    auto worker(int id) -> void;

public:
    explicit WorkerPool(int threadCount);

    ~WorkerPool();

    auto addJob(const std::function<void()>& jobToExecute) -> void;
};

#endif //WORKER_POOL_HPP
