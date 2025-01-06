//
// Created by Simon Cros on 12/5/24.
//

#include "worker_pool.hpp"
#include "thread_safe_iostream.hpp"

#include <version>

#if __cpp_lib_format >= 201907L
#include <format>
#endif

auto WorkerPool::worker(const int id) -> void
{
    std::string prefix;
#if __cpp_lib_format >= 201907L
    prefix = std::format("[Worker {}] ", id);
#else
    prefix = std::string("[Worker ") + std::to_string(id) + "] ";
#endif

    threadSafeCout.setPrefix(prefix);
    tscout().setPrefix(prefix);
    tscerr().setPrefix(prefix);

    while (!m_stop)
    {
        {
            // Scope for std::unique_lock
            std::unique_lock lock(m_workerMutex);
            m_conditionVariable.wait(lock, [this]
            {
                return m_stop || !m_jobs.empty();
            });

            if (m_stop)
                break;
        }

        try
        {
            std::invoke(m_jobs.pop_front());
        }
        catch (...) {}
    }
}

WorkerPool::WorkerPool(const int threadCount)
{
    m_threads.reserve(threadCount);
    for (int i = 0; i < threadCount; ++i)
        m_threads.emplace_back(&WorkerPool::worker, this, i + 1);
}

WorkerPool::~WorkerPool()
{
    m_stop = true;

    {
        std::scoped_lock lock(m_workerMutex);
        m_conditionVariable.notify_all();
    }

    for (auto& m_thread : m_threads)
    {
        if (m_thread.joinable())
            m_thread.join();
    }
}

auto WorkerPool::addJob(const std::function<void()>& jobToExecute) -> void
{
    m_jobs.push_back(jobToExecute);
    m_conditionVariable.notify_one();
}
