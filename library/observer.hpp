//
// Created by Simon Cros on 26/11/2024.
//

#ifndef OBSERVER_HPP
#define OBSERVER_HPP
#include <unordered_map>
#include <vector>
#include <functional>

template <class TEvent>
class Observer
{
    using container_type = std::unordered_map<TEvent, std::vector<std::function<void()>>>;

public:
    void subscribe(const TEvent& event, const std::function<void()>& lambda)
    {
        typename container_type::iterator it;
        // Try emplace with _Args... empty will create new empty vector if key is not already registered
        std::tie(it, std::ignore) = m_observers.try_emplace(event);\
        it->second.push_back(lambda);
    }

    void notify(const TEvent& event)
    {
        const auto it = m_observers.find(event);
        if (it != m_observers.end())
        {
            for (const auto& element : it->second)
            {
                std::invoke(element);
            }
        }
    }

private:
    container_type m_observers;
};

#endif //OBSERVER_HPP
