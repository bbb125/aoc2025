#include "util/algorithm.h"
#include "util/geometry2d.h"
#include "util/iterator.h"
#include "util/stopwatch.h"
#include "util/trace.h"


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

// Limit the number of buttons to allocate fixed-size arrays on the stack.
// Could use pmr or inline vector, but this is simpler and also constexpr compatible.
constexpr std::size_t maxSize = 16;
constexpr auto enableTraceMode = false;
constexpr auto trace = diagnostic::makeTracer<enableTraceMode>();

struct MachineConfiguration
{
    Container targetState;
    std::vector<Container> buttons;
};

using Row = std::vector<std::int64_t>;
using RowSpan = std::span<std::int64_t>;
using ConstRowSpan = std::span<const std::int64_t>;
using Matrix = std::vector<Row>;

constexpr auto isZero = std::bind_front(std::equal_to{}, 0);

constexpr void eliminateRowWith(Row& target, const Row& pivot, std::size_t pivotIndex)
{
    if (target[pivotIndex] == 0)
        return;

    auto gcd = std::gcd(pivot[pivotIndex], target[pivotIndex]);

    auto targetMultiplier = target[pivotIndex] / gcd;
    auto pivotMultiplier = pivot[pivotIndex] / gcd;
    auto size = std::size(target) - pivotIndex;
    for (auto [targetElement, pivotElement] : rng::views::zip(  //
             std::span{iterator::nth(target, pivotIndex), size},
             std::span{iterator::nth(pivot, pivotIndex), size}))
    {
        targetElement = targetElement * pivotMultiplier - pivotElement * targetMultiplier;
    }
}

constexpr void swapColumn(Matrix& target, std::size_t lhs, std::size_t rhs)
{
    for (auto& row : target)
        std::swap(row[lhs], row[rhs]);
}

constexpr auto gaussianElimination(Matrix& matrix)
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


    for (std::int64_t workSize = height, i = 0; i < workSize; ++i)
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
            std::abort();  // brute force will not handle this, need a better implementation

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

constexpr void forEachFreeVariable(ConstRowSpan multipliers,
                                   ConstRowSpan limits,
                                   std::int64_t target,
                                   auto function)
{
    // It was nice to implement this as std::generator, but performance is terrible.
    // I think a great use case for coroutines is cases when I/O is a bottleneck,
    // or not very performant parsers, when it doesn't make a difference.
    std::array<std::int64_t, maxSize> workingResult{};
    auto dfs = [&](this auto&& self, std::size_t i, std::int64_t rem)
    {
        if (const auto size = std::size(multipliers); i == size)
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

constexpr void numberOfPresses(Matrix& matrix, ConstRowSpan freeVariables, auto andThen)
{
    // Backtracking variables evaluation for given free variables solution
    std::array<std::int64_t, maxSize> solution{};
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
    andThen({std::begin(solution), width});
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
        {iterator::nth(matrix[diagonal], bruteForceStart), bruteForceSize},
        {iterator::nth(limits, bruteForceStart), bruteForceSize},
        matrix[diagonal].back(),
        [&](auto foundSolution)
        {
            numberOfPresses(  //
                matrix,
                foundSolution,
                [&](ConstRowSpan solution)
                { min = std::min(min, algorithm::sum(solution)); });
        });

    trace("{}", min);
    return min;
}

constexpr auto solve2(std::span<const MachineConfiguration> input)
{
    return algorithm::sum(input | rv::transform(solve));
}

constexpr MachineConfiguration parseInputLine(std::string_view line)
{
    auto [_1, _2, buttons, joltages] =
        ctre::match<R"(\[([\.#]+)\] (\(.*\)) \{(.*)\})">(line);
    auto joltageValues =
        ctre::search_all<R"(\d+)">(joltages.to_view())
        | rv::transform([](auto match) { return match.to_number(); });

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
            });
    return {joltageValues | rng::to<Container>(),
            buttonsImpact | rng::to<std::vector>()};
}

static_assert(
    solve(parseInputLine("[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}")) == 10);
static_assert(
    solve(parseInputLine(  //
        "[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}"))
    == 12);
static_assert(
    solve(parseInputLine(  //
        "[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}"))
    == 11);

static_assert(
    []
    {
        // part of an actual input
        auto input = std::to_array<std::string_view>({
            // clang-format off
"[.#.###] (0,1,2) (0,2,4,5) (3,5) (2,4,5) (0,1,3,4) (0,2,4) (5) {21,18,30,14,34,40}",
"[.##.##.#] (0,4,5,6) (1,3,6) (0,1,2,3,4,5,6) (0,3,6,7) (1,2,3,4,6,7) (1,2,4,5) {206,71,52,235,56,42,239,196}",
"[..##] (2,3) (0,3) (1,2) (0,2) {37,13,45,31}",
"[##......] (3) (0,3,4,5,7) (2,3,7) (0,2,5) (1,2,4,5,7) (4,6,7) (0,1,2,4,5,6) (0,2,3,4,6) (3,4,6) {29,11,38,45,45,33,27,54}",
"[#.....##] (5,7) (3,6) (0,2,3,4,6,7) (2,4) (0,6,7) (1,3,6,7) (1,2,5,6,7) {17,27,16,43,6,30,64,64}",
"[##.#.#.#] (0,3,4,5,6) (3,5,6,7) (0,2,7) (0,1,6,7) (1,2,3,4,6,7) (0,3,4,7) (0,1,2,4,5,6,7) (0,1,3,5,7) (0,1,2,4,5) (0,3) {80,74,41,62,60,59,56,88}",
"[#.#.] (0,2) (2,3) (0,3) (1,3) (3) (1,2) {26,7,25,27}",
"[#..#####..] (4,5,7,8,9) (1,5,6,7,8,9) (1,7,9) (5) (0,2,3,6) (4,5,7) (0,3) (0,1,3,4,5,6) (2,6,8) (4,5) (6,8) (1,2,4,5,6,7,8,9) {17,30,9,17,49,84,47,62,63,47}",
"[.#..###...] (0,2,3,4,6,7,8) (0,3,5,6,7,8) (1,5,7) (0,1,3,5,6,7) (0,2,3,4,6,8) (1,2,3,4,5,6,8,9) (2,6,7) (0,1,3,4,8) (7) (0,1,2,4,7,8) {48,25,54,58,48,22,64,47,53,11}",
            // clang-format on
        });
        return algorithm::sum(input | rv::transform(parseInputLine)
                              | rv::transform(solve))
               == (58 + 257 + 63 + 81 + 84 + 97 + 43 + 109 + 72);
    }());

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
        configurations.push_back({parseInputLine(line)});
    aoc2025::time::Stopwatch<> stopwatch;
    fmt::println("day10.solution2: {}", solve2(configurations));  // 20142
    fmt::println("Time elapsed: {}", stopwatch.elapsed());        // 24ms
}
