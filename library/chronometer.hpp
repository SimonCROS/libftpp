//
// Created by Simon Cros on 30/12/2024.
//

#ifndef CHRONOMETER_HPP
#define CHRONOMETER_HPP
#include <chrono>
#include <optional>
#include <vector>

class Chronometer
{
public:
    using clock_type = std::chrono::steady_clock;
    using time_point = clock_type::time_point;

    /**
     * Most accurate duration type for Chronometer
     */
    using duration_type = clock_type::duration;

    struct Lap
    {
        duration_type lapDuration;
        duration_type totalDuration;

        bool operator==(const Lap& lap) const = default;
    };

private:
    bool m_running = false;
    bool m_paused = false;
    time_point m_startTimePoint{};
    time_point m_stopTimePoint{};
    duration_type m_pausedDuration{};
    std::vector<Lap> m_laps;

public:
    /**
     * Resets the state of the chronometer.
     */
    auto reset() -> void;

    [[nodiscard]] auto running() const -> bool;
    [[nodiscard]] auto paused() const -> bool;
    [[nodiscard]] auto laps() const -> const std::vector<Lap>&;

    auto startOrResume() -> bool;
    auto pause() -> bool;
    auto lap() -> std::optional<Lap>;

    [[nodiscard]] auto currentLap() const -> std::optional<Lap>;
};

#endif //CHRONOMETER_HPP
