#include "util/trace.h"


#include <fmt/format.h>
#include <fmt/ranges.h>

#include <array>
#include <cassert>
#include <span>
#include <ranges>
#include <vector>
#include <fstream>

namespace aoc2025::day01
{
constexpr auto fullCycle = 100;
constexpr auto trace = diagnostic::makeTracer<false>();

struct RotationResult
{
    int rotations = 0;
    int newPos = 0;
};

constexpr RotationResult rotate(int pos, int val, int cycle = fullCycle)
{
    assert(pos >= 0 && pos < cycle && "position out of bounds");
    auto newPos = pos + val;
    return newPos <= 0 // todo: probably can be generalized with std::abs, but screw it
               ? RotationResult{(-newPos) / fullCycle + int{pos != 0},
                                (fullCycle - ((-newPos) % fullCycle)) % fullCycle}
               : RotationResult{newPos / fullCycle, newPos % fullCycle};
}

constexpr int solve1(std::span<const int> input, int start = 50)
{
    auto [_, count] = std::ranges::fold_left(  //
        input,
        std::tuple{start, int{start == 0}},
        [](auto acc, auto v)
        {
            auto [pos, counter] = acc;
            auto [_, newPos] = rotate(pos, v);
            return std::tuple{newPos, counter + (newPos == 0)};
        });
    return count;
}


static_assert(solve1(
                  std::array{
                      -49,
                      -200,
                      50,
                      -150,
                  },
                  49)
              == 3);
static_assert(solve1(std::array{-68, -30, 48, -5, 60, -55, -1, -99, 14, -82}) == 3);


constexpr int solve2(std::span<const int> input, int start = 50)
{
    auto [_, count] = std::ranges::fold_left(  //
        input,
        std::tuple{start, int{start == 0}},
        [](auto acc, auto v)
        {
            auto [pos, counter] = acc;
            auto [rotations, newPos] = rotate(pos, v);

            trace("{} + {} = {}", acc, v, std::tuple{newPos, counter + rotations});
            return std::tuple{newPos, counter + rotations};
        });
    return count;
}

static_assert(solve2(std::array{-68, -30, 48, -5, 60, -55, -1, -99, 14, -82}) == 6);
}  // namespace aoc2025::day01

int main()
{
    using namespace aoc2025::day01;
    std::ifstream file("./input1.txt");
    if (!file)
    {
        fmt::println("Failed to read input file");
        return 1;
    }
    auto input = std::views::istream<std::string>(file)
                 | std::views::transform(
                     [](const std::string& line)
                     {
                         switch (line[0])
                         {
                         case 'L':
                             return -std::stoi(line.substr(1));
                         case 'R':
                             return std::stoi(line.substr(1));
                         default:
                             std::unreachable();
                         }
                     })
                 | std::ranges::to<std::vector>();
    trace("inputs: {}", input);
    fmt::println("day01.1: {}", solve1(input));
    fmt::println("day01.2: {}", solve2(input));  // 5933
    return 0;
}