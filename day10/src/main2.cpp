#include "util/algorithm.h"
#include "util/geometry2d.h"
#include "util/iterator.h"
#include "util/stopwatch.h"


#include <ctre.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <functional>
#include <fstream>
#include <ranges>
#include <span>
#include <string>
#include <vector>
#include <optional>
#include <generator>
#include <numeric>


namespace rv = std::ranges::views;
namespace rng = std::ranges;

namespace aoc2025::day10
{
using Container = std::vector<int>;
constexpr std::size_t maxSize = 16;
struct MachineConfiguration
{
    Container targetState;
    std::vector<Container> buttons;
};

using Row = std::vector<std::int64_t>;
using Matrix = std::vector<Row>;

constexpr auto isZero = std::bind_front(std::equal_to{}, 0);

void eliminateRowWith(Row& target, const Row& pivot, std::size_t pivotIndex)
{
    if (target[pivotIndex] == 0)
        return;

    auto gcd = std::gcd(pivot[pivotIndex], target[pivotIndex]);

    auto targetMultiplier = target[pivotIndex] / gcd;
    auto pivotMultiplier = pivot[pivotIndex] / gcd;

    std::transform(                        //
        iterator::nth(pivot, pivotIndex),  // source
        std::cend(pivot),
        iterator::nth(target, pivotIndex),  // second source
        iterator::nth(target, pivotIndex),  // destination
        [&](auto pivot, auto target)
        { return target * pivotMultiplier - pivot * targetMultiplier; });
}

void swapColumn(Matrix& target, std::size_t lhs, std::size_t rhs)
{
    for (auto& row : target)
        std::swap(row[lhs], row[rhs]);
}

auto gaussianElimination(Matrix& matrix)
{
    const auto width = std::ssize(matrix[0]);
    const auto height = std::ssize(matrix);
    // We will bruteforce free variables later, so we need to know their limits
    // For each column, find the minimum target value it affects.
    // Limits are part of Gaussian elimination because we may swap columns
    // and need to track limits accordingly.
    auto limits =
        rv::iota(0, width - 1)
        | rv::transform(
            [&](auto i)
            {
                return rng::min(
                    rv::iota(0, height)
                    | rv::filter([&](auto j) { return matrix[j][i] != 0; })
                    | rv::transform([&](auto j) { return matrix[j].back(); }));
            })
        | rng::to<std::vector>();

    auto workSize = height;
    for (std::int64_t i = 0; i < workSize; ++i)
    {
        // find non zero pivot
        auto pivot = [&]() -> std::optional<std::pair<std::int64_t, std::int64_t>>
        {
            // prioritize row swap
            auto rightBottomRectangle =
                rv::cartesian_product(rv::iota(i, width - 1), rv::iota(i, workSize));
            auto it = rng::find_if_not(rightBottomRectangle,
                                       [&](auto val)
                                       {
                                           auto [col, row] = val;
                                           return matrix[row][col] == 0;
                                       });
            if (std::end(rightBottomRectangle) == it)
                return std::nullopt;
            return *it;
        }();
        if (not pivot)
            std::abort();  // brute force will no handle this, need a better implementation

        auto [col, row] = *pivot;
        std::swap(matrix[i], matrix[row]);
        if (i != col)
        {
            swapColumn(matrix, i, col);
            std::swap(limits[i], limits[col]);
        }

        // Eliminate rows below
        for (auto j = i + 1; j < workSize; ++j)
            eliminateRowWith(matrix[j], matrix[i], i);

        // Move zero rows to the end
        auto removed = std::erase_if(  //
            matrix,
            [&](const auto& row) { return rng::all_of(row, isZero); });
        workSize -= removed;
    }
    return limits;
}

constexpr void forEachFreeVariable(std::span<const std::int64_t> multipliers,
                                   std::span<const std::int64_t> limits,
                                   std::int64_t target,
                                   auto function)
{
    // It was nice to implement this as std::generator, but performance is terrible.
    // I think a great use case for coroutines is cases when I/O is a bottleneck,
    // or not very performant parsers, when it doesn't make a difference.
    const auto size = std::size(multipliers);
    Row workingResult(size, 0);
    // std::array<std::int64_t, maxSize> workingResult{};

    auto dfs = [&](this auto&& self, std::size_t i, std::int64_t rem) -> void
    {
        if (i == size)
        {
            function(std::span{std::begin(workingResult), size});
            return;
        }

        for (auto x : rv::iota(0, limits[i] + 1))
        {
            workingResult[i] = x;
            self(i + 1, rem - x * multipliers[i]);
        }
    };

    dfs(0, target);
}

constexpr void numberOfPresses(Matrix& matrix,
                               std::span<const std::int64_t> freeVariables,
                               auto andThen)
{
    // Backtracking variables evaluation for given free variables solution
    std::array<std::int64_t, maxSize> solution;
    const auto width = std::size(matrix[0]) - 1;
    for (auto [i, val] : freeVariables | rv::enumerate)
        solution[std::size(matrix[0]) - 1 - std::ssize(freeVariables) + i] = val;

    for (auto [i, row] : matrix | rv::enumerate | rv::reverse)
    {
        auto result = row.back();
        for (auto j = i + 1; std::cmp_less(j, width); ++j)
            result -= row[j] * solution[j];

        solution[i] = result / row[i];
        // If the result if fractional or negative - this solution is wrong
        if (solution[i] < 0 or result % row[i] != 0)
            return;
    }
    andThen(std::span{std::begin(solution), width});
}


constexpr std::int64_t solve(const MachineConfiguration& config)
{
    // Make a matrix for input
    // (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
    //   0     0   0     0     0     1      = 3
    //   0     1   0     0     0     1      = 5
    //   0     0   1     1     1     0      = 4
    //   1     1   0     1     0     0      = 7
    //-----------------------------------
    //   1     3  0      3     1     2
    auto matrix =  //
        rv::iota(0, std::ssize(config.targetState))
        | rv::transform(
            [&](std::size_t i)
            {
                auto result = std::vector(std::size(config.buttons) + 1, 0);
                result.back() = config.targetState[i];
                return result;
            })
        | rng::to<Matrix>();

    for (const auto& [i, button] : config.buttons | rv::enumerate)
    {
        for (auto value : button)
            matrix[value][i] = 1;
    }

    auto limits = gaussianElimination(matrix);

    // We have diagonal here
    const auto diagonal = std::size(matrix) - 1;
    const auto bruteForceStart = diagonal + 1;
    const auto bruteForceSize = std::size(matrix[0]) - 1 - bruteForceStart;
    auto min = std::numeric_limits<std::int64_t>::max();
    forEachFreeVariable(  //
        std::span{iterator::nth(matrix[diagonal], bruteForceStart), bruteForceSize},
        std::span{iterator::nth(limits, bruteForceStart), bruteForceSize},
        matrix[diagonal].back(),
        [&](auto foundSolution)
        {
            numberOfPresses(  //
                matrix,
                foundSolution,
                [&](std::span<std::int64_t> solution)
                { min = std::min(min, algorithm::sum(solution)); });
        });
    return min;
}

constexpr auto solve2(std::span<const MachineConfiguration> input)
{
    std::vector correctAnswersToTestRefactoring{
        58,  257, 63,  81,  84,  97,  43,  109, 72,  44,  95,  262, 24,  77,
        34,  53,  63,  122, 69,  254, 206, 75,  81,  101, 68,  256, 228, 83,
        70,  235, 85,  211, 228, 43,  199, 106, 287, 106, 24,  32,  91,  244,
        98,  50,  244, 138, 76,  245, 57,  93,  70,  149, 52,  63,  125, 63,
        83,  280, 70,  102, 115, 176, 193, 178, 231, 159, 246, 97,  5,   63,
        45,  193, 224, 207, 58,  85,  60,  117, 41,  75,  142, 109, 239, 163,
        69,  64,  83,  270, 160, 74,  31,  95,  42,  24,  83,  47,  277, 68,
        11,  22,  44,  34,  42,  43,  31,  48,  197, 62,  38,  100, 145, 97,
        113, 210, 59,  29,  26,  77,  221, 224, 108, 111, 160, 111, 35,  63,
        88,  88,  54,  57,  44,  59,  67,  87,  46,  95,  140, 217, 67,  96,
        265, 48,  268, 122, 68,  135, 22,  184, 150, 39,  66,  41,  31,  21,
        246, 91,  258, 75,  40,  103, 18,  33,  107, 105, 46,  123, 51,  87,
        50,  141, 58,  88,  94,  94,  50,  82,  94,  164, 51,  20,  231, 116,
        55,  64,  70,  69,  110, 62,  184};

    //
    // int i = 0;
    // for ([[maybe_unused]] auto [expected, solution] :
    //      rv::zip(correctAnswersToTestRefactoring, input | rv::transform(solve)))
    // {
    //     fmt::println("{} expected: {}, got: {}, {}",
    //                  i++,
    //                  expected,
    //                  solution,
    //                  expected == solution ? "OK" : "FAIL");
    //     // assert(expected == solution);
    // }
    // return 0;
    auto solutions = input | rv::transform(solve);
    return algorithm::sum(solutions);
}

// static_assert(
//     []
//     {
//         return solve({.targetState = {3, 5, 4, 7},
//                       .buttons = {Row{3}, Row{1, 3}, Row{2}, Row{2, 3}, Row{0, 2}, Row{0, 1}}})
//                == 10;
//     }());
}  // namespace aoc2025::day10

int main()
{
    using namespace aoc2025::day10;

    std::ifstream file("./input.txt");
    if (not file)
    {
        fmt::println("Could not open input.txt");
        return 1;
    }

    std::vector<MachineConfiguration> configurations;
    for (std::string line; std::getline(file, line);)
    {
        auto [_1, _2, buttons, joltages] =
            ctre::match<R"(\[([\.#]+)\] (\(.*\)) \{(.*)\})">(line);
        auto joltageValues =
            ctre::search_all<R"(\d+)">(joltages.to_view())
            | rv::transform([](auto match) { return match.to_number(); })
            | rng::to<Container>();

        auto buttonsImpact =
            ctre::search_all<R"(\([0-9,]+\))">(buttons.to_view())
            | rv::transform(
                [](auto match)
                {
                    // Extract numbers from within this parenthetical group
                    return ctre::search_all<R"(\d+)">(match.to_view())
                           | rv::transform([](auto numMatch)
                                           { return numMatch.to_number(); })
                           | rng::to<Container>();
                })
            | rng::to<std::vector>();

        configurations.push_back({std::move(joltageValues), std::move(buttonsImpact)});
    }
    aoc2025::time::Stopwatch<> stopwatch;
    fmt::println("day10.solution2: {}", solve2(configurations));  // 20142
    fmt::println("Time elapsed: {}", stopwatch.elapsed());        // 40ms
}
