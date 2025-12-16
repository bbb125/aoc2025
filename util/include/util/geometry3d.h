#pragma once

#include <cstdint>
#include <tuple>

namespace aoc2025::geometry3d
{
struct Point
{
    std::int64_t x;
    std::int64_t y;
    std::int64_t z;
};

constexpr auto format_as(const Point& point)
{
    return std::tie(point.x, point.y, point.x);
}

constexpr auto euclideanDistanceSquare(const Point& a, const Point& b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)
           + (a.z - b.z) * (a.z - b.z);
}
static_assert(euclideanDistanceSquare({0, 0, 0}, {0, 0, 0}) == 0);
static_assert(euclideanDistanceSquare({0, 0, 0}, {1, 1, 1}) == 3);
static_assert(euclideanDistanceSquare({1, 2, 3}, {4, 5, 6}) == 27);
}  // namespace aoc2025::geometry3d