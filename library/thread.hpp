//
// Created by Simon Cros on 28/11/2024.
//

#ifndef THREAD_HPP
#define THREAD_HPP
#include <functional>
#include <thread>

class Thread
{
    std::string m_name;
    std::function<void()> m_func;
    std::thread m_thread = {};

public:
    Thread(const std::string& name, std::function<void()> functToExecute);

    auto start() -> void
    {
        m_thread = std::thread(m_func); // TODO wrap and set name
    }

    auto stop() -> void
    {
        if (m_thread.joinable())
            m_thread.join();
    }
};

#endif //THREAD_HPP
