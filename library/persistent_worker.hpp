//
// Created by Simon Cros on 12/5/24.
//

#ifndef PERSISTENT_WORKER_HPP
#define PERSISTENT_WORKER_HPP
#include <version>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <thread>

class PersistentWorker
{
private:
    std::atomic_bool m_stop{false};
    std::thread m_thread;

    std::mutex m_dataMutex;
    std::unordered_map<std::string, std::function<void()>> m_jobs;
    std::mutex m_workerMutex;
    std::condition_variable m_conditionVariable;

    auto worker() -> void;

public:
    PersistentWorker();
    ~PersistentWorker();

    auto addTask(const std::string& name, const std::function<void()>& jobToExecute) -> void;
    auto removeTask(const std::string& name) -> void;
};

#endif //PERSISTENT_WORKER_HPP
