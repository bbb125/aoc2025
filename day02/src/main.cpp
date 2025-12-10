#include "util/numeric.h"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <functional>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace aoc2025::day02
{
[[maybe_unused]] constexpr auto trace = false;
using Int = std::int64_t;
using Range = std::pair<Int, Int>;

constexpr auto isInvalid(Int value, int chunks)
{
    auto len = numerics::countDigits(value);
    if (len % chunks != 0)
        return false;

    auto chunk = len / chunks;
    auto chunkMask = numerics::pow10<Int>(chunk);
    auto firstChunk = value % chunkMask;
    value /= chunkMask;
    while (value)
    {
        auto nextChunk = value % chunkMask;
        if (nextChunk != firstChunk)
            return false;
        value /= chunkMask;
    }
    return true;
};

static_assert(isInvalid(1212, 2));
static_assert(isInvalid(123123, 2));
static_assert(isInvalid(123123123, 3));
static_assert(isInvalid(1111, 4));
static_assert(not isInvalid(1234, 2));
static_assert(not isInvalid(1234, 4));


constexpr auto solve1(std::span<const Range> numbers)
{
    namespace rv = std::views;

    auto solveForRange = [](const Range& rng)
    {
        return std::ranges::fold_left(  //
            rv::iota(rng.first, rng.second + 1)
                | rv::filter(std::bind_back(isInvalid, 2)),
            0ll,
            std::plus<>{});
    };
    return std::ranges::fold_left(numbers | std::views::transform(solveForRange),
                                  0LL,
                                  std::plus<>{});
}

static_assert(solve1(std::to_array(  //
                  {Range{11, 22},
                   {95, 115},
                   {998, 1012},
                   {1188511880, 1188511890},
                   {222220, 222224},
                   {1698522, 1698528},
                   {446443, 446449},
                   {38593856, 38593862},
                   {565653, 565659},
                   {824824821, 824824827},
                   {2121212118, 2121212124}}))
              == 1227775554);


constexpr auto solve2(std::span<const Range> numbers)
{
    namespace rv = std::views;

    auto solveForRange = [&](const Range& rng)
    {
        return std::ranges::fold_left(  //
            rv::iota(rng.first, rng.second + 1)
                | rv::filter(
                    [](auto value)
                    {
                        auto len = numerics::countDigits(value);
                        for (int chunks = 2; chunks <= len; ++chunks)
                        {
                            if (isInvalid(value, chunks))
                                return true;
                        }
                        return false;
                    }),
            0ll,
            std::plus<>{});
    };
    return std::ranges::fold_left(numbers | std::views::transform(solveForRange),
                                  0LL,
                                  std::plus<>{});
}

static_assert(solve2(std::to_array(  //
                  {Range{11, 22},
                   {95, 115},
                   {998, 1012},
                   {1188511880, 1188511890},
                   {222220, 222224},
                   {1698522, 1698528},
                   {446443, 446449},
                   {38593856, 38593862},
                   {565653, 565659},
                   {824824821, 824824827},
                   {2121212118, 2121212124}}))
              == 4174379265);

}  // namespace aoc2025::day02

int main()
{
    using namespace aoc2025::day02;
    std::ifstream file("./input.txt");
    std::string line;
    if (!file || !std::getline(file, line))
    {
        fmt::println("Failed to read input file");
        return 1;
    }
    auto input = line | std::views::split(',')
                 | std::views::transform(
                     [](auto&& rng)
                     {
                         auto s = rng | std::ranges::to<std::string>();
                         auto dashPos = s.find('-');
                         assert(dashPos != std::string::npos && "Invalid range format");
                         auto left = std::stoll(s.substr(0, dashPos));
                         auto right = std::stoll(s.substr(dashPos + 1));
                         return Range{left, right};
                     })
                 | std::ranges::to<std::vector>();

    fmt::println("day01.solution1: {}", solve1(input));
    fmt::println("day01.solution2: {}", solve2(input));
}