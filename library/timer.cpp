//
// Created by Simon Cros on 29/12/2024.
//

#include "timer.hpp"

auto Timer::reset() -> void
{
    m_start_time_point = clock_type::now();
}

auto Timer::start_time() const -> time_point
{
    return m_start_time_point;
}

auto Timer::duration() const -> duration_type
{
    return m_duration;
}

auto Timer::elapsed_time() const -> duration_type
{
    return clock_type::now() - m_start_time_point;
}

auto Timer::timed_out() const -> bool
{
    return elapsed_time() >= duration();
}
