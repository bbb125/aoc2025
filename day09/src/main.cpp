#include "util/geometry2d.h"
#include "util/views.h"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <ctre.hpp>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <span>
#include <vector>

namespace aoc2025::day09
{

constexpr auto solve1(std::span<const geometry2d::Point> points)
{
    namespace rv = std::ranges::views;
    return std::ranges::max(  //
        views::upperTriangle(std::ssize(points))
        | rv::transform(
            [points](auto pair)
            {
                auto [i, j] = pair;
                return area(points[i], points[j]);
            }));
}

constexpr auto testSet = std::to_array<geometry2d::Point>({
    {7, 1},
    {11, 1},
    {11, 7},
    {9, 7},
    {9, 5},
    {2, 5},
    {2, 3},
    {7, 3},
});
static_assert(solve1(testSet) == 50);

constexpr auto solve2(std::span<const geometry2d::Point> points)
{
    namespace rv = std::ranges::views;
    namespace rng = std::ranges;
    // ugly brute force, but will work in given dimensions

    auto candidates =  //
        views::upperTriangle(std::ssize(points))
        | rv::filter(
            [points](auto pair)
            {
                auto [i, j] = pair;
                auto bounds = orderPoints(points[i], points[j]);

                return rng::none_of(  //
                    rv::concat(points, rv::single(points[0])) | rv::adjacent<2>,
                    [&](auto pair)
                    {
                        auto [p1, p2] = pair;
                        auto [low, hi] = orderPoints(p1, p2);

                        return  // check if edge intersects rectangle
                            bounds.first.x < hi.x && bounds.first.y < hi.y
                            && bounds.second.x > low.x && bounds.second.y > low.y;
                    });
            });
    return std::ranges::max(  //
        candidates
        | rv::transform(
            [points](auto pair)
            {
                auto [i, j] = pair;
                return area(points[i], points[j]);
            }));
}
static_assert(solve2(testSet) == 24);

}  // namespace aoc2025::day09

int main()
{
    using namespace aoc2025::day09;
    namespace geometry2d = aoc2025::geometry2d;

    std::ifstream file("./input.txt");
    if (not file)
    {
        fmt::println("Failed to open file");
        return 1;
    }
    std::vector<geometry2d::Point> points;
    for (std::string line; std::getline(file, line);)
    {
        if (auto [_, x, y] = ctre::match<"([0-9]+),([0-9]+)">(line))
            points.push_back({x.to_number(), y.to_number()});
        else
        {
            fmt::println("Failed to parse line");
            return 1;
        }
    }
    fmt::println("day09.solution1: {}", solve1(points));  // 4755429952
    fmt::println("day09.solution2: {}",
                 solve2(points));  // 4537026090 too high, 1429564036 too low
    // 1429596008 - correct
}
