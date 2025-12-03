#include <fmt/format.h>
#include <fmt/ranges.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <concepts>
#include <fstream>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace aoc2025::numerics
{
// todo: move to a common place when needed
constexpr auto countDigits(std::integral auto number)
{
    if (number == 0)
        return 1;
    int count = 0;
    while (number != 0)
    {
        number /= 10;
        ++count;
    }
    return count;
}

static_assert(countDigits(0) == 1);
static_assert(countDigits(5) == 1);
static_assert(countDigits(10) == 2);
static_assert(countDigits(999) == 3);
static_assert(countDigits(1000) == 4);
static_assert(countDigits(-1000) == 4);

constexpr auto takeRightDigits(std::integral auto number, int digits)
{
    assert(digits >= 0 && "digits must be non-negative");
    assert(
        digits <= countDigits(number)
        && "digits must be less than or equal to number of digits in number");
    auto factor = 1;
    for (int i = 0; i < digits; ++i)
        factor *= 10;
    return number % factor;
}
static_assert(takeRightDigits(12345, 1) == 5);
static_assert(takeRightDigits(12345, 2) == 45);
static_assert(takeRightDigits(12345, 3) == 345);
static_assert(takeRightDigits(12345, 5) == 12345);

constexpr auto takeLeftDigits(std::integral auto number, int digits)
{
    assert(digits >= 0 && "digits must be non-negative");
    assert(
        digits <= countDigits(number)
        && "digits must be less than or equal to number of digits in number");
    auto totalDigits = countDigits(number);
    auto factor = 1;
    for (int i = 0; i < totalDigits - digits; ++i)
        factor *= 10;
    return number / factor;
}
static_assert(takeLeftDigits(12345, 1) == 1);
static_assert(takeLeftDigits(12345, 2) == 12);
static_assert(takeLeftDigits(12345, 3) == 123);
static_assert(takeLeftDigits(12345, 5) == 12345);

constexpr auto isOdd(std::integral auto number)
{
    return (number % 2) != 0;
}
static_assert(isOdd(1));
static_assert(not isOdd(2));
static_assert(isOdd(999));
static_assert(not isOdd(1000));
static_assert(not isOdd(0));

template <std::integral T>
constexpr T pow10(int exp)
{
    assert(exp >= 0 && "exponent must be non-negative");
    T result = 1;
    for (int i = 0; i < exp; ++i)
        result *= 10;
    return result;
}
static_assert(pow10<std::int64_t>(0) == 1);
static_assert(pow10<std::int64_t>(1) == 10);
static_assert(pow10<std::int64_t>(5) == 100000);

}  // namespace aoc2025::numerics

namespace aoc2025::day02
{
constexpr auto trace = false;
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
    if (!std::getline(file, line))
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