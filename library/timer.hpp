//
// Created by Simon Cros on 29/12/2024.
//

#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>

class Timer
{
public:
    using clock_type = std::chrono::steady_clock;
    using time_point = clock_type::time_point;

    /**
     * Most accurate duration type for Timer
     */
    using duration_type = clock_type::duration;

private:
    time_point m_start_time_point;
    duration_type m_duration;

    template <typename Rep, typename Period>
    constexpr auto convert_duration(const std::chrono::duration<Rep, Period>& duration) const -> duration_type
    {
        return std::chrono::duration_cast<duration_type>(duration);
    }

public:
    /**
     * Starts a new timer, which will time out after @p duration.
     * Use @ref timed_out to check if the timer has timed out.
     *
     * @param duration The duration of the timer, which will be cast to @ref duration_type using @ref std::chrono::duration_cast.
     */
    template <typename Rep, typename Period>
    explicit Timer(const std::chrono::duration<Rep, Period>& duration) : m_duration(convert_duration(duration))
    {
        reset();
    }

    /**
     * Sets the new duration of the timer.
     *
     * @param duration The duration of the timer, which will be cast to @ref duration_type using @ref std::chrono::duration_cast.
     */
    template <typename Rep, typename Period>
    auto set_duration(const std::chrono::duration<Rep, Period>& duration) -> void
    {
        m_duration = convert_duration(duration);
    }

    /**
     * Resets the start time of the timer.
     */
    auto reset() -> void;

    [[nodiscard]] auto start_time() const -> time_point;

    [[nodiscard]] auto duration() const -> duration_type;

    [[nodiscard]] auto elapsed_time() const -> duration_type;

    /**
     * Checks if the timer has timed out, i.e. if @ref elapsed_time >= @ref duration.
     *
     * @return A boolean representing if the timer has timed out.
     */
    [[nodiscard]] auto timed_out() const -> bool;
};

#endif //TIMER_HPP
