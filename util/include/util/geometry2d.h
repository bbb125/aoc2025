#pragma once

#include <algorithm>
#include <cstdint>
#include <tuple>
#include <utility>

namespace aoc2025::geometry2d
{
struct Point
{
    std::int64_t x;
    std::int64_t y;
};

constexpr auto format_as(const Point& point)
{
    return std::tie(point.x, point.y);
}

constexpr auto orderPoints(Point p1, Point p2)
{
    auto [left, right] = std::minmax(p1.x, p2.x);
    auto [bottom, top] = std::minmax(p1.y, p2.y);
    return std::pair{Point{left, bottom}, Point{right, top}};
}

constexpr auto area(Point p1, Point p2)
{
    auto [lo, hi] = orderPoints(p1, p2);
    return (hi.x - lo.x + 1) * (hi.y - lo.y + 1);
}

}  // namespace aoc2025::geometry2d