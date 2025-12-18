#pragma once

#include <chrono>

namespace aoc2025::time
{
/**
 * The units of measurement for time elapsed.
 */
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using SecondsAsDouble = std::chrono::duration<double>;

/**
 * A Stopwatch wrapper that saves the creation time and allows to obtain time
 * elapsed since the creation.
 */
template <typename ClockT = std::chrono::steady_clock>
class Stopwatch
{
public:
    using Clock = ClockT;

    /**
     * Returns time elapsed since the timer creation in specified units.
     * @tparam T units of measurement.
     * @return the time elapsed.
     */
    template <typename T = Milliseconds>
    T elapsed() const
    {
        return std::chrono::duration_cast<T>(Clock::now() - start_);
    }

private:
    std::chrono::time_point<Clock> start_ = Clock::now();
};
}  // namespace aoc2025::time