//
// Created by Simon Cros on 12/31/24.
//

#ifndef CHRONOMETER_BASE_HPP
#define CHRONOMETER_BASE_HPP
#include <chrono>

class ChronometerBase {
public:
    using clock_type = std::chrono::steady_clock;
    using time_point = clock_type::time_point;

    /**
     * Most accurate duration type for Chronometer
     */
    using duration_type = clock_type::duration;

protected:
    ChronometerBase() = default;
    ChronometerBase(const ChronometerBase& other) = default;

    bool m_running = false;
    bool m_paused = false;
    time_point m_startTimePoint{};
    time_point m_stopTimePoint{};
    duration_type m_pausedDuration{};

    /**
     * Resets the state of the chronometer.
     */
    auto reset() -> void;

    auto startOrResume() -> bool;
    auto pause() -> bool;

public:
    [[nodiscard]] auto running() const -> bool;
    [[nodiscard]] auto paused() const -> bool;
    [[nodiscard]] auto elapsedTime() const -> duration_type;
};

#endif //CHRONOMETER_BASE_HPP
