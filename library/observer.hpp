//
// Created by Simon Cros on 26/11/2024.
//

#ifndef OBSERVER_HPP
#define OBSERVER_HPP
#include <functional>

template <class TEvent>
class Observer
{
public:
    void subscribe(const TEvent& event, const std::function<void()>& lambda)
    {
    }

    void notify(const TEvent& event)
    {
    }

private:
    std::unordered_map<TEvent, std::vector<std::function<void()>>> m_observers;
};

#endif //OBSERVER_HPP
