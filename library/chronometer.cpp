//
// Created by Simon Cros on 30/12/2024.
//

#include "chronometer.hpp"

auto Chronometer::reset() -> void
{
    ChronometerBase::reset();
    m_laps.clear();
}

auto Chronometer::lap() -> std::optional<Lap>
{
    const auto e_lap = currentLap();

    if (e_lap)
        m_laps.push_back(*e_lap);

    return e_lap;
}

auto Chronometer::laps() const -> const std::vector<Lap>&
{
    return m_laps;
}

auto Chronometer::currentLap() const -> std::optional<Lap>
{
    if (!running() && !paused())
        return std::nullopt;

    const time_point endTime = paused() ? m_stopTimePoint : clock_type::now();

    const duration_type lastLapTotalDuration = m_laps.empty() ? duration_type{} : m_laps.back().totalDuration;
    const duration_type endDuration = endTime - m_startTimePoint - m_pausedDuration;

    return std::optional<Lap>(std::in_place, endDuration - lastLapTotalDuration, endDuration);
}
