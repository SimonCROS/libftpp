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
    std::declval<std::stringstream>() << value;
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

    PrefixedOStream m_buffer{std::cout};

public:
    ThreadSafeIOStream() = default;

    explicit ThreadSafeIOStream(const std::ostream& stream): m_buffer(stream)
    {
    }

    ~ThreadSafeIOStream();

    auto setPrefix(const std::string& prefix) -> void;

    auto operator<<(std::ostream& (*manip)(std::ostream&)) -> ThreadSafeIOStream&; // required to accept endl

    template <ThreadSafePrintable T>
    auto operator<<(const T& value) -> ThreadSafeIOStream&
    {
        std::scoped_lock lock(ms_writeMutex);
        m_buffer << value;
        return *this;
    }

    template <ThreadSafeReadable T>
    auto operator>>(T& value) -> ThreadSafeIOStream&
    {
        std::scoped_lock lock(ms_readMutex);
        std::cin >> value;
        return *this;
    }

    template <class T>
    auto prompt(const std::string& question, T& dest) -> void
    {
        std::scoped_lock lock(ms_readMutex);
        m_buffer << question;
        m_buffer.flush();
        std::cin >> dest;
    }

    auto flush() -> void;
};

auto tscout() -> ThreadSafeIOStream&;

auto tscerr() -> ThreadSafeIOStream&;

#endif //THREAD_SAFE_IO_STREAM_HPP
