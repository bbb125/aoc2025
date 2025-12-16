#pragma once
#include <cstdint>
#include <iterator>

namespace aoc2025::iterator
{
constexpr auto nth(auto& container, std::size_t n)
{
    return std::next(std::begin(container), n);
}

}  // namespace aoc2025::iterator