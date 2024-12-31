//
// Created by Simon Cros on 30/12/2024.
//

#ifndef CHRONOMETER_HPP
#define CHRONOMETER_HPP
#include <optional>
#include <vector>

#include "basic_chronometer.hpp"

class Chronometer : public ChronometerBase
{
public:
    struct Lap
    {
        duration_type lapDuration;
        duration_type totalDuration;

        bool operator==(const Lap& lap) const = default;
    };

private:
    std::vector<Lap> m_laps;

public:
    /**
     * Resets the state of the chronometer.
     */
    auto reset() -> void;

    using ChronometerBase::startOrResume;
    using ChronometerBase::pause;

    auto lap() -> std::optional<Lap>;

    [[nodiscard]] auto laps() const -> const std::vector<Lap>&;
    [[nodiscard]] auto currentLap() const -> std::optional<Lap>;
};

#endif //CHRONOMETER_HPP
