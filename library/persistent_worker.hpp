//
// Created by scros on 12/5/24.
//

#ifndef PERSISTENT_WORKER_HPP
#define PERSISTENT_WORKER_HPP
#include <condition_variable>
#include <functional>
#include <unordered_map>
#if __cpp_lib_associative_heterogeneous_insertion >= 202306L || __cpp_lib_associative_heterogeneous_erasure >= 202110L
#include <string_view> // used only when addTask/removeTask feature check
#endif

class PersistentWorker
{
private:
    std::atomic<bool> m_stop{false};
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
#if __cpp_lib_associative_heterogeneous_insertion >= 202306L
    auto addTask(const std::string_view& name, const std::function<void()>& jobToExecute) -> void;
#endif
    auto removeTask(const std::string& name) -> void;
#if __cpp_lib_associative_heterogeneous_erasure >= 202110L
    auto removeTask(const std::string_view& name) -> void;
#endif
};

#endif //PERSISTENT_WORKER_HPP
