//
// Created by Simon Cros on 28/11/2024.
//

#include "thread_safe_iostream.hpp"

auto ThreadSafeIOStream::setPrefix(const std::string& prefix) -> void
{
    std::lock_guard lock(ms_writeMutex); // << overload (write) uses m_prefix
    m_prefixedStdout.setPrefix(prefix);
}

auto ThreadSafeIOStream::operator<<(std::ostream&(* manip)(std::ostream&)) -> ThreadSafeIOStream&
{
    m_prefixedStdout << manip;
    return *this;
}
