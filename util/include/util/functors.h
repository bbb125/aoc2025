#pragma once

namespace aoc2025::functors
{
constexpr inline auto empty = [](const auto& container) noexcept
{
    return std::empty(container);
};

constexpr inline auto size = [](const auto& container) noexcept
{
    return std::size(container);
};

constexpr inline auto ssize = [](const auto& container) noexcept
{
    return std::ssize(container);
};

}  // namespace aoc2025::functors