//
// Created by scros on 12/31/24.
//

#include "chronometer_base.hpp"

auto ChronometerBase::reset() -> void
{
    m_running = false;
    m_paused = false;
    m_startTimePoint = {};
    m_stopTimePoint = {};
    m_pausedDuration = {};
}

auto ChronometerBase::startOrResume() -> bool
{
    if (running())
        return false;

    if (!paused())
        m_startTimePoint = clock_type::now();
    else
        m_pausedDuration += clock_type::now() - m_stopTimePoint;

    m_running = true;
    m_paused = false;
    m_stopTimePoint = {};
    return true;
}

auto ChronometerBase::pause() -> bool
{
    if (!running())
        return false;

    m_stopTimePoint = clock_type::now();
    m_running = false;
    m_paused = true;
    return true;
}

auto ChronometerBase::running() const -> bool
{
    return m_running;
}

auto ChronometerBase::paused() const -> bool
{
    return m_paused;
}

auto ChronometerBase::elapsedTime() const -> duration_type
{
    const time_point endTime = paused() ? m_stopTimePoint : clock_type::now();

    return endTime - m_startTimePoint - m_pausedDuration;
}
