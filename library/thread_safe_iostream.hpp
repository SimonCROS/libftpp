//
// Created by Simon Cros on 28/11/2024.
//

#ifndef THREAD_SAFE_IO_STREAM_HPP
#define THREAD_SAFE_IO_STREAM_HPP
#include <mutex>
#include <string>
#include <iostream>
#include <sstream>

#include "prefixed_ostream.hpp"

template <class T>
concept ThreadSafePrintable = requires(T value)
{
    std::declval<std::ostringstream>() << value;
    !std::is_same_v<T, std::ostream& (*)(std::ostream&)>;
};

template <class T>
concept ThreadSafeReadable = requires(T value)
{
    std::cin >> value;
};

class ThreadSafeIOStream
{
    inline static std::mutex ms_writeMutex;
    inline static std::mutex ms_readMutex;

    std::ostringstream m_stream;

    PrefixedOStream m_prefixedStdout{std::cout};

public:
    auto setPrefix(const std::string& prefix) -> void;

    auto operator<<(std::ostream& (*manip)(std::ostream&)) -> ThreadSafeIOStream&; // required to accept endl

    template <ThreadSafePrintable T>
    auto operator<<(const T& value) -> ThreadSafeIOStream&
    {
        std::lock_guard lock(ms_writeMutex);
        m_stream << value;


        // m_prefixedStdout << value;
        return *this;
    }

    template <ThreadSafeReadable T>
    auto operator>>(T& value) -> ThreadSafeIOStream&
    {
        std::lock_guard lock(ms_readMutex);
        std::cin >> value;
        return *this;
    }
};

thread_local inline ThreadSafeIOStream threadSafeCout = {};

#endif //THREAD_SAFE_IO_STREAM_HPP
