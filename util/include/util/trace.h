#pragma once

#include <fmt/format.h>

namespace aoc2025::diagnostic
{
/**
 * Simple tracing utility that can be enabled/disabled at compile time.
 * And constexpr friendly (does nothing in constant evaluated context).
 */
template <bool Enabled>
consteval auto makeTracer()
{
    return []<class... Args>(fmt::format_string<Args...> fmt, Args&&... args) constexpr
    {
        if constexpr (Enabled)
        {
            if (not std::is_constant_evaluated())
                fmt::println(fmt, std::forward<Args>(args)...);
        }
    };
}

}  // namespace aoc2025::diagnostic
