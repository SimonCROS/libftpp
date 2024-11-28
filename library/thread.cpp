//
// Created by Simon Cros on 28/11/2024.
//

#include "thread.hpp"

Thread::Thread(const std::string& name, std::function<void()> functToExecute): m_name(name), m_func(std::move(functToExecute))
{
}

auto Thread::start() -> void
{
    m_thread = std::thread([](const std::string& name, const std::function<void()>& func)
    {
        threadSafeCout.setPrefix("[" + name + "] ");
        std::invoke(func);
    }, m_name, m_func);
}

auto Thread::stop() -> void
{
    if (m_thread.joinable())
        m_thread.join();
}
