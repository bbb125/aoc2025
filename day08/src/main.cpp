#include "util/geometry3d.h"
#include "util/views.h"
#include "util/functors.h"

#include <ctre.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <functional>
#include <span>
#include <vector>
#include <ranges>
#include <optional>

namespace aoc2025::day08
{

constexpr auto solveTree(std::span<const geometry3d::Point> points,
                         std::optional<std::int64_t> iterations)
{
    namespace rv = std::ranges::views;
    namespace rng = std::ranges;

    struct Edge
    {
        std::int64_t id1;
        std::int64_t id2;
        std::int64_t distance;
    };

    auto edges =
        views::upperTriangle(std::ssize(points))
        | rv::transform(
            [&](auto val) -> Edge
            {
                auto [i, j] = val;
                return {.id1 = i,
                        .id2 = j,
                        .distance = geometry3d::euclideanDistanceSquare(points[i],
                                                                        points[j])};
            })
        | rng::to<std::vector>();
    rng::sort(edges, std::less{}, &Edge::distance);
    auto vertexColor = rv::iota(0ll, std::ssize(points)) | rng::to<std::vector>();
    std::vector<std::pair<std::int64_t, std::int64_t>> connections;
    auto notDone = [&](const auto& val)
    {
        const auto& [itNum, _] = val;
        return iterations.transform(std::bind_front(std::less{}, itNum))
            .value_or(std::ssize(connections) < std::ssize(points) - 1);
    };
    for (const auto& [_, edge] : edges | rv::enumerate | rv::take_while(notDone))
    {
        auto newColor = vertexColor[edge.id1];
        auto oldColor = vertexColor[edge.id2];
        if (newColor == oldColor)
            continue;

        connections.emplace_back(edge.id1, edge.id2);
        // we already O(N^2), because of computing distances, so don't care about another loop
        rng::replace(vertexColor, oldColor, newColor);
    }
    return std::pair{std::move(vertexColor), std::move(connections)};
}

constexpr auto solve1(std::span<const geometry3d::Point> points, std::int64_t iterations)
{
    namespace rv = std::ranges::views;
    namespace rng = std::ranges;

    auto [vertexColor, _] = solveTree(points, iterations);
    rng::sort(vertexColor);
    auto groups = vertexColor | rv::chunk_by(rng::equal_to{})
                  | rv::transform(functors::ssize) | rng::to<std::vector>();
    rng::sort(groups, std::greater{});
    return rng::fold_left(groups | rv::take(3), 1ll, std::multiplies{});
}

constexpr auto testSet = std::to_array<geometry3d::Point>({
    {162, 817, 812}, {57, 618, 57},   {906, 360, 560}, {592, 479, 940},
    {352, 342, 300}, {466, 668, 158}, {542, 29, 236},  {431, 825, 988},
    {739, 650, 466}, {52, 470, 668},  {216, 146, 977}, {819, 987, 18},
    {117, 168, 530}, {805, 96, 715},  {346, 949, 466}, {970, 615, 88},
    {941, 993, 340}, {862, 61, 35},   {984, 92, 344},  {425, 690, 689},
});

static_assert(solve1(testSet, 10) == 40);

constexpr auto solve2(std::span<const geometry3d::Point> points)
{
    auto [_, connections] = solveTree(points, std::nullopt);
    auto [i, j] = connections.back();
    return points[i].x * points[j].x;
}

static_assert(solve2(testSet) == 25272);

}  // namespace aoc2025::day08
int main()
{
    using namespace aoc2025::day08;

    std::ifstream file("./input.txt");
    if (not file)
    {
        fmt::println("Failed to open file");
        return 1;
    }
    std::vector<aoc2025::geometry3d::Point> points;
    for (std::string line; std::getline(file, line);)
    {
        if (auto [_, x, y, z] = ctre::match<"([0-9]+),([0-9]+),([0-9]+)">(line))
        {
            points.emplace_back(std::stoll(x.data()),
                                std::stoll(y.data()),
                                std::stoll(z.data()));
        }
        else
        {
            fmt::println("Failed to parse line");
            return 1;
        }
    }
    fmt::println("day08.solution1: {}", solve1(points, 1000));
    fmt::println("day08.solution2: {}", solve2(points));
}