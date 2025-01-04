//
// Created by Simon Cros on 1/4/25.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <version>

// std::format_string was standardized on __cpp_lib_format >= 202207L.
// However, this class is present in implementations where P2508R1 has been adopted, but P2419R2 has not yet been implemented.
#if __cpp_lib_format >= 202207L || (__cpp_lib_format >= 201907L && defined(LIBFTPP_ENABLE_P2508R1))

#define LIBFTPP_LOGGER

#include <format>
#include <string>
#include <iostream>

#include "thread_safe_iostream.hpp"

enum LogLevel
{
    Debug,
    Info,
    Warning,
    Error,
};

class Logger
{
private:
    static auto raw_log(ThreadSafeIOStream& stream, const std::string_view& prefix,
                        const std::string_view& message) -> void
    {
        threadSafeCout << prefix << message << std::endl;
    }

public:
    template <class... Args>
    auto debug(std::format_string<Args...> fmt, Args&&... args) const
    {
        log(Debug, std::format(fmt, std::forward<Args>(args)...));
    }

    template <class... Args>
    auto info(std::format_string<Args...> fmt, Args&&... args) const
    {
        log(Info, std::format(fmt, std::forward<Args>(args)...));
    }

    template <class... Args>
    auto warning(std::format_string<Args...> fmt, Args&&... args) const
    {
        log(Warning, std::format(fmt, std::forward<Args>(args)...));
    }

    template <class... Args>
    auto error(std::format_string<Args...> fmt, Args&&... args) const
    {
        log(Error, std::format(fmt, std::forward<Args>(args)...));
    }

    template <class... Args>
    auto log(const LogLevel level, std::format_string<Args...> fmt, Args&&... args) const
    {
        ThreadSafeIOStream& stream = std::cout;
        std::string_view prefix;
        switch (level)
        {
        case Debug:
            prefix = "[DEBUG] ";
        case Info:
            prefix = "[INFO] ";
        case Warning:
            prefix = "[WARNING] ";
        case Error:
            prefix = "[ERROR] ";
        default:
            throw std::invalid_argument("Unknown log level");
        }

        raw_log(stream, prefix, std::format(fmt, std::forward<Args>(args)...));
    }
};

#endif

#endif //LOGGER_HPP
