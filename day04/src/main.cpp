#include "util/stopwatch.h"


#include <fmt/format.h>
#include <fmt/chrono.h>


#include <array>
#include <cstdint>
#include <fstream>
#include <vector>
#include <ranges>
#include <string>


namespace aoc2025::day04
{

constexpr bool inBounds(auto matrix, std::int64_t i, std::int64_t j)
{
    return i >= 0 && i < std::ssize(matrix) && j >= 0 && j < std::ssize(matrix[i]);
}

constexpr auto matrixIndicies(std::int64_t height, std::int64_t width)
{
    namespace rv = std::views;
    return rv::cartesian_product(rv::iota(0, height), rv::iota(0, width));
}

constexpr auto directions = std::to_array<std::pair<std::int64_t, std::int64_t>>({
    {-1, -1},
    {-1, 0},
    {-1, 1},
    {0, -1},
    {0, 1},
    {1, -1},
    {1, 0},
    {1, 1},
});


constexpr auto solveStep(std::vector<std::string>& input)
{
    std::vector counts(std::size(input), std::vector(std::size(input[0]), 0));

    constexpr int ignore = 100;
    for (auto [i, j] : matrixIndicies(std::ssize(input), std::size(input[0])))
    {
        if (input[i][j] == '.')
        {
            counts[i][j] = ignore;
            continue;
        }

        for (auto [di, dj] : directions)
        {
            auto ni = i + di;
            auto nj = j + dj;
            if (inBounds(counts, ni, nj) && input[ni][nj])
                counts[ni][nj] += 1;
        }
    }

    // remove ones with less than 4 neighbors
    std::int64_t result = 0;
    for (auto [i, j] : matrixIndicies(std::ssize(input), std::ssize(input[0])))
    {
        if (counts[i][j] < 4)
        {
            ++result;
            input[i][j] = '.';
        }
    }
    return result;
}

constexpr auto solve1(std::vector<std::string> input)
{
    return solveStep(input);
}

static_assert(
    []
    {
        using std::string_literals::operator""s;
        std::vector input{
            "..@@.@@@@."s,
            "@@@.@.@.@@"s,
            "@@@@@.@.@@"s,
            "@.@@@@..@."s,
            "@@.@@@@.@@"s,
            ".@@@@@@@.@"s,
            ".@.@.@.@@@"s,
            "@.@@@.@@@@"s,
            ".@@@@@@@@."s,
            "@.@.@@@.@."s,
        };
        return solve1(input) == 13;
    }());


constexpr auto solve2(std::vector<std::string> input)
{
    std::int64_t result = 0;
    while (auto num = solveStep(input))
        result += num;

    return result;
}

static_assert(
    []
    {
        using std::string_literals::operator""s;
        std::vector input{
            "..@@.@@@@."s,
            "@@@.@.@.@@"s,
            "@@@@@.@.@@"s,
            "@.@@@@..@."s,
            "@@.@@@@.@@"s,
            ".@@@@@@@.@"s,
            ".@.@.@.@@@"s,
            "@.@@@.@@@@"s,
            ".@@@@@@@@."s,
            "@.@.@@@.@."s,
        };
        return solve2(input) == 43;
    }());

}  // namespace aoc2025::day04

int main()
{
    using namespace aoc2025::day04;

    std::ifstream file("./input.txt");
    if (!file)
    {
        fmt::println("Failed to read input file");
        return 1;
    }
    auto input = std::views::istream<std::string>(file)
                 | std::ranges::to<std::vector<std::string>>();

    aoc2025::time::Stopwatch<> stopwatch;
    fmt::println("day04.solution1: {}", solve1(input));
    fmt::println("Time elapsed: {}", stopwatch.elapsed());
    stopwatch = {};
    fmt::println("day04.solution2: {}", solve2(input));
    fmt::println("Time elapsed: {}", stopwatch.elapsed());
}