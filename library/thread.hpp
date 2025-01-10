//
// Created by Simon Cros on 28/11/2024.
//

#ifndef THREAD_HPP
#define THREAD_HPP
#include <functional>
#include <thread>

#include "thread_safe_iostream.hpp"

class Thread
{
    std::string m_name;
    std::function<void()> m_func;
    std::thread m_thread = {};

public:
    Thread(const std::string_view& name, std::function<void()> functToExecute);

    auto start() -> void;

    auto stop() -> void;
};

#endif //THREAD_HPP
