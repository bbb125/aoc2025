#include "util/algorithm.h"

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <string_view>
#include <span>
#include <ranges>
#include <fstream>
#include <vector>
#include <functional>

namespace aoc2025::day03
{
constexpr std::uint8_t toInt(char ch)
{
    assert(ch >= '0' && ch <= '9' && "character must be a digit");
    return static_cast<std::uint8_t>(ch - '0');
}

static_assert(toInt('0') == 0);
static_assert(toInt('5') == 5);
static_assert(toInt('9') == 9);


// We will work with string-view for simpler input
template <class T>
concept StringLike = std::same_as<std::remove_cvref_t<T>, std::string_view>
                     || std::constructible_from<std::string_view, T>;

// Initial solution for #1: not used anymore
constexpr std::uint64_t maxJoltage(const StringLike auto& bank)
{
    std::string_view sv = bank;
    assert(std::size(sv) >= 2 && "bank must have at least two digits");
    auto first = std::max_element(std::begin(sv), std::prev(std::rbegin(sv).base()));
    auto second = std::max_element(std::next(first), std::end(sv));
    return toInt(*first) * 10 + toInt(*second);
}

static_assert(maxJoltage("123456789") == 89);
static_assert(maxJoltage("163841689525773") == 97);
static_assert(maxJoltage("0000000000") == 0);
static_assert(maxJoltage("811111111111119") == 89);


constexpr std::uint64_t maxJoltageN(const StringLike auto& bank, std::uint8_t n)
{
    std::string_view sv = bank;
    assert(std::size(sv) >= n && "bank is too small");
    auto first = std::begin(sv);
    auto last = std::prev(std::rbegin(sv).base(), n - 1);
    std::uint64_t result = 0;
    for (auto i = 0; i < n; ++i, last = std::next(last))
    {
        auto it = std::max_element(first, last);
        result = result * 10 + toInt(*it);
        first = std::next(it);
    }
    return result;
}
static_assert(maxJoltageN("987654321111111", 12) == 987654321111);
static_assert(maxJoltageN("811111111111119", 12) == 811111111119);
static_assert(maxJoltageN("234234234234278", 12) == 434234234278);
static_assert(maxJoltageN("818181911112111", 12) == 888911112111);

template <StringLike T>
constexpr std::uint64_t solve1(std::span<T> banks)
{
    return algorithm::sum(
        banks | std::views::transform(std::bind_back(maxJoltageN<T>, 2)));
}

static_assert(
    []
    {
        using std::string_view_literals::operator""sv;

        std::array input{"987654321111111"sv,
                         "811111111111119"sv,
                         "234234234234278"sv,
                         "818181911112111"sv};
        return solve1(std::span{std::begin(input), std::end(input)});
    }()
    == 98 + 89 + 78 + 92);


template <StringLike T>
constexpr std::uint64_t solve2(std::span<T> banks)
{
    return std::ranges::fold_left(  //
        banks | std::views::transform(std::bind_back(maxJoltageN<T>, 12)),
        0ull,
        std::plus<>{});
}

static_assert(
    []
    {
        using std::string_view_literals::operator""sv;

        std::array input{"987654321111111"sv,
                         "811111111111119"sv,
                         "234234234234278"sv,
                         "818181911112111"sv};
        return solve2(std::span{std::begin(input), std::end(input)});
    }()
    == 987654321111 + 811111111119 + 434234234278 + 888911112111);

}  // namespace aoc2025::day03
int main()
{
    using namespace aoc2025::day03;
    std::ifstream file("./input.txt");
    if (!file)
    {
        fmt::println("Failed to read input file");
        return 1;
    }
    auto input = std::views::istream<std::string>(file)
                 | std::ranges::to<std::vector<std::string>>();

    fmt::println("day03.01: {}",
                 solve1(std::span{std::begin(input), std::end(input)}));
    fmt::println("day03.02: {}",
                 solve2(std::span{std::begin(input), std::end(input)}));
}