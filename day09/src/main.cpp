#include "util/geometry2d.h"
#include "util/stopwatch.h"
#include "util/views.h"
#include "util/funcional.h"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>

#include <ctre.hpp>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <span>
#include <vector>
#include <tuple>

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

    using Box = geometry2d::Box;
    auto boxFromIndices = [&points](auto pair)
    {
        auto [i, j] = pair;
        return geometry2d::orderPoints(points[i], points[j]);
    };

    // Build edge boxes -- to skip some good cases
    auto edges = rv::concat(points, rv::single(points[0])) | rv::adjacent<2>
                 | rv::transform(
                     [&](auto pair)
                     {
                         auto [p1, p2] = pair;
                         return orderPoints(p1, p2);
                     })
                 | rng::to<std::vector>();

    // Sort edges by their low.x so we can limit checks by x-range
    rng::sort(edges, {}, [](const auto& edge) { return edge.lo.x; });

    auto candidates =  //
        views::upperTriangle(std::ssize(points)) | rv::transform(boxFromIndices)
        | rv::filter(
            [&](const Box& box)
            {
                auto earlyExit = rv::take_while([&](const auto& edge)
                                                { return edge.lo.x < box.hi.x; });
                // only check edges with lo.x < box.hi.x
                return rng::none_of(  //
                    edges | earlyExit,
                    [&](const auto& edge)
                    {
                        return  // check if edge intersects rectangle
                            box.lo.x < edge.hi.x && box.lo.y < edge.hi.y
                            && box.hi.x > edge.lo.x && box.hi.y > edge.lo.y;
                    });
            });

    return std::ranges::max(  //
        candidates
        | rv::transform(functional::select<const geometry2d::Box&>(geometry2d::area)));
}

static_assert(solve2(testSet) == 24);

}  // namespace aoc2025::day09

int main()
{
    using namespace aoc2025::day09;
    namespace geometry2d = aoc2025::geometry2d;

    fmt::println("test2 {}", solve2(testSet));
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
            points.emplace_back(x.to_number(), y.to_number());
        else
        {
            fmt::println("Failed to parse line");
            return 1;
        }
    }
    aoc2025::time::Stopwatch sw;
    fmt::println("day09.solution1: {}", solve1(points));  // 4755429952
    fmt::println("Time elapsed: {}", sw.elapsed<>());

    sw = {};
    fmt::println("day09.solution2: {}", solve2(points));
    fmt::println("Time elapsed: {}", sw.elapsed<>());  // 10ms
    // 1429596008 - correct
}
