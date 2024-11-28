//
// Created by Simon Cros on 28/11/2024.
//

#include "thread.hpp"

Thread::Thread(const std::string& name, std::function<void()> functToExecute): m_name(name), m_func(std::move(functToExecute))
{
}
