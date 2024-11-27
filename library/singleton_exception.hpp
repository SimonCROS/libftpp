//
// Created by scros on 11/27/24.
//

#ifndef SINGLETON_EXCEPTION_HPP
#define SINGLETON_EXCEPTION_HPP
#include <string>
#include <exception>

class SingletonException final : public std::exception
{
    const std::string m_message;

public:
    explicit SingletonException(const char* message);

    [[nodiscard]] auto what() const noexcept -> const char* override;
};

#endif //SINGLETON_EXCEPTION_HPP
