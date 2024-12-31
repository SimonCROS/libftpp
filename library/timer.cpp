//
// Created by Simon Cros on 29/12/2024.
//

#include "timer.hpp"

auto Timer::duration() const -> duration_type
{
    return m_duration;
}

auto Timer::remainingTime() const -> duration_type
{
    return duration() - elapsedTime();
}

auto Timer::timedOut() const -> bool
{
    return elapsedTime() >= duration();
}
