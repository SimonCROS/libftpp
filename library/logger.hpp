//
// Created by Simon Cros on 1/4/25.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <version>

// std::format_string was standardized on __cpp_lib_format >= 202207L.
// However, this class is present in implementations where P2508R1 has been adopted, but P2419R2 has not yet been implemented.
#if __cpp_lib_format >= 202207L || (__cpp_lib_format >= 201907L && defined(LIBFTPP_ENABLE_P2508R1))

#ifndef LIBFTPP_LOGGER
#define LIBFTPP_LOGGER
#endif

#include <format>
#include <string_view>

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
    LogLevel m_level = Info;

    static auto raw_log(ThreadSafeIOStream& stream, const std::string_view& prefix,
                        const std::string_view& message) -> void;

public:
    constexpr Logger() = default;

    explicit Logger(LogLevel level);

    auto setLevel(LogLevel level) -> void;

    [[nodiscard]] auto level() const -> LogLevel;

    template <class... Args>
    auto debug(std::format_string<Args...> fmt, Args&&... args) const
    {
        log(Debug, fmt, std::forward<Args>(args)...);
    }

    template <class... Args>
    auto info(std::format_string<Args...> fmt, Args&&... args) const
    {
        log(Info, fmt, std::forward<Args>(args)...);
    }

    template <class... Args>
    auto warning(std::format_string<Args...> fmt, Args&&... args) const
    {
        log(Warning, fmt, std::forward<Args>(args)...);
    }

    template <class... Args>
    auto error(std::format_string<Args...> fmt, Args&&... args) const
    {
        log(Error, fmt, std::forward<Args>(args)...);
    }

    template <class... Args>
    auto log(const LogLevel level, std::format_string<Args...> fmt, Args&&... args) const
    {
        if (m_level > level)
            return;

        ThreadSafeIOStream* stream;
        std::string_view prefix;
        switch (level)
        {
        case Debug:
            prefix = "[DEBUG] ";
            stream = &tscout();
            break;
        case Info:
            prefix = "[INFO] ";
            stream = &tscout();
            break;
        case Warning:
            prefix = "[WARNING] ";
            stream = &tscout();
            break;
        case Error:
            prefix = "[ERROR] ";
            stream = &tscerr();
            break;
        default:
            throw std::invalid_argument("Unknown log level");
        }

        raw_log(*stream, prefix, std::format(fmt, std::forward<Args>(args)...));
    }
};

auto globalLogger() -> Logger&;

#define LOG_LEVEL(level) globalLogger().setLevel(level)

#define LOG_DEBUG(format, ...) globalLogger().debug(format __VA_OPT__(, ) __VA_ARGS__)
#define LOG_INFO(format, ...) globalLogger().info(format __VA_OPT__(, ) __VA_ARGS__)
#define LOG_WARNING(format, ...) globalLogger().warning(format __VA_OPT__(, ) __VA_ARGS__)
#define LOG_ERROR(format, ...) globalLogger().error(format __VA_OPT__(, ) __VA_ARGS__)

#endif

#endif //LOGGER_HPP
