//
// Created by scros on 11/27/24.
//

#include "singleton_exception.hpp"

SingletonException::SingletonException(const char* message): m_message(message)
{
}

const char* SingletonException::what() const noexcept
{
    return m_message.c_str();
}
