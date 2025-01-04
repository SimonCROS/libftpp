//
// Created by Simon Cros on 1/4/25.
//

#include "logger.hpp"

#ifdef LIBFTPP_LOGGER

auto Logger::raw_log(ThreadSafeIOStream& stream, const std::string_view& prefix,
    const std::string_view& message) -> void
{
    stream << prefix << message << std::endl;
}

Logger::Logger(const LogLevel level): m_level(level)
{
}

auto Logger::setLevel(const LogLevel level) -> void
{
    m_level = level;
}

auto Logger::level() const -> LogLevel
{
    return m_level;
}

auto globalLogger() -> Logger&
{
    static Logger logger;
    return logger;
}

#endif
