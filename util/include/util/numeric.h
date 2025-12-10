#pragma once

#include <concepts>
#include <cassert>
#include <cstdint>

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
