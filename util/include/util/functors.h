#pragma once

namespace aoc2025::functors
{
constexpr auto empty = [](const auto& container) noexcept
{
    return std::empty(container);
};

constexpr auto size = [](const auto& container) noexcept
{
    return std::size(container);
};

constexpr auto ssize = [](const auto& container) noexcept
{
    return std::ssize(container);
};

}  // namespace aoc2025::functors