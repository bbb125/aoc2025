#include "util/algorithm.h"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <span>
#include <string>
#include <vector>
#include <ranges>
#include <utility>

namespace aoc2025::day07
{


constexpr std::pair<std::int64_t, std::int64_t> solveImpl(std::span<std::string> input)
{
    struct Accumulator
    {
        std::vector<std::int64_t> line;
        std::int64_t splitCount;
    };
    auto workRow = [&]
    {
        return std::vector(std::size(input), 0ll);
    };
    auto result = std::ranges::fold_left(  //
        input,
        Accumulator{workRow(), 0ll},
        [next = workRow()](auto&& acc, const auto& line) mutable
        {
            std::ranges::fill(next, 0ll);
            for (auto [i, ch] : line | std::views::enumerate)
            {
                switch (ch)
                {
                case 'S':
                    next[i] = 1;
                    break;
                case '.':
                    next[i] += acc.line[i];
                    break;
                case '^':
                    acc.splitCount += acc.line[i] > 0;
                    assert(i > 0 && i + 1 < std::ssize(acc.line));
                    next[i - 1] += acc.line[i];
                    next[i + 1] += acc.line[i];
                    next[i] = 0;
                    break;
                }
            }
            std::swap(acc.line, next);
            return std::move(acc);
        });
    return {result.splitCount, algorithm::sum(result.line)};
}

static_assert(
    []
    {
        std::vector<std::string> input{
            ".......S.......",
            "...............",
            ".......^.......",
            "...............",
            "......^.^......",
            "...............",
            ".....^.^.^.....",
            "...............",
            "....^.^...^....",
            "...............",
            "...^.^...^.^...",
            "...............",
            "..^...^.....^..",
            "...............",
            ".^.^.^.^.^...^.",
            "...............",
        };
        return solveImpl(input) == std::pair{21ll, 40ll};
    }());
}  // namespace aoc2025::day07

int main()
{
    using namespace aoc2025::day07;

    std::ifstream file("./input.txt");
    if (!file)
    {
        fmt::println("Failed to open file");
        return 1;
    }

    std::vector<std::string> input;
    for (std::string line; std::getline(file, line);)
        input.push_back(line);
    auto [part1, part2] = solveImpl(input);  // 1496 too low -- 1709 too high -- 1587
    fmt::println("day07.solution1: {}", part1);
    fmt::println("day07.solution2: {}", part2);
}