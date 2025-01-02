//
// Created by Simon Cros on 29/12/2024.
//

#ifndef TIMER_HPP
#define TIMER_HPP

#include "basic_chronometer.hpp"

class Timer : public BasicChronometer
{
private:
    duration_type m_duration{};

    template <class Rep, class Period>
    constexpr auto convert_duration(const std::chrono::duration<Rep, Period>& duration) const -> duration_type
    {
        return std::chrono::duration_cast<duration_type>(duration);
    }

public:
    Timer() = default;

    /**
     * Starts a new timer, which will time out after @p duration.
     * Use @ref timed_out to check if the timer has timed out.
     *
     * @param duration The duration of the timer, which will be cast to @ref duration_type using @ref std::chrono::duration_cast.
     */
    template <class Rep, class Period>
    explicit Timer(const std::chrono::duration<Rep, Period>& duration) : m_duration(convert_duration(duration))
    {
    }

    /**
     * Sets the new duration of the timer.
     *
     * @param duration The duration of the timer, which will be cast to @ref duration_type using @ref std::chrono::duration_cast.
     */
    template <class Rep, class Period>
    auto setDuration(const std::chrono::duration<Rep, Period>& duration) -> void
    {
        m_duration = convert_duration(duration);
    }

    using ChronometerBase::reset;
    using ChronometerBase::startOrResume;
    using ChronometerBase::pause;

    /**
     * Gets the duration of the timer.
     */
    [[nodiscard]] auto duration() const -> duration_type;

    /**
     * Calculates the remaining time before the Timer times out.
     *
     * @ref remainingTime() <= @ref duration_type::zero() is true when the Timer is already @ref timedOut
     *
     * @return The remaining time.
     */
    [[nodiscard]] auto remainingTime() const -> duration_type;

    /**
     * Checks if the timer has timed out, i.e. if @ref elapsed_time >= @ref duration.
     *
     * @return A boolean representing if the timer has timed out.
     */
    [[nodiscard]] auto timedOut() const -> bool;
};

#endif //TIMER_HPP
