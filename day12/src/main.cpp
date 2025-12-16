#include "util/algorithm.h"

#include <ctre.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <ranges>

namespace aoc2025::day12
{
constexpr std::size_t numberOfShapes = 6;

struct Shape
{
    std::int64_t height = 0;
    std::int64_t width = 0;
    std::int64_t area = 0;
    std::vector<std::string> data;
};

struct Requirements
{
    std::int64_t width = 0;
    std::int64_t height = 0;
    std::vector<std::int64_t> shapes;
};


constexpr auto plus = []<typename T, typename U>(const std::pair<T, U>& l,
                                                 const std::pair<T, U>& r)
    -> std::pair<decltype(l.first + r.first), decltype(l.second + r.second)>
{
    return {l.first + r.first, l.second + r.second};
};
}  // namespace aoc2025::day12

int main()
{
    using namespace aoc2025::day12;
    namespace rv = std::ranges::views;
    namespace rng = std::ranges;
    std::ifstream file("input.txt");
    if (not file)
    {
        fmt::println("Failed to open input.txt");
        return 1;
    }

    std::vector<Shape> shapes;
    for (std::size_t i = 0; i < numberOfShapes; ++i)
    {
        std::string line;
        std::getline(file, line);

        std::vector<std::string> shape;
        for (std::string line; std::getline(file, line) && not std::empty(line);)
            shape.push_back(std::move(line));

        auto area = aoc2025::algorithm::sum(
            shape
            | rv::transform([](const auto& row) { return rng::count(row, '#'); }));
        shapes.push_back({
            .height = std::ssize(shape),
            .width = std::ssize(shape[0]),
            .area = area,
            .data = std::move(shape),
        });
    }
    std::vector<Requirements> requirements;
    // 41x48: 29 35 26 36 38 44
    for (std::string line; std::getline(file, line);)
    {
        auto [_, width, height, list] =
            ctre::match<"([0-9]+)x([0-9]+): ([0-9 ]+)">(line);
        requirements.push_back({
            .width = width.to_number<std::int64_t>(),
            .height = height.to_number<std::int64_t>(),
            .shapes = list | rv::split(' ')
                      | rv::transform(
                          [](auto part)
                          {
                              return static_cast<std::int64_t>(
                                  std::stoi(part | rng::to<std::string>()));
                          })
                      | rng::to<std::vector>()  //
        });
    }

    struct Stats
    {
        std::int64_t availableArea = 0;
        std::int64_t requiredSquareArea = 0;
        std::int64_t requiredArea = 0;
        double squareAreaRatio = 0.0;
        double areaRatio = 0.0;
    };
    auto stats =
        requirements
        | rv::transform(
            [&](const auto& req)
            {
                auto [requiredSquareAre, requiredArea] = rng::fold_left(  //
                    req.shapes | rv::enumerate
                        | rv::transform(
                            [&](auto val)
                            {
                                auto [shapeNum, count] = val;
                                const auto& shape = shapes[shapeNum];
                                return std::pair{shape.width * shape.height * count,
                                                 shape.area * count};
                            }),
                    std::pair<std::int64_t, std::int64_t>{0, 0},
                    plus);
                return Stats{
                    .availableArea = req.width * req.height,
                    .requiredSquareArea = requiredSquareAre,
                    .requiredArea = requiredArea,
                    .squareAreaRatio = static_cast<double>(requiredSquareAre)
                                       / (req.width * req.height),
                    .areaRatio = static_cast<double>(requiredArea)
                                 / (req.width * req.height),
                };
            })
        | rng::to<std::vector>();
    rng::sort(stats, {}, &Stats::areaRatio);
    for (const auto& [i, s] : stats | rv::enumerate)
    {
        fmt::println("{:>3}: {:>7}{:>7}{:>7} {:7.4} {:7.4}",
                     i + 1,
                     s.availableArea,
                     s.requiredSquareArea,
                     s.requiredArea,
                     s.squareAreaRatio,
                     s.areaRatio);
    }
    /**
     * Apparently there is no general case solution.
     * However, looking at the output and ratio between needed are (copmuted by
     * squre area or number occupied cells) and trying highest possible number
     * was a correct answer.
     *      440:    2304   2304   1701       1  0.7383 <--- right answer
     *      441:    2400   3348   2401   1.395       1
     */
    return 0;
}