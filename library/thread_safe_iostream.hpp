//
// Created by Simon Cros on 28/11/2024.
//

#ifndef THREAD_SAFE_IO_STREAM_HPP
#define THREAD_SAFE_IO_STREAM_HPP
#include <mutex>
#include <string>
#include <iostream>

#include "prefixed_o_stream.hpp"

template <class T>
concept Printable = requires(T value)
{
    std::cout << value;
    !std::is_same_v<T, std::ostream& (*)(std::ostream&)>;
};

template <class T>
concept Readable = requires(T value)
{
    std::cin >> value;
};

class ThreadSafeIOStream
{
    inline static std::mutex ms_writeMutex;
    inline static std::mutex ms_readMutex;

    PrefixedOStream m_prefixedStdout{std::cout};

public:
    auto setPrefix(const std::string& prefix) -> void;

    auto operator<<(std::ostream& (*manip)(std::ostream&)) -> ThreadSafeIOStream&; // required to accept endl

    template <Printable T>
    auto operator<<(const T& value) -> ThreadSafeIOStream&
    {
        std::lock_guard lock(ms_writeMutex);
        m_prefixedStdout << value;
        return *this;
    }

    template <Readable T>
    auto operator>>(T& value) -> ThreadSafeIOStream&
    {
        std::lock_guard lock(ms_readMutex);
        std::cin >> value;
        return *this;
    }
};

thread_local inline ThreadSafeIOStream threadSafeCout = {};

#endif //THREAD_SAFE_IO_STREAM_HPP
