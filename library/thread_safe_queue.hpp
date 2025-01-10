//
// Created by Simon Cros on 28/11/2024.
//

#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP
#include <deque>
#include <mutex>
#include <queue>
#include <stdexcept>

template<class TType>
class ThreadSafeQueue {
    std::mutex m_mutex;
    std::queue<TType> m_deque;

public:
    auto empty() -> bool
    {
        std::scoped_lock lock(m_mutex);
        return m_deque.empty();
    }

    auto push(const TType& newElement) -> void
    {
        std::scoped_lock lock(m_mutex);
        m_deque.push(newElement);
    }

    auto pop() -> TType
    {
        std::scoped_lock lock(m_mutex);

        if (m_deque.empty())
            throw std::runtime_error("Queue is empty");

        TType tmp = m_deque.front();
        m_deque.pop();
        return tmp; // copy elision will happen if possible
    }
};

#endif //THREAD_SAFE_QUEUE_HPP
