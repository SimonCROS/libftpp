//
// Created by Simon Cros on 28/11/2024.
//

#include "thread_safe_iostream.hpp"

ThreadSafeIOStream::~ThreadSafeIOStream()
{
    flush();
}

auto ThreadSafeIOStream::setPrefix(const std::string& prefix) -> void
{
    std::scoped_lock lock(ms_writeMutex); // << overload (write) uses m_prefix
    m_buffer.setPrefix(prefix);
}

auto ThreadSafeIOStream::operator<<(std::ostream&(*manip)(std::ostream&)) -> ThreadSafeIOStream&
{
    m_buffer << manip;
    return *this;
}

auto ThreadSafeIOStream::flush() -> void
{
    std::scoped_lock lock(ms_readMutex);
    m_buffer.flush();
}
