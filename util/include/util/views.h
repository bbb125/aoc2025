#pragma once

#include <concepts>
#include <ranges>
#include <utility>

namespace aoc2025::views
{
template <std::integral I>
constexpr auto upper_triangle(I n)
{
    namespace rv = std::views;
    return rv::iota(I{}, n)
           | rv::transform(
               [n](auto i)
               {
                   auto inner =
                       rv::iota(i + 1, n)
                       | rv::transform([i](I j) { return std::pair{i, j}; });
                   return inner;  // prvalue view (safe)
               })
           | rv::join;
}
}  // namespace aoc2025::views
