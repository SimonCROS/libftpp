//
// Created by scros on 12/5/24.
//

#include "persistent_worker.hpp"

#include <ranges>

auto PersistentWorker::worker() -> void
{
    std::vector<std::function<void()>> cachedJobs;
    while (!m_stop)
    {
        // Don't try to iterate on an empty set
        {
            // Scope for std::unique_lock
            std::unique_lock lock{m_workerMutex};
            m_conditionVariable.wait(lock, [this]
            {
                std::scoped_lock s_lock{m_dataMutex};
                return m_stop || !m_jobs.empty();
            });

            if (m_stop)
                break;
        }

        // Copy jobs to cachedJobs, to avoid blocking insertion of a new job from the main thread
        {
            // Scope for std::scoped_lock
            std::scoped_lock s_lock{m_dataMutex};

            auto jobs = std::views::values(m_jobs);
#if __cpp_lib_containers_ranges >= 202202L
            cachedJobs.assign_range(jobs);
#else
            cachedJobs.assign(jobs.begin(), jobs.end());
#endif
        }

        for (auto& job : cachedJobs)
        {
            std::invoke(job);
        }
    }
}

PersistentWorker::PersistentWorker()
{
    m_thread = std::thread(&PersistentWorker::worker, this);
}

PersistentWorker::~PersistentWorker()
{
    m_stop = true;

    {
        std::scoped_lock lock{m_workerMutex};
        m_conditionVariable.notify_all();
    }

    if (m_thread.joinable())
        m_thread.join();
}

auto PersistentWorker::addTask(const std::string& name, const std::function<void()>& jobToExecute) -> void
#if __cpp_lib_associative_heterogeneous_insertion >= 202306L
{
    addTask(std::string_view(name), jobToExecute);
}

auto PersistentWorker::addTask(const std::string_view& name, const std::function<void()>& jobToExecute) -> void
#endif
{
    std::scoped_lock lock{m_dataMutex};
    // ignore, the prototype require a void return, and I don't want to throw for this
    std::ignore = m_jobs.try_emplace(name, jobToExecute);
    m_conditionVariable.notify_all();
}

auto PersistentWorker::removeTask(const std::string& name) -> void
#if __cpp_lib_associative_heterogeneous_erasure >= 202110L
{
    removeTask(std::string_view(name));
}

auto PersistentWorker::removeTask(const std::string_view& name) -> void
#endif
{
    std::scoped_lock lock{m_dataMutex};
    // ignore, the prototype require a void return, and I don't want to throw for this
    std::ignore = m_jobs.erase(name);
}
